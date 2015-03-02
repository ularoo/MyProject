#include "protocol.h"
#include "debug.h"
#include "main.h"
#include "crcmodel.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#define service_color_lte700 		"#00CCFF"
#define service_color_cellular 	"#FFFF66"
#define service_color_pcs 		"#FFCC33"
#define service_color_aws 		"#FF99CC"
#define service_color_td_lte	"#FFCC66"
#ifdef iDEN800_FUNC_ADD	/* iden800 */
#define service_color_iden		"#66CC99"
#endif /* iDEN800_FUNC_ADD */
#define service_color_none 		"#CCCCCC"
#define alarm_color_red			"red"
#define alarm_color_yellow		"yellow"
#define alarm_color_green		"lime"
#define alarm_color_white		"white"
#define table_color_header		"#AAAAAA"
#define table_color_body			"#CCCCCC"
#define str_enabled				"enabled"
#define str_disabled				"disabled"
#define str_service_lte700		"700"//"LTE700"
#define str_service_cellular		"850"//"Cellular"
#define str_service_pcs			"1900"//"PCS"
#define str_service_aws			"2100"//"AWS"
#ifdef iDEN800_FUNC_ADD	/* iden800 */
#define str_service_iden		"800"//"iDEN"
#endif /* iDEN800_FUNC_ADD */
#define str_service_td_lte		"2500"
#define str_service_none			"---"
#define str_service_unknown		"UNKNOWN"
#define str_mount				"Mounted"
#define str_blank					"Blank"

const char* service_name[]=
{
	"---",	//"---",		// 0:blank
	"700",	//"LTE700",	// 1:LTE700
	"850",	//"Cellular",	// 2:Cellular
	"800",	//"iDEN",		// 3:iDEN
	"1900",	//"PCS",		// 4:PCS
	"2100",	//"AWS",		// 5:AWS
	"2600",	//"WiMAX",	// 6:WiMAX
	"2600L",	//"LTE2600",	// 7:LTE 2600
};

const char* service_color[]=
{
	"#CCCCCC",		// 0:blank
	"#00CCFF",		// 1:LTE700
	"#FFFF66",	// 2:Cellular
	"#66CC99",		// 3:iDEN
	"#FFCC33",		// 4:PCS
	"#FF99CC",		// 5:AWS
	"#CCFFFF",		// 6:WiMAX
	"#CC9900",	// 7:LTE 2600
};

void tree(int is_config)//is_config=0:status, 1:config
{
	struct protocol_web_status protocol_status;
	if(get_status(&protocol_status)<0) return;

#ifdef MOUNT_TEST
	protocol_status.status[0].svc_info=1;// 1:LTE700
	protocol_status.status[1].svc_info=2;// 2:Cellular
	protocol_status.status[2].svc_info=4;// 4:PCS
	protocol_status.status[3].svc_info=3;// 3:iDEN

	protocol_status.status[0].amp_capacity=24;
	protocol_status.status[1].amp_capacity=24;
	protocol_status.status[2].amp_capacity=33;
	protocol_status.status[3].amp_capacity=30;

	sprintf((char*)protocol_status.status[0].serial_no, "12345");
	sprintf((char*)protocol_status.status[1].serial_no, "23456");
	sprintf((char*)protocol_status.status[2].serial_no, "34567");
	sprintf((char*)protocol_status.status[3].serial_no, "45678");
#endif

	int i;
	char *p_color_service[4]={service_color_none};
	char *p_color_rcu_fan=alarm_color_white;
	char *p_color_rcu_voltage=alarm_color_white;
	char *p_color_rcu_current=alarm_color_white;
	char *p_color_rcu_reset=alarm_color_white;
	char *p_color_alarm[4]={alarm_color_white};
	char *p_color_shutdown[4]={alarm_color_white};
	char *p_str_enabled[4]={str_enabled};
	char *p_str_service[4]={str_service_none};
	char *p_str_mount[4]={str_blank};

	if(protocol_status.alarm.alarm_fan==0) 
		p_color_rcu_fan=alarm_color_green;
	else 
		p_color_rcu_fan=alarm_color_red;

	if(protocol_status.alarm.alarm_voltage==0) 
		p_color_rcu_voltage=alarm_color_green;
	else 
		p_color_rcu_voltage=alarm_color_red;

	if(protocol_status.alarm.alarm_current==0) 
		p_color_rcu_current=alarm_color_green;
	else 
		p_color_rcu_current=alarm_color_red;

	if(protocol_status.alarm.alarm_reset==0) 
		p_color_rcu_reset=alarm_color_green;
	else 
		p_color_rcu_reset=alarm_color_red;

	for(i=0;i<4;i++)
	{
		if(protocol_status.status[i].svc_info==0)
		{
				p_color_alarm[i]=alarm_color_white;
				p_color_shutdown[i]=alarm_color_white;
		}
		else
		{
			if(protocol_status.alarm.slot[i].alarm_led==0) 
				p_color_alarm[i]=alarm_color_green;
//			else if(protocol_status.alarm.slot[i].alarm_led==1) 
//				p_color_alarm[i]=alarm_color_yellow;
			else 
				p_color_alarm[i]=alarm_color_red;

			if(protocol_status.alarm.slot[i].shutdown_led==0) 
				p_color_shutdown[i]=alarm_color_green;
//			else if(protocol_status.alarm.slot[i].shutdown_led==1) 
//				p_color_shutdown[i]=alarm_color_yellow;
			else 
				p_color_shutdown[i]=alarm_color_red;
		}
	}	

	for(i=0;i<4;i++)
	{
		switch(protocol_status.status[i].svc_info)
		{
			case 0:	//blank, 
				p_color_service[i]=service_color_none; 	
				p_str_service[i]=str_service_none;	
				p_str_mount[i]=str_blank;	
				p_str_enabled[i]=str_disabled;	
				break;	
			case 1:	//LTE700, 
				p_color_service[i]=service_color_lte700; 	
				p_str_service[i]=str_service_lte700;	
				p_str_mount[i]=str_mount;	
				p_str_enabled[i]=str_enabled;	
				break;	
			case 2:	//Cellular, 
				p_color_service[i]=service_color_cellular; 	
				p_str_service[i]=str_service_cellular;	
				p_str_mount[i]=str_mount;	
				p_str_enabled[i]=str_enabled;	
				break;	
#ifdef iDEN800_FUNC_ADD	/* iden800 */
			case 3:	//iDEN, 
				p_color_service[i]=service_color_iden; 	
				p_str_service[i]=str_service_iden;	
				p_str_mount[i]=str_mount;	
				p_str_enabled[i]=str_enabled;	
				break;	
#endif /* iDEN800_FUNC_ADD */
			case 4:	//PCS, 
				p_color_service[i]=service_color_pcs; 		
				p_str_service[i]=str_service_pcs;		
				p_str_mount[i]=str_mount;	
				p_str_enabled[i]=str_enabled;	
				break;	
			case 5:	//AWS, 
				p_color_service[i]=service_color_aws; 	
				p_str_service[i]=str_service_aws;	
				p_str_mount[i]=str_mount;	
				p_str_enabled[i]=str_enabled;	
				break;	
			case 6:	//TD-LTE, 
				p_color_service[i]=service_color_td_lte; 	
				p_str_service[i]=str_service_td_lte;	
				p_str_mount[i]=str_mount;	
				p_str_enabled[i]=str_enabled;	
				break;	
/*
			case 6:	//WiMAX, 
				p_color_service[i]=service_color_wimax; 	
				p_str_service[i]=str_service_wimax;	
				p_str_mount[i]=str_mount;	
				p_str_enabled[i]=str_enabled;	
				break;	
			case 7:	//LTE 2600
				p_color_service[i]=service_color_lte2600; 	
				p_str_service[i]=str_service_lte2600;	
				p_str_mount[i]=str_mount;	
				p_str_enabled[i]=str_enabled;	
				break;	
*/
			default:
				p_color_service[i]=service_color_none; 	
				p_str_service[i]=str_service_unknown;	
				p_str_mount[i]=str_mount;	
				p_str_enabled[i]=str_enabled;	
				break;	
		}
	}

	//==============================================================
	html_page_head(0);  // arg==requiredfrom true(1), false(0)
	printf(	"<!-- \n");
	create_ajax_code();

	printf(	"function logout()\n");
	printf(	"{\n");
	printf(	"	if (document.cookie != \"\") \n");
	printf(	"	{\n");
	printf(	"        thisCookie = document.cookie.split(\"; \")\n");
	printf(	"        expireDate = new Date\n");
	printf(	"        expireDate.setDate(expireDate.getDate()-1)\n");
	printf(	"        for (i=0; i<thisCookie.length; i++) \n");
	printf(	"		{\n");
	printf(	"			document.cookie = thisCookie + \";expires=\" + expireDate.toGMTString()\n");
	printf(	"		}\n");
	printf(	"	}\n");
	printf(	"	location.replace(\"html.cgi?function=logout\");\n");
	printf(	"}\n");
	printf(	"\n");

	printf(	"function reload() {\n");
	printf(	"	sendRequest(status_load,'','GET','/web_tree',true,true);\n");
	printf(	"	time_id = setTimeout(\"reload()\", 60000);\n");
	printf(	"}\n");
	printf(	"\n");

	printf(	"function status_load(oj)\n");
	printf(	"{\n");
	printf(	"	var detect = decodeURIComponent(oj.responseText);\n");
	printf(	" \n");
	printf(	"	var s90=\"led_alarm_total\";\n");
	printf(	"	var i90=detect.indexOf(s90)+s90.length+1;\n");
	printf(	"	var val_alarm_led = detect.substring(i90, i90+1);\n");//i00 to i00; 1 character
	printf(	"\n");
	printf(	"	var s91=\"led_shutdown_total\";\n");
	printf(	"	var i91=detect.indexOf(s91)+s91.length+1;\n");
	printf(	"	var val_shutdown_led = detect.substring(i91, i91+1);\n");//0
	printf(	"\n");
	printf(	"	var s00 = \"alarm_rcu_fan\";\n");
	printf(	"	var i00 = detect.indexOf(s00) + s00.length + 1;\n");
	printf(	"	var val_alarm_rcu_fan = detect.substring(i00, i00+1);\n");
	printf(	"\n");
	printf(	"	var s01 = \"alarm_rcu_voltage\";\n");
	printf(	"	var i01 = detect.indexOf(s01) + s01.length + 1;\n");
	printf(	"	var val_alarm_rcu_voltage = detect.substring(i01, i01+1);\n");
	printf(	"\n");
	printf(	"	var s02 = \"alarm_rcu_current\";\n");
	printf(	"	var i02 = detect.indexOf(s02) + s02.length + 1;\n");
	printf(	"	var val_alarm_rcu_current = detect.substring(i02, i02+1);\n");
	printf(	"\n");
	printf(	"	var s03 = \"alarm_rcu_reset\";\n");
	printf(	"	var i03 = detect.indexOf(s03) + s03.length + 1;\n");
	printf(	"	var val_alarm_rcu_reset = detect.substring(i03, i03+1);\n");
	printf(	"\n");
	printf(	"	var s04 = \"alarm_amp0_alarm\";\n");
	printf(	"	var i04 = detect.indexOf(s04) + s04.length + 1;\n");
	printf(	"	var val_alarm_amp0_alarm = detect.substring(i04, i04+1);\n");
	printf(	"\n");
	printf(	"	var s05 = \"alarm_amp1_alarm\";\n");
	printf(	"	var i05 = detect.indexOf(s05) + s05.length + 1;\n");
	printf(	"	var val_alarm_amp1_alarm = detect.substring(i05, i05+1);\n");
	printf(	"\n");
	printf(	"	var s06 = \"alarm_amp2_alarm\";\n");
	printf(	"	var i06 = detect.indexOf(s06) + s06.length + 1;\n");
	printf(	"	var val_alarm_amp2_alarm = detect.substring(i06, i06+1);\n");
	printf(	"\n");
	printf(	"	var s07 = \"alarm_amp3_alarm\";\n");
	printf(	"	var i07 = detect.indexOf(s07) + s07.length + 1;\n");
	printf(	"	var val_alarm_amp3_alarm = detect.substring(i07, i07+1);\n");
	printf(	"\n");
	printf(	"	var s08 = \"alarm_amp0_shutdown\";\n");
	printf(	"	var i08 = detect.indexOf(s08) + s08.length + 1;\n");
	printf(	"	var val_alarm_amp0_shutdown = detect.substring(i08, i08+1);\n");
	printf(	"\n");
	printf(	"	var s09 = \"alarm_amp1_shutdown\";\n");
	printf(	"	var i09 = detect.indexOf(s09) + s09.length + 1;\n");
	printf(	"	var val_alarm_amp1_shutdown = detect.substring(i09, i09+1);\n");
	printf(	"\n");
	printf(	"	var s10 = \"alarm_amp2_shutdown\";\n");
	printf(	"	var i10 = detect.indexOf(s10) + s10.length + 1;\n");
	printf(	"	var val_alarm_amp2_shutdown = detect.substring(i11, i10+1);\n");
	printf(	"\n");
	printf(	"	var s11 = \"alarm_amp3_shutdown\";\n");
	printf(	"	var i11 = detect.indexOf(s11) + s11.length + 1;\n");
	printf(	"	var val_alarm_amp3_shutdown = detect.substring(i11, i11+1);\n");
	printf(	"\n");
	printf(	"	var s12 = \"amp0_mount\";\n");
	printf(	"	var i12 = detect.indexOf(s12) + s12.length + 1;\n");
	printf(	"	var val_amp0_mount = detect.substring(i12, i12+1);\n");
	printf(	"\n");
	printf(	"	var s13 = \"amp1_mount\";\n");
	printf(	"	var i13 = detect.indexOf(s13) + s13.length + 1;\n");
	printf(	"	var val_amp1_mount = detect.substring(i13, i13+1);\n");
	printf(	"\n");
	printf(	"	var s14 = \"amp2_mount\";\n");
	printf(	"	var i14 = detect.indexOf(s14) + s14.length + 1;\n");
	printf(	"	var val_amp2_mount = detect.substring(i14, i14+1);\n");
	printf(	"\n");
	printf(	"	var s15 = \"amp3_mount\";\n");
	printf(	"	var i15 = detect.indexOf(s15) + s15.length + 1;\n");
	printf(	"	var val_amp3_mount = detect.substring(i15, i15+1);\n");
	printf(	"\n");
	printf(	"	var s16 = \"amp0_service\";\n");
	printf(	"	var i16 = detect.indexOf(s16) + s16.length + 1;\n");
	printf(	"	var val_amp0_service = detect.substring(i16, i16+1);\n");
	printf(	"\n");
	printf(	"	var s17 = \"amp1_sevice\";\n");
	printf(	"	var i17 = detect.indexOf(s17) + s17.length + 1;\n");
	printf(	"	var val_amp1_sevice = detect.substring(i17, i17+1);\n");
	printf(	"\n");
	printf(	"	var s18 = \"amp2_service\";\n");
	printf(	"	var i18 = detect.indexOf(s18) + s18.length + 1;\n");
	printf(	"	var val_amp2_service = detect.substring(i18, i18+1);\n");
	printf(	"\n");
	printf(	"	var s19 = \"amp3_service\";\n");
	printf(	"	var i19 = detect.indexOf(s19) + s19.length + 1;\n");
	printf(	"	var val_amp3_service = detect.substring(i19, i19+1);\n");
	printf(	"\n");
 	printf(	"	document.form0.alarm_status.style.background = (val_alarm_led>=1)? \"red\": \"lime\";\n");
	printf(	"	document.form0.shutdown_status.style.background = (val_shutdown_led>=1)? \"red\": \"lime\";\n");
	printf(	"	document.form1.txt_alarm_rcu_fan.style.background = (val_alarm_rcu_fan>=1)? \"red\": \"lime\";\n");
	printf(	"	document.form1.txt_alarm_rcu_voltage.style.background = (val_alarm_rcu_voltage>=1)? \"red\": \"lime\";\n");
	printf(	"	document.form1.txt_alarm_rcu_current.style.background = (val_alarm_rcu_current>=1)? \"red\": \"lime\";\n");
	printf(	"	document.form1.txt_alarm_rcu_reset.style.background = (val_alarm_rcu_reset>=1)? \"red\": \"lime\";\n");
	printf(	"\n");
#ifdef MOUNT_TEST
	printf(	"	val_amp0_mount=%d;\n", (protocol_status.status[0].svc_info)? 1: 0);
	printf(	"	val_amp1_mount=%d;\n", (protocol_status.status[1].svc_info)? 1: 0);
	printf(	"	val_amp2_mount=%d;\n", (protocol_status.status[2].svc_info)? 1: 0);
	printf(	"	val_amp3_mount=%d;\n", (protocol_status.status[3].svc_info)? 1: 0);
#endif
	printf(	"	if(val_amp0_mount==0)\n");
	printf(	"	{\n");
	printf(	"		document.form1.txt_alarm_amp0_alarm.style.background = \"white\";\n");
	printf(	"		document.form1.txt_alarm_amp0_shutdown.style.background = \"white\";\n");
	printf(	"	}else{\n");
	printf(	"		document.form1.txt_alarm_amp0_alarm.style.background = (val_alarm_amp0_alarm==1)? \"red\": \"lime\";\n");
	printf(	"		document.form1.txt_alarm_amp0_shutdown.style.background = (val_alarm_amp0_shutdown==1)? \"red\": \"lime\";\n");
	printf(	"	}\n");
	printf(	"\n");
	printf(	"	if(val_amp1_mount==0)\n");
	printf(	"	{\n");
	printf(	"		document.form1.txt_alarm_amp1_alarm.style.background = \"white\";\n");
	printf(	"		document.form1.txt_alarm_amp1_shutdown.style.background = \"white\";\n");
	printf(	"	}else{\n");
	printf(	"		document.form1.txt_alarm_amp1_alarm.style.background = (val_alarm_amp1_alarm==1)? \"red\": \"lime\";\n");
	printf(	"		document.form1.txt_alarm_amp1_shutdown.style.background = (val_alarm_amp1_shutdown==1)? \"red\": \"lime\";\n");
	printf(	"	}\n");
	printf(	"\n");
	printf(	"	if(val_amp2_mount==0)\n");
	printf(	"	{\n");
	printf(	"		document.form1.txt_alarm_amp2_alarm.style.background = \"white\";\n");
	printf(	"		document.form1.txt_alarm_amp2_shutdown.style.background = \"white\";\n");
	printf(	"	}\n");
	printf(	"	else \n");
	printf(	"	{\n");
	printf(	"		document.form1.txt_alarm_amp2_alarm.style.background = (val_alarm_amp2_alarm==1)? \"red\": \"lime\";\n");
	printf(	"		document.form1.txt_alarm_amp2_shutdown.style.background = (val_alarm_amp2_shutdown==1)? \"red\": \"lime\";\n");
	printf(	"	}\n");
	printf(	"\n");
	printf(	"	if(val_amp3_mount==0)\n");
	printf(	"	{\n");
	printf(	"		document.form1.txt_alarm_amp3_alarm.style.background = \"white\";\n");
	printf(	"		document.form1.txt_alarm_amp3_shutdown.style.background = \"white\";\n");
	printf(	"	}else{\n");
	printf(	"		document.form1.txt_alarm_amp3_alarm.style.background = (val_alarm_amp3_alarm==1)? \"red\": \"lime\";\n");
	printf(	"		document.form1.txt_alarm_amp3_shutdown.style.background = (val_alarm_amp3_shutdown==1)? \"red\": \"lime\";\n");
	printf(	"	}\n");
	printf(	"\n");

	printf("	var mount_info=0;\n");
	printf("	if(val_amp0_mount==1) mount_info |= (1<<0);\n");
	printf("	if(val_amp1_mount==1) mount_info |= (1<<1);\n");
	printf("	if(val_amp2_mount==1) mount_info |= (1<<2);\n");
	printf("	if(val_amp3_mount==1) mount_info |= (1<<3);\n");

	printf("	if(document.form1.first_load.value==1)\n");
	printf("	{\n");
	printf("		document.form1.first_load.value=0;\n");
	printf("	}else if(mount_info!=document.form1.monitor_mount.value)\n");
	printf("	{\n");
//	printf("		alert(\"reload\");\n");
	printf("		window.location.reload();\n");
	printf("	}\n");
	printf("	document.form1.monitor_mount.value = mount_info;\n");

	printf(	"	clearTimeout(time_id);\n");
	printf(	"	time_id = setTimeout(\"reload()\", 3000);\n");
	printf(	"}\n");
	printf(	"\n");
	printf(	"//--> \n");
	printf(	"</script>\n");
	printf(	"\n");
	printf(	"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf(	"\n");
	printf(	"</head>\n");
	printf(	"\n");
	printf(	"<body bgcolor=\"#FFFFFF\" text=\"#000000\" onLoad = \"reload()\">\n");

//==============================================================
	html_menu_head(&protocol_status, (is_config)? "RF Configuration": "RF Status");	//neil: simplify

//--------------------------------------------------------------------------------
	printf("<form name = \"form1\">\n");
	printf("<table cellspacing=\"0\" cellspacing=\"0\" border=\"0\" width=\"740\">\n");
	printf("	<tr height=22 style='height:16.5pt'>\n");
	printf("		<td height=\"50\" colspan=\"12\" width=\"529\" rowspan=\"2\" bgcolor=\"#CCCCCC\" style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\">\n");
	printf("		<p align=\"left\"><font size=\"5\">&nbsp;<b>SmartCell Configuration - %s</b></font></p></td>\n", (is_config)? "RF Configuration": "RF Status");
	printf("		<td width=\"136\" bgcolor=\"#CCCCCC\" colspan=\"3\" style=\"border-width:2; border-top-style:ridge; border-right-style:ridge; border-bottom-style:hidden; border-left-style:hidden;\">&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=22>\n");
	printf("		<td colspan=3 bgcolor=\"#CCCCCC\" style=\"border-width:2; border-top-style:hidden; border-right-style:ridge; border-bottom-style:ridge; border-left-style:hidden;\">\n");
	printf("		<p align=\"right\"><input type=\"button\" style=\"font-size: 10pt; font-family: Arial, Helvetica, sans-serif\" name=\"renew\" value=\"Configuration\" onclick=\"location='html.cgi?function=%s'\"></p></td>\n", (is_config)? "tree_rfconfig": "tree_status");
	printf("	</tr>\n");
	printf("	<tr height=22 style='height:16.5pt'>\n");
	printf("		<td height=22 style=\"border-style:hidden; height:16.5pt;\" colspan=\"15\">&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");

	printf("<table cellspacing=\"0\" cellspacing=\"0\" border=\"0\" width=\"740\">\n");
	printf("	<tr height=22 style='height:16.5pt' bgcolor=\"#AAAAAA\">\n");
	printf("		<td colspan=3 height=22 style=\"border-width:2; border-style:ridge; height:16.5pt;\" width=\"100\">\n");
	printf("			<p align=\"center\" style=\"font-size: 10pt; font-weight:bold; color:#222222; font-family: Arial, Helvetica, sans-serif\">NMS</p></td>\n");
	printf("		<td colspan=3 style=\"border-width:2; border-style:ridge;\" width=\"160\">\n");
	printf("			<p align=\"center\"><input type=\"button\" style=\"width:100%;height:30px;font-size:10pt; font-weight:bold; color:#222222; font-family: Arial, Helvetica, sans-serif\" name=\"amp0link\" value=\"AMP 1\" %s onclick=\"location='html.cgi?function=%s'\"></p></td>\n", p_str_enabled[0], (is_config)? "rfconfig": "status");
	printf("		<td colspan=3 style=\"border-width:2; border-style:ridge;\" width=\"160\">\n");
	printf("			<p align=\"center\"><input type=\"button\" style=\"width:100%;height:30px;font-size:10pt; font-weight:bold; color:#222222; font-family: Arial, Helvetica, sans-serif\" name=\"amp1link\" value=\"AMP 2\" %s onclick=\"location='html.cgi?function=%s'\"></p></td>\n", p_str_enabled[1], (is_config)? "rfconfig_1": "status_1");
	printf("		<td colspan=3 style=\"border-width:2; border-style:ridge;\" width=\"160\">\n");
	printf("			<p align=\"center\"><input type=\"button\" style=\"width:100%;height:30px;font-size:10pt; font-weight:bold; color:#222222; font-family: Arial, Helvetica, sans-serif\" name=\"amp2link\" value=\"AMP 3\" %s onclick=\"location='html.cgi?function=%s'\"></p></td>\n", p_str_enabled[2], (is_config)? "rfconfig_2": "status_2");
	printf("		<td colspan=3 style=\"border-width:2; border-style:ridge;\" width=\"160\">\n");
	printf("			<p align=\"center\"><input type=\"button\" style=\"width:100%;height:30px;font-size:10pt; font-weight:bold; color:#222222; font-family: Arial, Helvetica, sans-serif\" name=\"amp3link\" value=\"AMP 4\" %s onclick=\"location='html.cgi?function=%s'\"></p></td>\n", p_str_enabled[3], (is_config)? "rfconfig_3": "status_3");
	printf("	</tr>\n");
	printf("	<tr height=22 style='height:16.5pt'>\n");
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"	 width=\"10\"  bgcolor=\"#CCCCCC\" >&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" width=\"80\"  bgcolor=\"#CCCCCC\" >&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" width=\"10\"  bgcolor=\"#CCCCCC\" >&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  width=\"20\"  bgcolor=\"%s\" >&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" width=\"120\" bgcolor=\"%s\" >&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" width=\"20\"  bgcolor=\"%s\" >&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  width=\"20\"  bgcolor=\"%s\" >&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" width=\"120\" bgcolor=\"%s\" >&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" width=\"20\"  bgcolor=\"%s\" >&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  width=\"20\"  bgcolor=\"%s\" >&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" width=\"120\" bgcolor=\"%s\" >&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" width=\"20\"  bgcolor=\"%s\" >&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  width=\"20\"  bgcolor=\"%s\" >&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" width=\"120\" bgcolor=\"%s\" >&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:2; border-top-style:ridge; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" width=\"20\"  bgcolor=\"%s\" >&nbsp;</td>\n", 	p_color_service[3]);
	printf("	</tr>\n");
	printf("	<tr height=22 style='height:16.5pt'>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
///
	printf("		<td style=\"border-width:0;\" height=\"21\" bgcolor=\"#CCCCCC\">\n");
	printf("			<input type=\"text\" name=\"txt_alarm_rcu_fan\" value=\"Fan\" style=\"width:100\%; font-size:16; text-align:center; border-width:1; border-style:solid; background-color:%s\" ></td>\n", p_color_rcu_fan);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:1; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">\n", 			p_color_service[0]);
	printf("			<p align=\"center\">%s</p></td>\n", p_str_mount[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:1; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">\n", 			p_color_service[1]);
	printf("			<p align=\"center\">%s</p></td>\n", p_str_mount[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:1; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">\n", 			p_color_service[2]);
	printf("			<p align=\"center\">%s</p></td>\n", p_str_mount[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:1; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">\n", 			p_color_service[3]);
	printf("			<p align=\"center\">%s</p></td>\n", p_str_mount[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("	</tr>\n");
	printf("    <tr height=22 style='height:16.5pt' bgcolor=\"#CCCCCC\">\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>	\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">\n", 			p_color_service[0]);
	printf("			<p align=\"center\"><b><font size=\"4\">%s</font></b></p></td>\n", p_str_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">\n", 			p_color_service[1]);
	printf("			<p align=\"center\"><b><font size=\"4\">%s</font></b></p></td>\n", p_str_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>	\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">\n", 			p_color_service[2]);
	printf("			<p align=\"center\"><b><font size=\"4\">%s</font></b></p></td>\n", p_str_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>	\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>	\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">\n", 			p_color_service[3]);
	printf("			<p align=\"center\"><b><font size=\"4\">%s</font></b></p></td>\n", p_str_service[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>	\n", 	p_color_service[3]);
	printf("	</tr>\n");
	printf("    <tr height=22 style='height:16.5pt' bgcolor=\"#CCCCCC\">\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\" >&nbsp;</td>\n");
	printf("		<td style=\"border-width:0;\" height=\"21\" bgcolor=\"#CCCCCC\">\n");
	printf("			<input type=\"text\" name=\"txt_alarm_rcu_voltage\" value=\"Voltage\" style=\"width:100\%; font-size:16; text-align:center; border-width:1; border-style:solid; background-color:%s\" ></td>\n", p_color_rcu_voltage);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("	</tr>\n");
	printf("    <tr height=22 style='height:16.5pt' bgcolor=\"#CCCCCC\">\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("	</tr>\n");
	printf("    <tr height=22 style='height:16.5pt' bgcolor=\"#CCCCCC\">\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:0;\" height=\"21\" bgcolor=\"#CCCCCC\">\n");
	printf("			<input type=\"text\" name=\"txt_alarm_rcu_current\" value=\"Current\" style=\"width:100\%; font-size:16; text-align:center; border-width:1; border-style:solid; background-color:%s\" ></td>\n", p_color_rcu_current);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:0;\" height=\"21\" bgcolor=\"%s\">\n", p_color_service[0]);
	printf("			<input type=\"text\" name=\"txt_alarm_amp0_alarm\" value=\"Alarm\" style=\"width:100\%; font-size:16; text-align:center; border-width:1; border-style:solid; background-color:%s\" ></td>\n", p_color_alarm[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:0;\" height=\"21\" bgcolor=\"%s\">\n", p_color_service[1]);
	printf("			<input type=\"text\" name=\"txt_alarm_amp1_alarm\" value=\"Alarm\" style=\"width:100\%; font-size:16; text-align:center; border-width:1; border-style:solid;background-color:%s\" ></td>\n", p_color_alarm[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:0;\" height=\"21\" bgcolor=\"%s\">\n", p_color_service[2]);
	printf("			<input type=\"text\" name=\"txt_alarm_amp2_alarm\" value=\"Alarm\" style=\"width:100\%; font-size:16; text-align:center; border-width:1; border-style:solid; background-color:%s\" ></td>\n", p_color_alarm[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:0;\" height=\"21\" bgcolor=\"%s\">\n", p_color_service[3]);
	printf("			<input type=\"text\" name=\"txt_alarm_amp3_alarm\" value=\"Alarm\" style=\"width:100\%; font-size:16; text-align:center; border-width:1; border-style:solid; background-color:%s\" ></td>\n", p_color_alarm[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("	</tr>\n");
	printf("    <tr height=22 style='height:16.5pt' bgcolor=\"#CCCCCC\">\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("	</tr>\n");
	printf("    <tr height=22 style='height:16.5pt' bgcolor=\"#CCCCCC\">\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:0;\" height=\"21\" bgcolor=\"#CCCCCC\">\n");
	printf("			<input type=\"text\" name=\"txt_alarm_rcu_reset\" value=\"Reset\" style=\"width:100\%; font-size:16; text-align:center; border-width:1; border-style:solid; background-color:%s\" ></td>\n", p_color_rcu_reset);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:0;\" height=\"21\" bgcolor=\"%s\">\n", p_color_service[0]);
	printf("			<input type=\"text\" name=\"txt_alarm_amp0_shutdown\" value=\"Shutdown\" style=\"width:100\%; font-size:16; text-align:center; border-width:1; border-style:solid;background-color:%s\" ></td>\n", p_color_shutdown[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:0;\" height=\"21\" bgcolor=\"%s\">\n", p_color_service[1]);
	printf("			<input type=\"text\" name=\"txt_alarm_amp1_shutdown\" value=\"Shutdown\" style=\"width:100\%; font-size:16; text-align:center; border-width:1; border-style:solid; background-color:%s\" ></td>\n", p_color_shutdown[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:0;\" height=\"21\" bgcolor=\"%s\">\n", p_color_service[2]);
	printf("			<input type=\"text\" name=\"txt_alarm_amp2_shutdown\" value=\"Shutdown\" style=\"width:100\%; font-size:16; text-align:center; border-width:1; border-style:solid; background-color:%s\" ></td>\n", p_color_shutdown[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:hidden; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:0;\" height=\"21\" bgcolor=\"%s\">\n", p_color_service[3]);
	printf("			<input type=\"text\" name=\"txt_alarm_amp3_shutdown\" value=\"Shutdown\" style=\"width:100\%; font-size:16; text-align:center; border-width:1; border-style:solid; background-color:%s\" ></td>\n", p_color_shutdown[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:hidden; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("	</tr>\n");
	printf("    <tr height=22 style='height:16.5pt' bgcolor=\"#CCCCCC\">\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\"  bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:ridge; border-left-style:hidden;\" bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:ridge; border-left-style:hidden;\" bgcolor=\"#CCCCCC\">&nbsp;</td>\n");
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:ridge; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:ridge; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[0]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:ridge; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:ridge; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[1]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:ridge; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:ridge; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[2]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\"  bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:ridge; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("		<td style=\"border-width:2; border-top-style:hidden; border-right-style:ridge;  border-bottom-style:ridge; border-left-style:hidden;\" bgcolor=\"%s\">&nbsp;</td>\n", 	p_color_service[3]);
	printf("	</tr>\n");
	printf("</table>\n");
	printf("	<input name=\"monitor_mount\" type=\"checkbox\" checked value=\"15\" style=\"display:none\">\n");
	printf("	<input name=\"first_load\" type=\"checkbox\" checked value=\"1\" style=\"display:none\">\n");

	printf("%s", "</form>");
	printf("%s", "</body>\n");
	printf("%s", "</html>\n");

//	close(sockfd);
}


