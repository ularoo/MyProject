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

void transfer_except(int flag)
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
	html_menu_head(&protocol_status, "Configuration Transfer");//TITLE_TRANSFER);	//neil: simplify
//--------------------------------------------------------------------------------

	if(flag == TRANSFER_DOWNLOAD_STEP_ONE)
	{
		printf("%s", "</br>\n");
		printf("%s", "<table width=\"750\" height=\"200\" border=\"1\">\n");
		printf("%s", "  <tr>\n");
		printf("%s", "    \n");
		printf("%s", "    <td colspan=\"2\">\n");
		printf("%s", "      \n");
		printf("%s", "      <p align=\"center\">\n");
//		printf("%s", "    <font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><font color=\"#000000\" size=\"4\">What you want to Configuration Transfer Function this repeater ? </font></a></b></font>\n");
//		printf("%s", "  <p><p align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><a href=\"html.cgi?function=transfer_except_download\"><font color=\"#000000\" size=\"4\">Download</font></a></b></font>\n");
//		printf("%s", "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;    \n");
//		printf("%s", "    <font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><a href=\"html.cgi?function=transfer_except_upload\"><font color=\"#000000\" size=\"4\">Upload</font></a></b></font>\n");

		printf("%s", "    <font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><a href=\"html.cgi?function=transfer_except_download\"><font color=\"#000000\" size=\"4\">Download configurations from repeater to laptop.</font></a></b></font>\n");
		printf("%s", "      <p align=\"center\">\n");
		printf("%s", "    <font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><a href=\"html.cgi?function=transfer_except_upload\"><font color=\"#000000\" size=\"4\">Upload configuration from laptop to repeater.</font></a></b></font>\n");

		printf("%s", "\n");
		printf("%s", "\n");
		printf("%s", "  </tr>\n");
		printf("%s", "</table>\n");
	}

	else if(flag == TRANSFER_DOWNLOAD_STEP_TWO)
	{
		printf("%s", "</br>\n");
		printf("%s", "<table width=\"750\" height=\"150\" border=\"1\">\n");
		printf("%s", "  <tr>\n");
		printf("%s", "    \n");
		printf("%s", "    <td colspan=\"2\">\n");
		printf("%s", "      \n");
		printf("%s", "      <p><p align=\"center\">\n");
		printf("%s", "    <font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><font color=\"#000000\" size=\"4\">Download Transfer File  </font></a></b></font>\n");
		printf("%s", "      <p><p align=\"center\">\n");
//		printf("%s", "    <font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><a href=\"../transfer.tar.gz\" target=\"_blank\"><font color=\"#000000\" size=\"4\">transfer.tar.gz</font></a></b></font>\n");
		printf("%s", "    <font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><a href=\"../transfer.tgz\" target=\"_blank\"><font color=\"#000000\" size=\"4\">transfer.tgz</font></a></b></font>\n");
		printf("%s", "\n");
		printf("%s", "  </tr>\n");
		printf("%s", "</table>\n");
	}
}


