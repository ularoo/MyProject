#include <stdio.h>
#include "protocol.h"
#include "debug.h"
#include "main.h"
#include "crcmodel.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

void login()
{
	int amp_id=0;
	struct protocol_web_status protocol_status;
	if(get_status(&protocol_status)<0) return;
	if(get_status(&protocol_status)<0) return;//for sure


//=====================================
	html_page_head(0);  // arg==requiredfrom true(1), false(0)
	printf("<!-- \n");
	printf("%s", "function setcookie()\n");
	printf("%s", "{\n");
	printf("%s", "	if (document.cookie != \"\") \n");
	printf("%s", "	{\n");
	printf("%s", "		thisCookie = document.cookie.split(\"; \")\n");
	printf("%s", "		expireDate = new Date\n");
	printf("%s", "		expireDate.setDate(expireDate.getDate()-1)\n");
	printf("%s", "		for (i=0; i<thisCookie.length; i++) \n");
	printf("%s", "		{\n");
	printf("%s", "			document.cookie = thisCookie + \";expires=\" + expireDate.toGMTString()\n");
	printf("%s", "		}\n");
	printf("%s", "	}\n");
	printf("%s", "}\n");

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

	printf("%s", "\n");
	printf("%s", "function my_reload() {\n");
	printf("%s", "	document.form_login.login_txt_username.focus();\n");
	printf("%s", "	reload();\n");
	printf("%s", "}\n");

	printf("\n");
	printf("//--> \n");
	printf("</script> \n");
	printf("\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("</head>\n");
	printf("\n");

	printf("<body bgcolor=\"#FFFFFF\" text=\"#000000\" onLoad = \"my_reload()\">\n");

//--------------------------------------------------------------------------------
	html_menu_head_gate(&protocol_status,1, 1, "Login");
//--------------------------------------------------------------------------------
	printf("<br>");

//==============================================================
	printf("%s", "<form name=\"form_login\" method=\"POST\" action=\"html.cgi\">\n");
	printf("%s", "  <table width=\"650\" border=\"1\" height=\"20\" bordercolor=\"#000000\" align=\"center\">\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td height=\"42\" bordercolor=\"#006699\" bgcolor=\"#FFFFFF\"><div align=\"center\"><font color=\"#000000\" size=\"3\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", "        <b>User Name </b><input type=\"text\" name=\"login_txt_username\" maxlength=\"8\" size=\"19\" >\n");
	printf("%s", "        </font></div></td>\n");
	printf("%s", "      <td height=\"42\" bordercolor=\"#006699\"> <div align=\"center\"><font color=\"#000000\" size=\"3\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s", "          <b>Password </b><input type=\"password\" name=\"login_txt_password\" maxlength=\"8\">\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "      <td width=\"15%\" height=\"42\" bordercolor=\"#006699\"> <div align=\"center\"><font color=\"#000000\" size=\"3\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s", "          <button type=\"submit\" name=\"bt_login\" value=\"login_bt_login\" style=\"WIDTH: 80pt; HEIGHT: 30pt\" onClick=\"setcookie();this.form.submit();this.disabled=true\"><b>Login</b></button>\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");
	printf("%s", "  </table>\n");
	printf("%s", "</form>\n");
	printf("%s", "\n");
	printf("%s", "<div align=\"center\"></div>\n");
	printf("%s", "<div align=\"center\"> </div>\n");
	printf("%s", "<p align=\"center\">&nbsp;</p>\n");
	printf("%s", "</body>\n");
	printf("%s", "</html>\n");

//	close(sockfd);
}

