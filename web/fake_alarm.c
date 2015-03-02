#include <stdio.h>
#include "protocol.h"
#include "debug.h"
#include "main.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

extern const char *snmp_mapping[];
extern const char *alarm_name[];
extern const char* service_name[];
extern const char* service_color[];


void fake_alarm_configuration(int amp_id)
{
	struct protocol_web_status protocol_status;
	struct protocol_web_alarm protocol_web_alarm;
	int i;//, j;//, cnt;
	char str[64], str_tmp[64];

	if(get_status(&protocol_status)<0) return;
	if(get_alarmconfig(&protocol_web_alarm)<0) return;

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

//=====================================
	html_page_head(0);  // arg==requiredfrom true(1), false(0)
	printf("<!-- \n");
	printf("function logout()\n");
	printf("{\n");
	printf("	if (document.cookie != \"\") \n");
	printf("	{\n");
	printf("        thisCookie = document.cookie.split(\"; \")\n");
	printf("        expireDate = new Date\n");
	printf("        expireDate.setDate(expireDate.getDate()-1)\n");
	printf("        for (i=0; i<thisCookie.length; i++) \n");
	printf("		{\n");
	printf("			document.cookie = thisCookie + \";expires=\" + expireDate.toGMTString()\n");
	printf("		}\n");
	printf("	}\n");
	printf("	location.replace(\"html.cgi?function=logout\");\n");
	printf("}\n");

	create_ajax_code();
	printf("\n");

	printf("function reload() {\n");
	printf("	sendRequest(status_load,'','GET','/web_status%d',true,true);\n", amp_id);	// amp_id=0~3
	printf("	time_id = setTimeout(\"reload()\", 60000);\n");
	printf("}\n");
	printf("\n");

	printf("function status_load(oj)\n");
	printf("{\n");
	printf("	var detect = decodeURIComponent(oj.responseText);\n");
	printf("	var s00=\"led_alarm_total\";\n");
	printf("	var i00=detect.indexOf(s00)+s00.length+1;\n");
	printf("	var val_alarm_led = detect.substring(i00, i00+1);\n");//i00 to i00; 1 character
	printf("	var s01=\"led_shutdown_total\";\n");
	printf("	var i01=detect.indexOf(s01)+s01.length+1;\n");
	printf("	var val_shutdown_led = detect.substring(i01, i01+1);\n");//0
 	printf("	document.form0.alarm_status.style.background = (val_alarm_led == 1)? \"#FF0000\": \"#00FF00\";\n");
	printf("	document.form0.shutdown_status.style.background = (val_shutdown_led == 1)? \"#FF0000\": \"#00FF00\";\n");
	printf("\n");
	printf("	clearTimeout(time_id);\n");
	printf("	time_id = setTimeout(\"reload()\", 3000);\n");
	printf("}\n");

	printf("\n");
	printf("//--> \n");
	printf("</script> \n");
	printf("\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("</head>\n");
	printf("\n");

	printf("<body bgcolor=\"#FFFFFF\" text=\"#000000\" onLoad = \"reload()\">\n");
//--------------------------------------------------------------------------------
	html_menu_head(&protocol_status, "Fake alarm configuration");

//--------------------------------------------------------------------------------
	if(protocol_status.status[amp_id].svc_info>7) protocol_status.status[amp_id].svc_info=0;

	printf("<form name=\"form1\" method=\"POST\" action=\"html.cgi\">\n");
	printf("	<input name=\"check_amp_id_fake\" type=\"checkbox\" checked value=\"%d\" style=\"display:none\">\n", amp_id);
	printf("<table cellspacing=\"0\" border=\"0\" width=\"740\">\n");
	printf("	<tr height=22 style='height:16.5pt'>\n");
	printf("		<td height=\"50\" colspan=\"2\" width=\"529\" rowspan=\"2\" bgcolor=\"%s\" style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\">\n", service_color[protocol_status.status[amp_id].svc_info]);
	if(protocol_status.status[amp_id].svc_info==0)
		sprintf(str, "unknown"); 
	else
		sprintf(str, "%s (model %d)", service_name[protocol_status.status[amp_id].svc_info], protocol_status.status[amp_id].amp_capacity);
	printf("		<p align=\"left\"><font size=\"5\"><b>&nbsp;Fake alarm configuration - %s</b></font></p></td>\n", str);

	printf("		<td width=\"136\" bgcolor=\"%s\" colspan=\"3\" style=\"border-width:2; border-top-style:ridge; border-right-style:ridge; border-bottom-style:hidden; border-left-style:hidden;\">\n", service_color[protocol_status.status[amp_id].svc_info]);
	printf("		<p align=\"right\"><input type=\"button\" style=\"width:136; font-size: 10pt; name=\"apply\" value=\"Apply\" onclick=\"this.form.submit();\"></p></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=22>\n");
	printf("		<td bgcolor=\"%s\" style=\"border-width:2; border-top-style:hidden; border-right-style:ridge; border-bottom-style:ridge; border-left-style:hidden;\">\n", service_color[protocol_status.status[amp_id].svc_info]);
	printf("		<p align=\"right\">&nbsp;</p></td>\n");
	printf("	</tr>\n");
	printf("	<tr>\n");
	memcpy(str_tmp, protocol_status.status[amp_id].serial_no, 16);
	str_tmp[16]=0;
	sprintf(str, "[AMP-%d] S/N:%s", amp_id+1, str_tmp);
	printf("		<td height=30 bgcolor=\"%s\" style=\"border-width:2; border-top-style:hidden; border-right-style:ridge; border-bottom-style:ridge; border-left-style:ridge;\" colspan=\"15\">\n", service_color[protocol_status.status[amp_id].svc_info]);
	printf("			&nbsp;%s</td>\n", str);
	printf("	</tr>\n");
	printf("</table>\n");

	printf("<table cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"740\">\n");
	printf("<tr><td>\n");
	printf("<table cellspacing=\"0\" cellpadding=\"0\" border=\"0\" width=\"736\">\n");
	printf("    <tr height=22 style='height:16.5pt' bgcolor=\"#CCCCCC\" >\n");
	printf("        <td width=\"15%\" ><input type=\"button\" name=\"alarm_amp1\" value=\"AMP1\" style=\"width:120; font-size: 10pt;\" onclick=\"form1.check_amp_id_fake.value=0;self.location='html.cgi?function=fake_alarm_config'\"></td>\n");
	printf("        <td width=\"15%\" ><input type=\"button\" name=\"alarm_amp2\" value=\"AMP2\" style=\"width:120; font-size: 10pt;\" onclick=\"form1.check_amp_id_fake.value=1;self.location='html.cgi?function=fake_alarm_config_1'\"></td>\n");
	printf("        <td width=\"15%\" ><input type=\"button\" name=\"alarm_amp3\" value=\"AMP3\" style=\"width:120; font-size: 10pt;\" onclick=\"form1.check_amp_id_fake.value=2;self.location='html.cgi?function=fake_alarm_config_2'\"></td>\n");
	printf("        <td width=\"15%\" ><input type=\"button\" name=\"alarm_amp4\" value=\"AMP4\" style=\"width:120; font-size: 10pt;\" onclick=\"form1.check_amp_id_fake.value=3;self.location='html.cgi?function=fake_alarm_config_3'\"></td>\n");
	printf("        <td width=\"40%\" ><p align=\"right\">&nbsp;Fake alarm mode&nbsp;\n");
	printf("            <select name=\"combo_fake_mode\" style=\"WIDTH:50pt;\" >\n");
	printf("                <option value=\"0\" %s>OFF</option>\n", (protocol_status.alarm.slot[amp_id].fake_mode)? "":"selected");
	printf("                <option value=\"1\" %s>ON</option>\n", (protocol_status.alarm.slot[amp_id].fake_mode)? "selected":"");
	printf("            </select></td>\n");
	printf("    </tr>\n");
	printf("</table>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");

	printf("<table cellspacing=\"0\" border=\"1\" width=\"740\">\n");
	printf("	<tr height=22 style='height:16.5pt' bgcolor=\"#CCCCCC\" >\n");
	printf("		<td width=\"30\" ><p align=\"center\"><b>no</b></p></td>\n");
	printf("		<td width=\"230\" ><p align=\"center\"><b>name</b></p></td>\n");
	printf("		<td width=\"70\" ><p align=\"center\"><b>status</b></p></td>\n");
	printf("		<td width=\"60\" ><p align=\"center\"><b>active</b></p></td>\n");
	printf("	</tr>\n");
//for(i=0;i<22;i++)
for(i=0;i<ALARM_INDEX_MAX;i++)
{
	printf("	<tr height=22 >\n");
	GetAlarmColorString(str, protocol_status.alarm.slot[amp_id].alarm_item[i].status);
	printf("		<td >&nbsp;%d</td>\n", i+1);
	printf("		<td >&nbsp;%s</td>\n", alarm_name[i]);
	printf("		<td ><div><input type=\"text\" name=\"txt_status_%d\" style=\"width:100\%; height:100\%; background-color: %s\" readonly></div></td>\n", i, str);
	printf("		<td >\n");
	printf("			<select name=\"combo_active_%d\"  style=\"width:100\%;\">\n", i);
	printf("				<option value=\"0\" %s>OFF</option>\n", (protocol_status.alarm.slot[amp_id].alarm_item[i].fake_status)? "": "selected");
	printf("				<option value=\"1\" %s>ON</option>\n", (protocol_status.alarm.slot[amp_id].alarm_item[i].fake_status)? "selected": "");
	printf("			</select></td>\n");
//	printf("		<td ><font size=\"2\">&nbsp;%s</font></td>\n", protocol_status.alarm.slot[amp_id].alarm_item[i].last_triggered);
//	printf("		<td >\n");
//	printf("			<select name=\"combo_category_%d\" >\n", i);
//	for(j=0;j<22;j++)
//		printf("				<option value=\"%d\" %s>%s</option>\n", j, (j==protocol_status.alarm.slot[amp_id].alarm_item[i].category)? "selected": "", snmp_mapping[j]);
//	printf("			</select></td>\n");
	printf("	</tr>\n");
}
	printf("</table>\n");
	printf("</form>\n");

	printf("</body>\n");
	printf("</html>\n");

//	close(sockfd);	
}


