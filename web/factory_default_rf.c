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

void factory_default_rf()
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
	printf("	var s00=\"alarm_led\";\n");
	printf("	var i00=detect.indexOf(s00)+s00.length+1;\n");
	printf("	var val_alarm_led = detect.substring(i00, i00+1);\n");//i00 to i00; 1 character
	printf("	var s01=\"shutdown_led\";\n");
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
	html_menu_head(&protocol_status, "Factory Default Setting - RF Configuration");
//--------------------------------------------------------------------------------
	printf("<br>\n");

	printf("<table width=\"740\" height=\"300\" border=\"1\">\n");
	printf("  <tr>\n");
	printf("    <td>\n");
	printf("      <p><p align=\"center\">\n");
	printf("      <font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><font color=\"#000000\" size=\"4\">Choose amplifier module to be restored to RF Configuration<br> factory default settings.</font></a></b></font>\n");
	printf("      <p>\n");
	printf("      <p><p align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><a href=\"html.cgi?function=default_rf1\"><font color=\"#000000\" size=\"4\">AMP1 module</font></a></b></font>\n");
	printf("      <p><p align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><a href=\"html.cgi?function=default_rf2\"><font color=\"#000000\" size=\"4\">AMP2 module</font></a></b></font>\n");
	printf("      <p><p align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><a href=\"html.cgi?function=default_rf3\"><font color=\"#000000\" size=\"4\">AMP3 module</font></a></b></font>\n");
	printf("      <p><p align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><a href=\"html.cgi?function=default_rf4\"><font color=\"#000000\" size=\"4\">AMP4 module</font></a></b></font>\n");
	printf("      <p><p align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><a href=\"html.cgi?function=default_rf\"><font color=\"#000000\" size=\"4\">All AMP modules</font></a></b></font>\n");
	printf("  </tr>\n");
	printf("</table>\n");

//	close(sockfd);
}


