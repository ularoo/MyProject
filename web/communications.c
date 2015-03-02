#include <stdio.h>
#include "protocol.h"
#include "debug.h"
#include "main.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>


void communications()
{
	int amp_id=0;
	struct protocol_communication protocol_communication;
	struct protocol_web_status protocol_status;
	int i;
/*
	char *canada_timezone[] = 
	{
		"Atlantic", 
		"Central", 
		"Eastern",
		"East-Saskatchewan",
		"Mountain",
		"Newfoundland",
		"Pacific",
		"Saskatchewan",
		"Yukon"
	};
*/
	char *usa_timezone[] = 
	{
		"Alaska",
		"Aleutian",
		"Arizona",
		"Central",
		"Eastern",
		"East-Indiana",
		"Hawaii",
		"Indiana-Starke",
		"Michigan",
		"Mountain",
		"Pacific",
		"Samoa"
	};

	char *month[] = 
	{
		"January", 
		"February", 
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December"
	};

	if(get_status(&protocol_status)<0) return;
	if(get_communication(&protocol_communication)<0) return;

//==============================================
	html_page_head(0);	// arg==requiredfrom true(1), false(0)
	printf("<!--\n");
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
	printf("\n");

	create_ajax_code();
	printf("\n");

	printf("function reload() {\n");
	printf("   sendRequest(status_load,'','GET','/web_status%d',true,true);\n", amp_id);
	printf("   time_id = setTimeout(\"reload()\", 60000);\n");
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
	printf("        if(form1.combo_wan_modem_onoff.value==\"1\")\n");
	printf("        {\n");
	printf("            form1.combo_wan_dhcp_client_onoff.value=\"1\";\n");
	printf("            CheckCheckObj(form1.check_combo_wan_dhcp_client_onoff);\n");
	//printf("            form1.combo_wan_dhcp_client_onoff.disabled=true;\n");
	printf("        }\n");
	printf("\n");
	printf("	clearTimeout(time_id);\n");
	printf("	time_id = setTimeout(\"reload()\", 3000);\n");
	printf("}\n");

	printf("function CheckCheckObj(CheckObj)\n");
	printf("{\n");
	printf("	CheckObj.checked=\"true\";\n");
	printf("	CheckObj.value=\"1\";\n");
	printf("\n");
	printf("	if(CheckObj==form1.check_combo_lan_dhcp_client_onoff)\n");
	printf("	{\n");
	printf("        if(form1.combo_lan_dhcp_client_onoff.value==\"1\")\n");
	printf("        {\n");
	printf("            form1.combo_lan_dhcp_server_onoff.value=\"0\";\n");
	printf("            form1.txt_lan_ip.style.backgroundColor=\"silver\";\n");
	printf("            form1.txt_lan_ip.readOnly=true;\n");
	printf("            form1.txt_lan_netmask.style.backgroundColor=\"silver\";\n");
	printf("            form1.txt_lan_netmask.readOnly=true;\n");
	printf("            form1.txt_lan_gateway.style.backgroundColor=\"silver\";\n");
	printf("            form1.txt_lan_gateway.readOnly=true;\n");
	printf("        }\n");
	printf("        else\n");
	printf("        {\n");
	printf("            form1.txt_lan_ip.style.backgroundColor=\"white\";\n");
	printf("            form1.txt_lan_ip.readOnly=false;\n");
	printf("            form1.txt_lan_netmask.style.backgroundColor =\"white\";\n");
	printf("            form1.txt_lan_netmask.readOnly=false;\n");
	printf("            form1.txt_lan_gateway.style.backgroundColor=\"white\";\n");
	printf("            form1.txt_lan_gateway.readOnly=false;\n");
	printf("        }\n");
	printf("    }\n");
	printf("	if(CheckObj==form1.check_combo_lan_dhcp_server_onoff)\n");
	printf("	{\n");
	printf("        if(form1.check_combo_lan_dhcp_server_onoff.value==\"1\")\n");
	printf("        {\n");
	printf("            form1.combo_lan_dhcp_client_onoff.value=\"0\";\n");
	printf("            CheckCheckObj(form1.check_combo_lan_dhcp_client_onoff);\n");
	printf("        }\n");
	printf("    }\n");
	printf("	if(CheckObj==form1.check_combo_wan_dhcp_client_onoff)\n");
	printf("	{\n");
	printf("        if(form1.combo_wan_dhcp_client_onoff.value==\"1\")\n");
	printf("        {\n");
	printf("            form1.txt_wan_ip.style.backgroundColor=\"silver\";\n");
	printf("            form1.txt_wan_ip.readOnly=true;\n");
	printf("            form1.txt_wan_netmask.style.backgroundColor=\"silver\";\n");
	printf("            form1.txt_wan_netmask.readOnly=true;\n");
	printf("            form1.txt_wan_gateway.style.backgroundColor=\"silver\";\n");
	printf("            form1.txt_wan_gateway.readOnly=true;\n");
	printf("        }\n");
	printf("        else\n");
	printf("        {\n");
	printf("            form1.combo_wan_modem_onoff.value=\"0\";\n");
	printf("            form1.txt_wan_ip.style.backgroundColor=\"white\";\n");
	printf("            form1.txt_wan_ip.readOnly=false;\n");
	printf("            form1.txt_wan_netmask.style.backgroundColor=\"white\";\n");
	printf("            form1.txt_wan_netmask.readOnly=false;\n");
	printf("            form1.txt_wan_gateway.style.backgroundColor=\"white\";\n");
	printf("            form1.txt_wan_gateway.readOnly=false;\n");
	printf("            \n");
	printf("        }\n");
	printf("    }\n");
	printf("	if(CheckObj==form1.check_combo_wan_modem_onoff)\n");
	printf("	{\n");
	printf("        if(form1.combo_wan_modem_onoff.value==\"1\")\n");
	printf("        {\n");
	printf("            form1.combo_wan_dhcp_client_onoff.value=\"1\";\n");
	printf("            CheckCheckObj(form1.check_combo_wan_dhcp_client_onoff);\n");
//	printf("            form1.combo_wan_dhcp_client_onoff.disabled=true;\n");
 	printf("        }\n");
	printf("        else\n");
	printf("        {\n");
	printf("            form1.combo_wan_dhcp_client_onoff.disabled=false;\n");
	printf("        }\n");
	printf("    }\n");
	printf("	if(CheckObj==form1.check_combo_auth)\n");
	printf("	{\n");
	printf("        if(form1.combo_auth.value==\"%d\")\n", SNMP_NONE);
	printf("        {\n");
	printf("            if(form1.combo_priv.value!==\"%d\")\n", SNMP_NONE);
	printf("            {\n");
	printf("                form1.combo_priv.value=\"%d\";\n", SNMP_NONE);
	printf("                CheckCheckObj(form1.check_combo_priv.value);\n");
 	printf("            }\n");
 	printf("        }\n");
	printf("    }\n");
	printf("	if(CheckObj==form1.check_combo_trap_auth)\n");
	printf("	{\n");
	printf("        if(form1.combo_trap_auth.value==\"%d\")\n", SNMP_NONE);
	printf("        {\n");
	printf("            if(form1.combo_trap_priv.value!==\"%d\")\n", SNMP_NONE);
	printf("            {\n");
	printf("                form1.combo_trap_priv.value=\"%d\";\n", SNMP_NONE);
	printf("                CheckCheckObj(form1.check_combo_trap_priv.value);\n");
 	printf("            }\n");
 	printf("        }\n");
	printf("    }\n");
	printf("	if(CheckObj==form1.check_combo_priv)\n");
	printf("	{\n");
	printf("        if(form1.combo_priv.value!==\"%d\")\n", SNMP_NONE);
	printf("        {\n");
	printf("            if(form1.combo_auth.value==\"%d\")\n", SNMP_NONE);
	printf("            {\n");
	printf("                form1.combo_auth.value=\"%d\";\n", SNMP_SHA);
	printf("                CheckCheckObj(form1.check_combo_auth.value);\n");
 	printf("            }\n");
 	printf("        }\n");
	printf("    }\n");
	printf("	if(CheckObj==form1.check_combo_trap_priv)\n");
	printf("	{\n");
	printf("        if(form1.combo_trap_priv.value!==\"%d\")\n", SNMP_NONE);
	printf("        {\n");
	printf("            if(form1.combo_trap_auth.value==\"%d\")\n", SNMP_NONE);
	printf("            {\n");
	printf("                form1.combo_trap_auth.value=\"%d\";\n", SNMP_SHA);
	printf("                CheckCheckObj(form1.check_combo_trap_auth.value);\n");
 	printf("            }\n");
 	printf("        }\n");
	printf("    }\n");
	printf("}\n");

/*
	printf("function select_ntp_period_change(value)\n");
	printf("{\n");
	printf("  if(value == 0)\n");
	printf("  {\n");
	printf("      document.form_communications.txt_month.style.background=\"white\";\n");
	printf("      document.form_communications.txt_day.style.background=\"white\";\n");
	printf("      document.form_communications.txt_hour.style.background=\"white\";\n");
	printf("      document.form_communications.txt_min.style.background=\"white\";\n");
	printf("      document.form_communications.txt_year.style.background=\"white\";\n");
	printf("      document.form_communications.txt_month.readOnly=false;\n");
	printf("      document.form_communications.txt_day.readOnly=false;\n");
	printf("      document.form_communications.txt_hour.readOnly=false;\n");
	printf("      document.form_communications.txt_min.readOnly=false;\n");
	printf("      document.form_communications.txt_year.readOnly=false;\n");
	printf("  }\n");
	printf("  else\n");
	printf("  {\n");
	printf("      document.form_communications.txt_month.style.background=\"silver\";\n");
	printf("      document.form_communications.txt_day.style.background=\"silver\";\n");
	printf("      document.form_communications.txt_hour.style.background=\"silver\";\n");
	printf("      document.form_communications.txt_min.style.background=\"silver\";\n");
	printf("      document.form_communications.txt_year.style.background=\"silver\";\n");
	printf("      document.form_communications.txt_month.readOnly=true;\n");
	printf("      document.form_communications.txt_day.readOnly=true;\n");
	printf("      document.form_communications.txt_hour.readOnly=true;\n");
	printf("      document.form_communications.txt_min.readOnly=true;\n");
	printf("      document.form_communications.txt_year.readOnly=true;\n");
	printf("  }\n");
	printf("}\n");
*/
	printf("//-->\n");
	printf("</script>\n");
	printf("\n");

	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("\n");
	printf("</head>\n");
	printf("\n");
	printf("<body bgcolor=\"#FFFFFF\" text=\"#000000\" onLoad = \"reload()\">\n");

//--------------------------------------------------------------------------------
	html_menu_head(&protocol_status, "Communication configuration");

//--------------------------------------------------------------------------------
	printf("<form name=\"form1\" method=\"POST\" action=\"html.cgi\">\n");
	printf("	<input name=\"check_combo_lan_dhcp_client_onoff\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_lan_ip\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_combo_lan_dhcp_server_onoff\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_lan_netmask\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_lan_gateway\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_combo_wan_modem_onoff\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_wan_ip\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_combo_wan_dhcp_client_onoff\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_wan_netmask\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_wan_gateway\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_snmp_read_community\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_snmp_manager_ip\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_snmp_write_community\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_snmp_general_port\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_snmp_trap_community\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_snmp_trap_port\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_set_year\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_combo_set_month\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_set_day\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_set_hour\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_set_minute\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_heartbeat_interval\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
#if 0	/* sprint */
	printf("	<input name=\"check_txt_system_id\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_coordinate\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_location\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_contact_info\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
#else
	printf("	<input name=\"check_txt_latitude\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_longitude\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
#endif
	
	printf("	<input name=\"check_txt_ntp_ip\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_combo_ntp_period\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_combo_time_zone\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
/* C^2 Rev3 */
	printf("	<input name=\"check_combo_snmp_version\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_engine_id\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_read_user\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_write_user\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_combo_auth\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_combo_priv\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_authpassphrase\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_privpassphrase\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_trap_engine_id\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_trap_user\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_combo_trap_auth\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_combo_trap_priv\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_trap_authpassphrase\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_txt_trap_privpassphrase\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("\n");
	
	printf("<table cellspacing=\"0\" border=\"0\" width=\"740\">\n");
	printf("	<tr height=22 style='height:16.5pt'>\n");
	printf("		<td height=\"50\" colspan=\"2\" width=\"529\" rowspan=\"2\" bgcolor=\"#CCCCCC\" style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\">\n");
	printf("		<p align=\"left\"><font size=\"5\"><b>&nbsp;Communication configuration</b></font></p></td>\n");
	printf("		<td width=\"136\" bgcolor=\"#CCCCCC\" colspan=\"3\" style=\"border-width:2; border-top-style:ridge; border-right-style:ridge; border-bottom-style:hidden; border-left-style:hidden;\">\n");
	printf("		<p align=\"right\"><input type=\"button\" style=\"width:136; font-size: 10pt; name=\"apply\" value=\"Apply\" onclick=\"this.form.submit();\"></p></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=22>\n");
	printf("		<td bgcolor=\"#CCCCCC\" style=\"border-width:2; border-top-style:hidden; border-right-style:ridge; border-bottom-style:ridge; border-left-style:hidden;\">&nbsp</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");

	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>LAN</b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Obtain IP address</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_lan_dhcp_client_onoff\" style=\"WIDTH: 120pt;\" onChange=CheckCheckObj(form1.check_combo_lan_dhcp_client_onoff)>\n");
	printf("				<option value=\"0\" %s>STATIC</option>\n", (protocol_communication.lan_dhcp_client_onoff)? "": "selected");
	printf("				<option value=\"1\" %s>DHCP</option>\n", (protocol_communication.lan_dhcp_client_onoff)? "selected": "");
	printf("			</select></td>\n");
	printf("		<td width=26% >&nbsp;IP address</td>\n");
	printf("		<td width=24% >\n");
if(protocol_communication.lan_dhcp_client_onoff)
	printf("			<input type=\"text\" name=\"txt_lan_ip\" style=\"WIDTH:120pt; background:silver\" maxlength=\"32\" readonly value=\"%s\" onChange=CheckCheckObj(form1.check_txt_lan_ip)></td>\n", protocol_communication.lan_ip);
else
	printf("			<input type=\"text\" name=\"txt_lan_ip\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_lan_ip)></td>\n", protocol_communication.lan_ip);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;DHCP server</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_lan_dhcp_server_onoff\" style=\"WIDTH: 120pt;\" onChange=CheckCheckObj(form1.check_combo_lan_dhcp_server_onoff)>\n");
	printf("				<option value=\"1\" %s>ON</option>\n", (protocol_communication.lan_dhcp_server_onoff)? "selected": "");
	printf("				<option value=\"0\" %s>OFF</option>\n", (protocol_communication.lan_dhcp_server_onoff)? "": "selected");
	printf("			</select></td>\n");
	printf("		<td width=26% >&nbsp;Netmask</td>\n");
	printf("		<td width=24% >\n");
if(protocol_communication.lan_dhcp_client_onoff)
	printf("			<input type=\"text\" name=\"txt_lan_netmask\" style=\"WIDTH:120pt; background:silver\" maxlength=\"32\" readonly value=\"%s\" onChange=CheckCheckObj(form1.check_txt_lan_netmask)></td>\n", protocol_communication.lan_netmask);
else
	printf("			<input type=\"text\" name=\"txt_lan_netmask\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_lan_netmask)></td>\n", protocol_communication.lan_netmask);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;</td>\n");
	printf("		<td width=24% >&nbsp;</td>\n");
	printf("		<td width=26% >&nbsp;Gateway</td>\n");
	printf("		<td width=24% >\n");
if(protocol_communication.lan_dhcp_client_onoff)
	printf("			<input type=\"text\" name=\"txt_lan_gateway\" style=\"WIDTH:120pt; background:silver\" maxlength=\"32\" readonly value=\"%s\" onChange=CheckCheckObj(form1.check_txt_lan_gateway)></td>\n", protocol_communication.lan_gateway);
else
	printf("			<input type=\"text\" name=\"txt_lan_gateway\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_lan_gateway)></td>\n", protocol_communication.lan_gateway);
	printf("	</tr>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>WAN</b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;WAN Interface</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_wan_modem_onoff\" style=\"WIDTH: 120pt;\" onChange=CheckCheckObj(form1.check_combo_wan_modem_onoff)>\n");
	printf("				<option value=\"0\" %s>ETHERNET</option>\n", (protocol_communication.wan_modem_onoff)? "": "selected");
	printf("				<option value=\"1\" %s>MODEM(PPP)</option>\n", (protocol_communication.wan_modem_onoff)? "selected": "");
	printf("			</select></td>\n");
	printf("		<td width=26% >&nbsp;IP Address</td>\n");
	printf("		<td width=24% >\n");
if(protocol_communication.wan_dhcp_client_onoff)
	printf("			<input type=\"text\" name=\"txt_wan_ip\" style=\"WIDTH:120pt; background:silver\" maxlength=\"32\" readonly value=\"%s\" onChange=CheckCheckObj(form1.check_txt_wan_ip)></td>\n", protocol_communication.wan_ip);
else
	printf("			<input type=\"text\" name=\"txt_wan_ip\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_wan_ip)></td>\n", protocol_communication.wan_ip);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Obtain IP Address</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_wan_dhcp_client_onoff\" style=\"WIDTH: 120pt;\" onChange=CheckCheckObj(form1.check_combo_wan_dhcp_client_onoff)>\n");
	printf("				<option value=\"0\" %s>STATIC</option>\n", (protocol_communication.wan_dhcp_client_onoff)? "": "selected");
	printf("				<option value=\"1\" %s>DHCP</option>\n", (protocol_communication.wan_dhcp_client_onoff)? "selected": "");
	printf("			</select></td>\n");
	printf("		<td width=26% >&nbsp;Netmask</td>\n");
	printf("		<td width=24% >\n");
if(protocol_communication.wan_dhcp_client_onoff)
	printf("			<input type=\"text\" name=\"txt_wan_netmask\" style=\"WIDTH:120pt; background:silver\" maxlength=\"32\" readonly value=\"%s\" onChange=CheckCheckObj(form1.check_txt_wan_netmask)></td>\n", protocol_communication.wan_netmask);
else
	printf("			<input type=\"text\" name=\"txt_wan_netmask\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_wan_netmask)></td>\n", protocol_communication.wan_netmask);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;</td>\n");
	printf("		<td width=24% >&nbsp;</td>\n");
	printf("		<td width=26% >&nbsp;Gateway</td>\n");
	printf("		<td width=24% >\n");
if(protocol_communication.wan_dhcp_client_onoff)
	printf("			<input type=\"text\" name=\"txt_wan_gateway\" style=\"WIDTH:120pt; background:silver\" maxlength=\"32\" readonly value=\"%s\" onChange=CheckCheckObj(form1.check_txt_wan_gateway)></td>\n", protocol_communication.wan_gateway);
else
	printf("			<input type=\"text\" name=\"txt_wan_gateway\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_wan_gateway)></td>\n", protocol_communication.wan_gateway);
	printf("	</tr>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");
/* SNMP Common */
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>SNMP Common</b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Version</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_snmp_version\" style=\"WIDTH: 120pt;\" onChange=CheckCheckObj(form1.check_combo_snmp_version)>\n");
	printf("				<option value=\"%d\"%s>2c</option>\n", SNMP_V2C, SNMP_V2C == protocol_communication.snmp_version ? " selected" : "");
	printf("				<option value=\"%d\"%s>3</option>\n", SNMP_V3, SNMP_V3 == protocol_communication.snmp_version ? " selected" : "");
	printf("			</select></td>\n");
	printf("		<td width=26% >&nbsp;Manager IP</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_snmp_manager_ip\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_snmp_manager_ip)></td>\n", protocol_communication.snmp_manager_ip);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;General Port</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_snmp_general_port\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%d\" onChange=CheckCheckObj(form1.check_txt_snmp_general_port)></td>\n", protocol_communication.snmp_general_port);
	printf("		<td width=26% >&nbsp;Trap Port</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_snmp_trap_port\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%d\" onChange=CheckCheckObj(form1.check_txt_snmp_trap_port)></td>\n", protocol_communication.snmp_trap_port);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Heartbeat interval</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_heartbeat_interval\" style=\"WIDTH:80pt; \" maxlength=\"32\" value=\"%d\" onChange=CheckCheckObj(form1.check_txt_heartbeat_interval)> Minutes</td>\n", protocol_communication.heartbeat_interval);
#if 0	/* sprint */
	printf("		<td width=26% >&nbsp;System ID</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_system_id\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_system_id)></td>\n", protocol_communication.system_id);
#else
	printf("		<td width=26% >&nbsp;</td>\n");
	printf("		<td width=24% >&nbsp;</td>\n");
#endif
	printf("	</tr>\n");
#if 0	/* sprint */
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Coordinate</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_coordinate\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_coordinate)></td>\n", protocol_communication.coordinate);
	printf("		<td width=26% >&nbsp;Site Name(Location)</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_location\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_location)></td>\n", protocol_communication.location);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Contact Information</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_contact_info\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_contact_info)></td>\n", protocol_communication.contact_info);
	printf("		<td width=26% >&nbsp;</td>\n");
	printf("		<td width=24% >&nbsp;</td>\n");
	printf("	</tr>\n");
#else
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Latitude</br>&nbsp;&nbsp;ex) N038.918890</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_latitude\" style=\"WIDTH:120pt; \" maxlength=\"12\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_latitude)></td>\n", protocol_communication.latitude);
	printf("		<td width=26% >&nbsp;Longitude</br>&nbsp;&nbsp;ex) W094.657840</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_longitude\" style=\"WIDTH:120pt; \" maxlength=\"12\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_longitude)></td>\n", protocol_communication.longitude);
	printf("	</tr>\n");
#endif
	printf("</table>\n");
/* SNMPv2c */
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>SNMPv2c</b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Read Community</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_snmp_read_community\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_snmp_read_community)></td>\n", protocol_communication.snmp_read_community);
	printf("		<td width=26% >&nbsp;Write Community</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_snmp_write_community\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_snmp_write_community)></td>\n", protocol_communication.snmp_write_community);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Trap Community</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_snmp_trap_community\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_snmp_trap_community)></td>\n", protocol_communication.snmp_trap_community);
	printf("		<td width=26% >&nbsp;</td>\n");
	printf("		<td width=24% >&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");
/* SNMPv3 */
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>SNMPv3</b></font></td>\n");
	printf("	</tr>\n");
	//printf("	<tr height=\"25\">\n");
	//printf("		<td width=26% >&nbsp;Engine ID</td>\n");
	//printf("		<td width=24% >\n");
	//printf("			<input type=\"text\" name=\"txt_engine_id\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_engine_id)></td>\n", protocol_communication.engine_id);
	//printf("		<td width=26% >&nbsp;</td>\n");
	//printf("		<td width=24% >&nbsp;</td>\n");
	//printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Read User</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_read_user\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_read_user)></td>\n", protocol_communication.read_user);
	printf("		<td width=26% >&nbsp;Write User</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_write_user\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_write_user)></td>\n", protocol_communication.write_user);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Authentication</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_auth\" style=\"WIDTH: 120pt;\" onChange=CheckCheckObj(form1.check_combo_auth)>\n");
	printf("				<option value=\"%d\"%s>none</option>\n", SNMP_NONE, SNMP_NONE == protocol_communication.auth ? " selected" : "");
	printf("				<option value=\"%d\"%s>MD5</option>\n", SNMP_MD5, SNMP_MD5 == protocol_communication.auth ? " selected" : "");
	printf("				<option value=\"%d\"%s>SHA</option>\n", SNMP_SHA, SNMP_SHA == protocol_communication.auth ? " selected" : "");
	printf("			</select></td>\n");
	printf("		<td width=26% >&nbsp;Privacy(Encryption)</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_priv\" style=\"WIDTH: 120pt;\" onChange=CheckCheckObj(form1.check_combo_priv)>\n");
	printf("				<option value=\"%d\" %s>none</option>\n", SNMP_NONE, SNMP_NONE == protocol_communication.priv ? " selected" : "");
	printf("				<option value=\"%d\" %s>DES</option>\n", SNMP_DES, SNMP_DES == protocol_communication.priv ? " selected" : "");
	printf("				<option value=\"%d\" %s>AES</option>\n", SNMP_AES, SNMP_AES == protocol_communication.priv ? " selected" : "");
	printf("			</select></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Authentication Passphrase</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_authpassphrase\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_authpassphrase)></td>\n", protocol_communication.authpassphrase);
	printf("		<td width=26% >&nbsp;Privacy Passphrase</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_privpassphrase\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_privpassphrase)></td>\n", protocol_communication.privpassphrase);
	printf("	</tr>\n");
	//printf("	<tr height=\"25\">\n");
	//printf("		<td width=26% >&nbsp;Trap Engine ID</td>\n");
	//printf("		<td width=24% >\n");
	//printf("			<input type=\"text\" name=\"txt_trap_engine_id\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_trap_engine_id)></td>\n", protocol_communication.trap_engine_id);
	//printf("		<td width=26% >&nbsp;Trap User</td>\n");
	//printf("		<td width=24% >\n");
	//printf("			<input type=\"text\" name=\"txt_trap_user\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_trap_user)></td>\n", protocol_communication.trap_user);
	//printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Trap User</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_trap_user\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_trap_user)></td>\n", protocol_communication.trap_user);
	printf("		<td width=26% >&nbsp;</td>\n");
	printf("		<td width=24% >&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Authentication</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_trap_auth\" style=\"WIDTH: 120pt;\" onChange=CheckCheckObj(form1.check_combo_trap_auth)>\n");
	printf("				<option value=\"%d\"%s>none</option>\n", SNMP_NONE, SNMP_NONE == protocol_communication.trap_auth ? " selected" : "");
	printf("				<option value=\"%d\"%s>MD5</option>\n", SNMP_MD5, SNMP_MD5 == protocol_communication.trap_auth ? " selected" : "");
	printf("				<option value=\"%d\"%s>SHA</option>\n", SNMP_SHA, SNMP_SHA == protocol_communication.trap_auth ? " selected" : "");
	printf("			</select></td>\n");
	printf("		<td width=26% >&nbsp;Privacy(Encryption)</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_trap_priv\" style=\"WIDTH: 120pt;\" onChange=CheckCheckObj(form1.check_combo_trap_priv)>\n");
	printf("				<option value=\"%d\"%s>none</option>\n", SNMP_NONE, SNMP_NONE == protocol_communication.trap_priv ? " selected" : "");
	printf("				<option value=\"%d\"%s>DES</option>\n", SNMP_DES, SNMP_DES == protocol_communication.trap_priv ? " selected" : "");
	printf("				<option value=\"%d\"%s>AES</option>\n", SNMP_AES, SNMP_AES == protocol_communication.trap_priv ? " selected" : "");
	printf("			</select></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Authentication Passphrase</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_trap_authpassphrase\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_trap_authpassphrase)></td>\n", protocol_communication.trap_authpassphrase);
	printf("		<td width=26% >&nbsp;Privacy Passphrase</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_trap_privpassphrase\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_trap_privpassphrase)></td>\n", protocol_communication.trap_privpassphrase);
	printf("	</tr>\n");
	printf("</table>\n");
/* Date And Time */
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");

	if(protocol_communication.current_month>12) protocol_communication.current_month=12;
	if(protocol_communication.current_month<1) protocol_communication.current_month=1;

	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>Date And Time</b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Current Date</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_current_year\" style=\"WIDTH:120pt; background:silver\" maxlength=\"32\" readonly value=\"%d %s, %d\" ></td>\n", protocol_communication.current_year, month[protocol_communication.current_month-1], protocol_communication.current_day);
	printf("		<td width=26% >&nbsp;Set Date(Year)</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_set_year\" style=\"WIDTH:80pt; \" maxlength=\"32\" value=\"%d\" onChange=CheckCheckObj(form1.check_txt_set_year)></td>\n", protocol_communication.set_year);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Current Time(hour:minute)</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_current_time\" style=\"WIDTH:120pt; background:silver\" maxlength=\"32\" readonly value=\"%02d:%02d\" )></td>\n", protocol_communication.current_hour, protocol_communication.current_minute);
	printf("		<td width=26% >&nbsp;Set Date(month,day)</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_set_month\" style=\"WIDTH:80pt;\" onChange=CheckCheckObj(form1.check_combo_set_month)>\n");
for(i=0;i<12;i++){
	printf("				<option value=\"%d\" %s>%s</option>\n", i+1, (protocol_communication.current_month==i+1)?"selected": "", month[i]);
}
	printf("			</select>&nbsp;,&nbsp;\n");
	printf("			<input type=\"text\" name=\"txt_set_day\" style=\"WIDTH:30pt; \" maxlength=\"32\" value=\"%d\" onChange=CheckCheckObj(form1.check_txt_set_day)></td>\n", protocol_communication.current_day);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Time Zone</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_time_zone\" style=\"WIDTH: 120pt;\" onChange=CheckCheckObj(form1.check_combo_time_zone)>\n");
for(i=0;i<12;i++){
	printf("				<option value=\"%d\" %s>%s</option>\n", i, (i==protocol_communication.time_zone)?"selected":"", usa_timezone[i]);
}
	printf("			</select></td>\n");
	printf("		<td width=26% >&nbsp;Set Time(hour:minute)</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_set_hour\" style=\"WIDTH:35pt; \" maxlength=\"32\" value=\"%02d\" onChange=CheckCheckObj(form1.check_txt_set_hour)>&nbsp;:\n", protocol_communication.set_hour);
	printf("			<input type=\"text\" name=\"txt_set_minute\" style=\"WIDTH:35pt; \" maxlength=\"32\" value=\"%02d\" onChange=CheckCheckObj(form1.check_txt_set_minute)></td>\n", protocol_communication.set_minute);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;NTP Server</td>\n");
	printf("		<td width=24% >\n");
	printf("			<input type=\"text\" name=\"txt_ntp_ip\" style=\"WIDTH:120pt; \" maxlength=\"32\" value=\"%s\" onChange=CheckCheckObj(form1.check_txt_ntp_ip)></td>\n", protocol_communication.ntp_ip);
	printf("		<td width=26% >&nbsp;NTP Reset Interval</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_ntp_period\" style=\"WIDTH: 80pt;\" onChange=CheckCheckObj(form1.check_combo_ntp_period)>\n");
for(i=0;i<25;i++){
	printf("				<option value=\"%d\" %s>%d</option>\n", i, (i==protocol_communication.ntp_period)?"selected":"", i );
}
	printf("			</select>&nbsp;Hours</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");
	printf("</form>\n");

	printf("</body>\n");
	printf("</html>\n");
//	close(sockfd);
}

