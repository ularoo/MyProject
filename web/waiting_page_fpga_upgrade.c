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

//void waiting_page_fpga_upgrade(unsigned int time, char *title, char *message, char *alarm, char page_id)
void waiting_page_fpga_upgrade(char *title, char *message, char *alarm)
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

	//================================
	html_page_head(1);  // arg==requiredfrom true(1), false(0)

	printf("var loadsize=1;\n");
	printf("var timer=3500; var speedx=10;\n");
	printf("var time_id;\n");

#if 1	/* progress_rate */
	create_ajax_code();
#endif
	printf("function logout() {\n");
	printf("	if (document.cookie != \"\") { \n");
	printf("		thisCookie = document.cookie.split(\"; \")\n");
	printf("		expireDate = new Date\n");
	printf("		expireDate.setDate(expireDate.getDate()-1)\n");
	printf("		for (i=0; i<thisCookie.length; i++) {\n");
	printf("			document.cookie = thisCookie + \";expires=\" + expireDate.toGMTString()\n");
	printf("		}\n");
	printf("	}\n");
	printf("	location.replace(\"html.cgi?function=logout\");\n");
	printf("}\n");

	printf("function loading() {\n");
	printf("	sendRequest(progress_rate,'','GET','/fpga_progress',true,true);\n");
	printf("	time_id = setTimeout('loading()', 60000); \n");
	printf("}\n");

	printf("function progress_rate(oj){\n");
	printf("    var detect = decodeURIComponent(oj.responseText);\n");

	// current packet no start index
	printf("    var s00=\"curr_packet_no\";\n");
	printf("    var i00=detect.indexOf(s00)+s00.length+1;\n");
	// total packet no start index
	printf("    var s01=\"tot_packet_no\";\n");
	printf("    var m01=detect.indexOf(s01);\n");
	printf("    var i01=detect.indexOf(s01)+s01.length+1;\n");
	// progress rate start index
	printf("    var s02=\"progress_rate\";\n");
	printf("    var m02=detect.indexOf(s02);\n");
	printf("    var i02=detect.indexOf(s02)+s02.length+1;\n");

	printf("    var val_curr_packet = detect.substring(i00, m01-1);\n");
	printf("    var val_tot_packet = detect.substring(i01, m02-1);\n");
	printf("    var val_progress_rate = detect.substring(i02, i02+3);\n");

	printf("	document.getElementById(\"progress_status\").innerHTML = \"(\"+val_curr_packet+\" / \"+val_tot_packet+\") \"+val_progress_rate+\"%\";\n");

	// progress bar
	printf("	var rate = parseInt(val_progress_rate);\n");
	printf("	if(rate>0)\n");
	printf("		loadsize=rate*5;\n");
	printf("	loader.width=loadsize;\n");

	printf("	clearTimeout(time_id);\n");
	printf("	if(val_progress_rate == '100') {\n");
	printf("		time_id = setTimeout('logout()', 30000); \n");
	printf("	} else {\n");
	printf("		time_id = setTimeout('loading()', timer); \n");
	printf("	}\n");
	printf("}\n");

	printf("</script>\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("</head>\n\n");

	printf("<body bgcolor=\"#FFFFFF\" text=\"#000000\" onload=\"loading(); \">\n");
	//===============================================================================
	printf("	<form name = \"form1\">\n");
	html_menu_head_gate(&protocol_status, 1, 0, title);
	printf("	</form>");

	//================================
	//=========1024*768 end===========
	//================================

	//printf("%s%s%s", "<p align=center><b><font color=\"#000000\" size=\"4\" face=\"Arial, Helvetica, sans-serif\">", message," ...\n"); 
	printf("	<p align=center><b><font color=\"#000000\" size=\"4\" face=\"Arial, Helvetica, sans-serif\">%s ...</font></b>\n", message);
	printf("	<table border=\"2\" id=\"loadarea\" cellspacing=\"0\" width=\"500\" cellpadding=\"0\" >\n");
	printf("		<tr>\n");
	printf("			<td>\n");
	printf("				<table width=\"1\" height=\"30\" id=\"loader\" bgcolor=\"#000000\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
	printf("					<tr>\n");
	printf("						<td>\n");
	printf("							<font color=\"blue\">&nbsp;</font>\n");
	printf("						</td>\n");
	printf("					</tr>\n");
	printf("				</table>\n");
	printf("			</td>\n");
	printf("		</tr>\n");
	printf("	</table>\n");
#if 1	/* progress_rate */
	printf("	<font size=\"3\"><b>\n");
	printf("	<p align=\"center\" id=\"progress_status\">(0/0) 0%</p>\n");
	printf("	</b></font>\n");
#endif
	if(alarm != NULL) {
		printf("	<p><p align=\"center\">\n");
		//printf("	<font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><font color=\"#FF0000\" size=\"4\">%s</font></a></b></font>\n", alarm);
		printf("	<font face=\"Arial, Helvetica, sans-serif\" size=\"3\"><b><font color=\"#FF0000\" size=\"4\">%s</font></b></font>\n", alarm);
		printf("	</p>\n");
	}
	for(i=0;i<1000;i++)
		printf("    ");

	printf("	</body>\n");
	printf("</html>\n");

	//    alarm(0);
	if(sockfd)
		close(sockfd);
}
