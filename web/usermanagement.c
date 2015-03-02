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

void usermanagement()
{
	int amp_id=0;
	struct protocol_web_status protocol_status;
/*
    int sockfd;
    int client;
    struct sockaddr_un serveraddr;
    struct sockaddr_un clientaddr;
    unsigned char frame[sizeof(protocol_status)*2];
    int frame_size;
    int buffer_size;
    char protocol_buffer[sizeof(protocol_status)*2];
    int n;
    unsigned short crc_data;
*/
	if(get_status(&protocol_status)<0) return;

	html_page_head(0);  // arg==requiredfrom true(1), false(0)
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
	printf("\n");
	printf("	clearTimeout(time_id);\n");
	printf("	time_id = setTimeout(\"reload()\", 3000);\n");
	printf("}\n");

	printf("//-->\n");
	printf("</script>\n");
	printf("\n");

	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("\n");
	printf("</head>\n");
	printf("\n");
	printf("<body bgcolor=\"#FFFFFF\" text=\"#000000\" onLoad = \"reload()\">\n");

//--------------------------------------------------------------------------------
	html_menu_head(&protocol_status, "User Management");

//--------------------------------------------------------------------------------
	printf("<form name=\"form1\" method=\"POST\" action=\"html.cgi\">\n");
	printf("	<input name=\"check_user_register\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"check_user_delete\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");

	printf("<table cellspacing=\"0\" border=\"0\" width=\"740\">\n");
	printf("	<tr height=22 style='height:16.5pt'>\n");
	printf("		<td height=\"50\" colspan=\"2\" width=\"529\" rowspan=\"2\" bgcolor=\"#CCCCCC\" style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\">\n");
	printf("		<p align=\"left\"><font size=\"5\"><b>&nbsp;User Management</b></font></p></td>\n");
	printf("		<td width=\"136\" bgcolor=\"#CCCCCC\" colspan=\"3\" style=\"border-width:2; border-top-style:ridge; border-right-style:ridge; border-bottom-style:hidden; border-left-style:hidden;\">&nbsp</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=22>\n");
	printf("		<td bgcolor=\"#CCCCCC\" style=\"border-width:2; border-top-style:hidden; border-right-style:ridge; border-bottom-style:ridge; border-left-style:hidden;\">&nbsp</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");
	
	printf("<table cellspacing=\"0\" border=\"1\" width=\"740\">\n");
	printf("    <tr height=\"25\">\n");
	printf("        <td width=\"360\" colspan=\"2\"  bgcolor=\"#CCCCCC\" align=\"center\"><b>Edit User</b></td>\n");
	printf("        <td width=\"160\" rowspan=\"6\" style=\"padding:10\">User name and password must be 5~8 characters.</td>\n");
	printf("        <td width=\"180\" bgcolor=\"#CCCCCC\" align=\"center\"><b>User List</b></td>\n");
	printf("    </tr>\n");
	printf("    <tr>\n");
	printf("        <td width=\"180\">&nbsp;User Name</td>\n");
	printf("        <td width=\"180\" ><input type=\"text\" name=\"txt_user\" style=\"WIDTH:160; HEIGHT:20\" maxlength=\"8\"></td>\n");
	printf("        <td rowspan=\"4\" align=\"center\">\n");
	printf("        	<select name=\"list_user\" style=\"WIDTH:170;\" size=\"10\">\n");
					getuser();
	printf("			</select></td>\n");
	printf("    </tr>\n");
	printf("    <tr>\n");
	printf("        <td >&nbsp;Password</td>\n");
	printf("        <td ><input type=\"password\" name=\"txt_password\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"8\"></td>\n");
	printf("    </tr>\n");
	printf("    <tr>\n");
	printf("        <td >&nbsp;Password Confirm</td>\n");
	printf("        <td ><input type=\"password\" name=\"txt_passwordconfirm\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"8\"></td>\n");
	printf("    </tr>\n");
	printf("    <tr>\n");
	printf("        <td >&nbsp;Authority</td>\n");
	printf("        <td >\n");
	printf("        <select name=\"combo_authority\" style=\"WIDTH: 120pt; HEIGHT: 20pt\">\n");
	printf("			<option value=\"0\">Super User</option>\n");
	printf("			<option value=\"1\">Read/Write</option>\n");
	printf("			<option value=\"2\" selected>Read</option>\n");
	printf("        </select></td>\n");
	printf("    </tr>\n");
	printf("    <tr>\n");
	printf("        <td colspan=\"2\" align=\"center\">\n");
	printf("        	<button type=\"reset\" name=\"bt_reset\" style=\"WIDTH:120; HEIGHT: 30\">Clear</button> &nbsp;\n");
	printf("			<button type=\"submit\" name=\"bt_register\" style=\"WIDTH:120; HEIGHT: 30\" onClick=\"this.form.check_user_register.value=1;this.form.submit();this.disabled=true\">Register</button>\n");
	printf("			</td>\n");
	printf("		<td align=\"center\">\n");
	printf("			<button type=\"submit\" name=\"bt_delete\" style=\"WIDTH:120; HEIGHT:30\" onClick=\"this.form.check_user_delete.value=1;this.form.submit();this.disabled=true\"><font color=\"#006699\">Delete</button>\n");
	printf("			</td>\n");
	printf("    </tr>\n");
	printf("</table>\n");
	printf("</form>\n");

	printf("</body>\n");
	printf("</html>\n");
}
