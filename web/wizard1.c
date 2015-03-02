#include <stdio.h>
#include "protocol.h"
#include "debug.h"
#include "main.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

void wizard1()
{
	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;
	struct protocol_web_status protocol_status;
	unsigned char frame[sizeof(protocol_status)*2];
	char protocol_buffer[sizeof(struct protocol_web_control)*2];
	int frame_size;
	int buffer_size;
	int n;
	unsigned short crc_data = 0;
//	unsigned int bits_band;

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

	protocol_buffer_to_frame((void*)&protocol_status,1,frame,&frame_size);
	sched_yield();
	sendto(sockfd, frame, 3, 0,(struct sockaddr*)&serveraddr,client);
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame),0,NULL,NULL);
	alarm(0);

	protocol_frame_to_buffer(frame,n,(unsigned char*)protocol_buffer,&buffer_size);
	memmove(&protocol_status,protocol_buffer,buffer_size);

//	bits_band = (protocol_status.band_select[0]<<16) | (protocol_status.band_select[1]<<8) | (protocol_status.band_select[2]);

	crc_data = crc((void*)&protocol_status.type, (void*)&protocol_status.crc);
	if(crc_data != protocol_status.crc)
	{
		message_dialog("crc error");
	}

//===========================

	html_page_head(0);  // arg==requiredfrom true(1), false(0)
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
	printf("%s", "\n");
	printf("%s", "//--> \n");
	printf("%s", "</script>\n");
	printf("%s", "\n");
	printf("%s", "\n");
	printf("%s", "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("%s", "\n");
	printf("%s", "</head>\n");
	printf("%s", "\n");
	
//====================================================================================================================
	memset(&protocol_status, 0, sizeof(protocol_status));
	protocol_status.type = PROTOCOL_STATUS;
	protocol_buffer_to_frame((void*)&protocol_status, 1,frame, &frame_size);
	sched_yield();
	sendto(sockfd, frame, 3,0,(struct sockaddr*)&serveraddr, client);
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);
	protocol_frame_to_buffer(frame, n, (unsigned char*)protocol_buffer, &buffer_size);
	memmove(&protocol_status,protocol_buffer,buffer_size);
//	bits_band = (protocol_status.band_select[0]<<16) | (protocol_status.band_select[1]<<8) | (protocol_status.band_select[2]);
//====================================================================================================================

    printf("%s", "<body bgcolor=\"#FFFFFF\" text=\"#000000\">\n");
    printf("%s", "<form name = \"form1\">\n");
//--------------------------------------------------------------------------------
	html_menu_head_left(&protocol_status);
	html_menu_head_gate(&protocol_status,0, 0, "Setup wizard");
	printf("%s", "<br>");
//--------------------------------------------------------------------------------
	printf("%s", "  </form>");

	printf("%s", "  <table width=\"746\" border=\"1\" height=\"50\" bgcolor=\"#ff6699\">\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td><div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", "          <b><font size=\"4\">Configuration setup wizard</font></b></font></div></td>\n");
	printf("%s", "    </tr>\n");
	printf("%s", "    </table>\n");
#if 0
//Setup wizard 1
	printf("%s", "<form name=\"form_wizard1\" method=\"POST\" action=\"html.cgi\">\n");
	printf("%s", "  <table width=\"746\" border=\"1\" bgcolor=\"#CCCCCC\">\n");

//Engineering number
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
	printf("%s", 		"    <td width=\"300\" height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">&nbsp;&nbsp;&nbsp;Engineering number</font></td>\n");
	printf("%s", 		"    <td height=\"18\"> <div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_engrno\" style=\"WIDTH:160pt; HEIGHT:15pt\"  maxlength=\"35\" value=\"", protocol_status.engr_no, "\">\n");
	printf("%s", 		"    	   </font></div></td>\n");
	printf("%s", 		"  </tr>\n");

//Site name
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
	printf("%s", 		"    <td width=\"300\" height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">&nbsp;&nbsp;&nbsp;Site name</font></td>\n");
	printf("%s", 		"    <td height=\"18\"> <div align=\"left\"> <font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%s%s",  "          <input type=\"text\" name=\"txt_sitename\" style=\"WIDTH:160pt; HEIGHT:15pt\"  maxlength=\"32\" value=\"", protocol_status.site_name, "\">\n");
	printf("%s", 		"    	   </font></div></td>\n");
	printf("%s", 		"  </tr>\n");

	printf("%s", "  </table>\n");

//	printf("%s", "<table width=\"746\" height=\"5\" border=\"0\" bgcolor=\"transparent\"><tr><td></td></tr></table>\n");
	printf("%s", "<br>");

	printf("%s", "<table width=\"746\" border=\"0\" bgcolor=\"transparent\"><tr><td>\n");
	printf("%s", "	<div align=\"center\">\n");
	printf("%s", "  <button type=\"button\" name=\"skip\" style=\"WIDTH: 160pt; HEIGHT: 30pt\" onClick=\"location.href='html.cgi?function=wizard2';this.disabled=true\"><b><font color=\"#000000\" size=\"2\">Skip</font></b></button>\n");
	printf("%s", "  &nbsp;&nbsp\n");
	printf("%s", "  <button type=\"button\" name=\"apply\" style=\"WIDTH: 160pt; HEIGHT: 30pt\" onClick=\"this.form.submit();this.disabled=true\"><b><font color=\"#000000\" size=\"2\">Apply</font></b></button>\n");
//	printf("%s", "   <button type=\"submit\" style=\"WIDTH: 160pt; HEIGHT: 30pt\" onClick=\"this.form.submit();this.disabled=true\"><font size=\"2\">Apply</font></button>\n");
	printf("%s", "	</div>\n");
	printf("%s", "</td></tr></table>\n");
	printf("%s", "</form>\n");
#endif
//===============================
	printf("%s", "</body>\n");
	printf("%s", "</html>\n");

	close(sockfd);
}



