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

//65=6'07"
//65=4'30"
//58=3'57"
//55=3'40"
//50=3'25"
//20=1'20"
//14=0'58"
//10=0'40"
// 8=0'32"
void waiting_page(unsigned int time, char *title, char *message, char *alarm, char page_id)
{
	struct protocol_web_status protocol_status;
	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;
	unsigned char frame[sizeof(protocol_status)*2];
	int frame_size;
	int buffer_size;
	char protocol_buffer[sizeof(protocol_status)*2];
	int i, n;
	unsigned short crc_data;
	char page_name[64];

	client = sizeof(serveraddr);
	sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		message_dialog("socket error");
		exit(0);
	}
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sun_family = AF_UNIX;
	strcpy(serveraddr.sun_path, "/tmp/socket_app");
	bzero(&clientaddr, sizeof(clientaddr));
	clientaddr.sun_family = AF_UNIX;
	strcpy(clientaddr.sun_path, "/tmp/socket_web");
	remove("/tmp/socket_web");

	if(bind(sockfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr)))
	{
		message_dialog("socket error");
		exit(0);
	}
	memset(&protocol_status, 0, sizeof(protocol_status));
	protocol_status.type = PROTOCOL_STATUS;
	protocol_buffer_to_frame((void*)&protocol_status, 1,frame, &frame_size);
	sched_yield();
	sendto(sockfd, frame, 3,0,(struct sockaddr*)&serveraddr, client);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	protocol_frame_to_buffer(frame, n, (unsigned char*)protocol_buffer, &buffer_size);
	memmove(&protocol_status,protocol_buffer,buffer_size);

	crc_data = crc((void*)&protocol_status.type, (void*)&protocol_status.crc);
	if(crc_data != protocol_status.crc)
		message_dialog("crc error");

if(time)
{
//================================
	html_page_head(1);  // arg==requiredfrom true(1), false(0)
#if 0
	printf("Content-type: text/html\r\n\r\n");
	printf("<font face=\"Courier New\"><font size=1><font size=2><html>\n");
	printf("<head>\n");
	printf("<title>Sprint SmartCell</title>\n");
	printf("\n");
	printf("<script language=\"JavaScript\">\n");
	printf("<!--\n");
	printf("<!-- Begin\n");
	printf("var requiredfrom = \"html.cgi\"; //  required prev. page\n");
	printf("if (document.referrer.indexOf(requiredfrom) == -1) {\n");
	printf("alert(\"Sorry! You should register to access. Please go back to log-in.\" );\n");
	printf("window.location=requiredfrom;\n");
	printf("}\n");
	printf("\n");
	printf("//  End -->\n");
	printf("\n");
#endif
	printf("%s%d%s", "var loadtime=",time,"; // timer set\n");
	printf("var linkactive=\"Click result\";\n");
	printf("var doload=0;var doing=0;var loadsize=1;\n");
	printf("\n");

//	create_ajax_code();
	
	printf("%s", "function logout()\n");
	printf("%s", "{\n");
	printf("%s", "	if (document.cookie != \"\") \n");
	printf("%s", "	{\n");
	printf("%s", "        thisCookie = document.cookie.split(\"; \")\n");
	printf("%s", "        expireDate = new Date\n");
	printf("%s", "        expireDate.setDate(expireDate.getDate()-1)\n");
	printf("%s", "        for (i=0; i<thisCookie.length; i++) \n");
	printf("%s", "		{\n");
	printf("%s", "			document.cookie = thisCookie + \";expires=\" + expireDate.toGMTString()\n");
	printf("%s", "		}\n");
	printf("%s", "	}\n");
	printf("%s", "	location.replace(\"html.cgi?function=logout\");\n");
	printf("%s", "}\n");
	printf("%s", "\n");

	switch(page_id)
	{
		default:
		case PAGE_logout:					sprintf(page_name, "logout");			break;
		case PAGE_list:						sprintf(page_name, "list");				break;
		case PAGE_tree_status:				sprintf(page_name, "tree_status");		break;
		case PAGE_status:						sprintf(page_name, "status");			break;
		case PAGE_tree_rfconfig:				
		case PAGE_rfconfig:					sprintf(page_name, "rfconfig");			break;
		case PAGE_rfconfig_1:					sprintf(page_name, "rfconfig_1");			break;
		case PAGE_rfconfig_2:					sprintf(page_name, "rfconfig_2");			break;
		case PAGE_rfconfig_3:					sprintf(page_name, "rfconfig_3");			break;
		case PAGE_alarm_config:				sprintf(page_name, "alarm_config");		break;
		case PAGE_alarm_config_1:				sprintf(page_name, "alarm_config_1");	break;
		case PAGE_alarm_config_2:				sprintf(page_name, "alarm_config_2");	break;
		case PAGE_alarm_config_3:				sprintf(page_name, "alarm_config_3");	break;
		case PAGE_communication:				sprintf(page_name, "communications");	break;
		case PAGE_usermanagement:			sprintf(page_name, "usermanagement");	break;
		case PAGE_logs:						sprintf(page_name, "logs");				break;
		case PAGE_alarmlogs_1:				sprintf(page_name, "alarm_log");			break;
		case PAGE_alarmlogs_2:				sprintf(page_name, "alarm_log2");		break;
		case PAGE_alarmlogs_3:				sprintf(page_name, "alarm_log3");		break;
		case PAGE_alarmlogs_4:				sprintf(page_name, "alarm_log4");		break;
		case PAGE_troubleshooting:				sprintf(page_name, "troubleshooting");	break;
		case PAGE_system_download:			sprintf(page_name, "system_download");	break;
		case PAGE_reset:						sprintf(page_name, "reset");				break;
		case PAGE_reset_module:				sprintf(page_name, "reset_module");		break;
		case PAGE_systemreset:				sprintf(page_name, "systemreset");			break;
		case PAGE_factory_default_select:		sprintf(page_name, "factory_default_select");	break;
		case PAGE_factory_default:				sprintf(page_name, "factory_default");			break;
		case PAGE_default_rf:					sprintf(page_name, "default_rf");				break;
		case PAGE_default_alarm:				sprintf(page_name, "default_alarm");			break;
		case PAGE_modem_activate:			sprintf(page_name, "modem_activate");		break;
		case PAGE_default_user:				sprintf(page_name, "default_user");			break;
		case PAGE_default_communication:		sprintf(page_name, "default_communication");	break;
		case PAGE_transfer_except:				sprintf(page_name, "transfer_except");		break;
		case PAGE_transfer_except_download:	sprintf(page_name, "transfer_except_download");	break;
		case PAGE_transfer_except_upload:		sprintf(page_name, "transfer_except_upload");		break;
		case PAGE_upload_waiting:				sprintf(page_name, "upload_waiting");				break;
		case PAGE_summary_page:				sprintf(page_name, "summary_page");		break;
		case PAGE_wizard1:					sprintf(page_name, "wizard1");		break;
		case PAGE_wizard2:					sprintf(page_name, "wizard2");		break;
		case PAGE_fake_alarm_1:	sprintf(page_name, "fake_alarm_config");			break;
		case PAGE_fake_alarm_2:	sprintf(page_name, "fake_alarm_config_1");		break;
		case PAGE_fake_alarm_3:	sprintf(page_name, "fake_alarm_config_2");		break;
		case PAGE_fake_alarm_4:	sprintf(page_name, "fake_alarm_config_3");		break;
	}

	printf("%s", "function cont() \n");
	printf("%s", "{\n");
	printf("%s", "	if (doload==0 & loadsize>1) \n");
	printf("%s", "		loadsize=loadsize -1;\n");
	printf("%s", "	\n");
	printf("%s", "	if (doload==1 & loadsize<loadtime*20) \n");
//	printf("%s", "		loadsize=loadsize- -1;\n");
	printf("%s", "		loadsize=loadsize +1;\n");
	printf("%s", "	\n");
	printf("%s", "	loader.width=Math.round(loadsize/(loadtime*20)*(loadarea.width-2-(loadarea.border*2)));\n");
	printf("%s", "	\n");
	printf("%s", "	if (loadsize>1 & loadsize<loadtime*20) \n");
	printf("%s", "	{\n");
	printf("%s", "		setTimeout('cont()', 0300); \n");
	printf("%s", "		doing=1\n");
	printf("%s", "	}\n");
	printf("%s", "	else\n");
	printf("%s", "	{\n");
	printf("%s", "		doing=0\n");
	printf("%s", "	}\n");
	printf("%s", "	\n");
	printf("%s", "	if (loadsize>=loadtime*20)\n");
	printf("%s", "	{\n");
if(page_id==PAGE_logout)
	printf(         "      document.location.replace(\"html.cgi\"); \n");
else
	printf(         "      document.location.replace(\"html.cgi?function=%s\"); \n", page_name);
	printf("%s", "	}\n");
	printf("%s", "}\n");
	printf("%s", "function loading() \n");
	printf("%s", "{\n");
	printf("%s", "	doload=1;\n");
	printf("%s", "	if (doing==0) \n");
	printf("%s", "		cont();\n");
	printf("%s", "}\n");
	printf("%s", "\n");
	printf("%s", "\n");
	printf("%s", "//-->\n");
	printf("%s", "</script>\n");
	printf("%s", "\n");
	printf("%s", "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("%s", "</head>\n");
	printf("%s", "\n");
	printf("%s", "<body bgcolor=\"#FFFFFF\" text=\"#000000\" onload=\"loading(); \">\n");
//===============================================================================
	printf("%s", "<form name = \"form1\">\n");
	html_menu_head_gate(&protocol_status, 1, 0, title);
	printf("%s", "  </form>");

//================================
//=========1024*768 end===========
//================================

	printf("%s%s%s", "<p align=center><b><font color=\"#000000\" size=\"4\" face=\"Arial, Helvetica, sans-serif\">", message," ...\n"); 
//	printf("%s", "</font></b><table border=\"2\" id=\"loadarea\" cellspacing=\"0\" width=\"400\" cellpadding=\"0\" ><tr><td>\n");
	printf("%s", "</font></b><table border=\"2\" id=\"loadarea\" cellspacing=\"0\" width=\"500\" cellpadding=\"0\" ><tr><td>\n");
//	printf("%s", "</font></b><table border=\"2\" id=\"loadarea\" cellspacing=\"0\" width=\"600\" cellpadding=\"0\" ><tr><td>\n");
	printf("%s", "<table width=\"1\" height=\"30\" id=\"loader\" bgcolor=\"#000000\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr><td><font color=\"blue\">&nbsp;</font></td></tr></table></td></tr></table>\n");

	if(alarm != NULL)
	{
		printf("%s", "      <p><p align=\"center\">\n");
		printf("%s%s%s", "    <font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><font color=\"#FF0000\" size=\"4\">", alarm, "</font></a></b></font>\n");
	}
	printf("%s", "\n");

	for(i=0;i<1000;i++)
		printf("    ");

	printf("%s", "</body>\n");
	printf("%s", "</html>\n");
}

//    alarm(0);
if(sockfd)
	close(sockfd);
}


