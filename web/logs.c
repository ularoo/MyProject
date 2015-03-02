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

void logs(void)
{
	int amp_id=0;
	struct protocol_web_status protocol_status;

	if(get_status(&protocol_status)<0) return;

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
	html_menu_head(&protocol_status, "Log");

//--------------------------------------------------------------------------------
	printf("<form name=\"form1\" method=\"POST\" action=\"html.cgi\">\n");
	printf("<input name=\"log_clear\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("<table cellspacing=\"0\" border=\"0\" width=\"740\">\n");
	printf("	<tr height=22 style='height:16.5pt'>\n");
	printf("		<td height=\"50\" colspan=\"2\" width=\"529\" rowspan=\"2\" bgcolor=\"#CCCCCC\" style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\">\n");
	printf("		<p align=\"left\"><font size=\"5\"><b>&nbsp;Log</b></font></p></td>\n");
	printf("		<td width=\"136\" bgcolor=\"#CCCCCC\" colspan=\"3\" style=\"border-width:2; border-top-style:ridge; border-right-style:ridge; border-bottom-style:hidden; border-left-style:hidden;\">\n");
	printf("		<p align=\"right\"><input type=\"button\" style=\"width:136; font-size: 10pt; name=\"apply\" value=\"Clear Log\" onclick=\"this.form.submit();\"></p></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=22>\n");
	printf("		<td bgcolor=\"#CCCCCC\" style=\"border-width:2; border-top-style:hidden; border-right-style:ridge; border-bottom-style:ridge; border-left-style:hidden;\">\n");
	printf("		<p align=\"right\">&nbsp;</p></td>\n");
	printf("	</tr>\n");
	printf("</table>\n");
	printf("</form>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");


	printf("<table cellpadding=\"0\" cellspacing=\"0\" border=\"2\" width=\"740\">\n");
	printf("	<tr height=\"25\" bgcolor=\"#cccccc\" align=\"center\">\n");
	printf("    <td width=\"80\" > <b>number</b></font></td>\n");
	printf("    <td width=\"200\" > <b>Time</b></font></td>\n");
	printf("    <td width=\"80\" > <b>User</b></font></td>\n");
	printf("    <td width=\"200\" > <b>Operation</b></font></td>\n");
	printf("    <td width=\"180\" > <b>Description</b></font></td>\n");
	printf("  </tr>\n");

	get_log();

	printf("</table>\n");
	printf("<p>&nbsp;</p>\n");
	printf("</body>\n");
	printf("</html>\n");

//	close(sockfd);
}

