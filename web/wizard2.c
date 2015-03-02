#include <stdio.h>
#include "protocol.h"
#include "debug.h"
#include "main.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

void wizard2()
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
//	int i, n;
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
	printf("%s", "<!-- \n");
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

	printf("function CheckCheckObj(CheckObj)\n");
	printf("{\n");
	printf("	CheckObj.checked=\"true\";\n");
	printf("	CheckObj.value=\"1\";\n");
	printf("}\n");

	printf("function CheckboxEachBand(idx)\n");
	printf("{\n");
	printf("	CheckCheckObj(form_wizard2.check_band_select);\n");
	printf("	var pre_val=false, cnt1=0, cnt2=0, pos1=100, pos2=100, pos3=100;\n");
	printf("	var i;\n");
	printf("	document.form_wizard2.err_display_line.value=\"\";\n");
	printf("\n");
	printf("	for(i=0;i<9;i++){\n");
	printf("		if(pre_val==false && document.form_wizard2.check_band[i].checked==true){\n");
	printf("			if(pos1==100) pos1=i;\n");
	printf("			else if(pos2==100) pos2=i;\n");
	printf("			else if(pos3==100) pos3=i;\n");
	printf("		}\n");
	printf("		pre_val=document.form_wizard2.check_band[i].checked\n");
	printf("	}\n");
	printf("\n");
	printf("	if(pos3!=100){\n");
	printf("		document.form_wizard2.check_band[(idx-1)/2].checked=!document.form_wizard2.check_band[(idx-1)/2].checked;\n");/////
	printf("		document.form_wizard2.err_display_line.value=\"Band selection error: Too many band blocks(maximun 2)\";\n");
	printf("		return;\n");
	printf("	}\n");
	printf("\n");
	printf("	if(pos1<100){\n");
	printf("		for(i=pos1;i<9;i++){\n");
	printf("			if(document.form_wizard2.check_band[i].checked==true) cnt1++;\n");
	printf("			else break;\n");
	printf("		}\n");
	printf("	}\n");
	printf("\n");
	printf("	if(pos2<100){\n");
	printf("		for(i=pos2;i<9;i++){\n");
	printf("			if(document.form_wizard2.check_band[i].checked==true) cnt2++;\n");
	printf("			else break;\n");
	printf("		}\n");
	printf("	}\n");
	printf("\n");
	printf("	if(3<cnt1 || 3<cnt2){\n");
	printf("		document.form_wizard2.check_band[(idx-1)/2].checked=!document.form_wizard2.check_band[(idx-1)/2].checked;\n");/////
	printf("		document.form_wizard2.err_display_line.value=\"Band selection error: Too wide bandwidth(maximun 15MHz)\";\n");
	printf("		return;\n");
	printf("	}\n");
	printf("	if(3==cnt1 && 3==cnt2){\n");
	printf("		document.form_wizard2.check_band[(idx-1)/2].checked=!document.form_wizard2.check_band[(idx-1)/2].checked;\n");/////
	printf("		document.form_wizard2.err_display_line.value=\"Band selection error: Invalid selection(15MHz+15MHz is not possible)\";\n");
	printf("		return;\n");
	printf("	}\n");
	printf("}\n");

	printf("function CheckBandSelect()\n");
	printf("{\n");
	printf("	for(i=0;i<3;i++){\n");
	printf("		if(form_wizard2.check_band[i].checked==false) form_wizard2.check_band[i].value=0;\n");
	printf("		form_wizard2.check_band[i].checked=true;\n");	//if not checked, it will not be submitted
	printf("	}\n");
	printf("}\n");

	printf("%s", "\n");
	printf("%s", "//--> \n");
	printf("%s", "</script>\n");
	printf("%s", "\n");
	printf("%s", "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("%s", "</head>\n");
	printf("%s", "\n");
	
//====================================================================================================================

    printf("%s", "<body bgcolor=\"#FFFFFF\" text=\"#000000\">\n");
    printf("%s", "<form name = \"form1\">\n");
//--------------------------------------------------------------------------------
	html_menu_head_left(&protocol_status);
	html_menu_head_gate(&protocol_status,0, 0, "Setup wizard");
	printf("%s", "<br>");
//--------------------------------------------------------------------------------
	printf("%s", "  </form>");
#if 0
	printf("%s", "  <table width=\"746\" border=\"1\" height=\"50\" bgcolor=\"#ff6699\">\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td><div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", "          <b><font size=\"4\">Configuration setup wizard</font></b></font></div></td>\n");
	printf("%s", "    </tr>\n");
	printf("%s", "    </table>\n");

//Setup wizard 1
	printf("%s", "<form name=\"form_wizard2\" method=\"POST\" action=\"html.cgi\">\n");
	printf("%s", "	<input name=\"check_band_select\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");

	printf("%s", "  <table width=\"746\" border=\"1\" height=\"96\" bgcolor=\"#CCCCCC\">\n");

//Band Select Status
	printf("%s", "<table width=\"746\" border=\"1\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", 	"  <tr> \n");
	printf("%s", 	"    <td height=\"29\" colspan=\"9\"><div align=\"left\"></div>\n");
	printf("%s", 	"      <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><strong>Band Select Status</strong></font></div></td>\n");
	printf("%s", 	"  </tr>\n");
//	printf("%s", "</table>\n");

//	printf("%s", "<table width=\"746\" border=\"1\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", "  <tr bgcolor=\"#FFFFFF\"> \n");
#if 1
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">A1</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">A2</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">B1</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">B2</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">C</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">D</font></div></td>\n");
#endif
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">E</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">F1</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">F2</font></div></td>\n");
	printf("%s", "  </tr>\n");

	printf("%s", "  <tr bgcolor=\"#FFFFFF\"> \n");

//for(i=12; i<=16; i+=2)
for(i=0; i<=16; i+=2)
{
	if((bits_band>>i)&1)
	{
		printf("    <td><div align=\"center\"> <font face=\"Arial, Helvetica, sans-serif\"> \n");
		printf("        <input type=\"checkbox\" name=\"check_band\" value=\"%d\" checked onClick=\"CheckboxEachBand(value)\"></font></div></td>\n", i+1);// +1 to avoid zero value
	}
	else
	{
		printf("%s", "    <td><div align=\"center\"> <font face=\"Arial, Helvetica, sans-serif\"> \n");
		printf("        <input type=\"checkbox\" name=\"check_band\" value=\"%d\" onClick=\"CheckboxEachBand(value)\"></font></div></td>\n", i+1);// +1 to avoid zero value
	}
}
	printf("%s", "  </tr>\n");
	printf("%s", "  </table>\n");

	printf("%s", "        &nbsp;&nbsp;&nbsp;&nbsp;<input name=\"err_display_line\" type=\"text\" style=\"WIDTH:400pt;border:0;background:transparent;color:#ff0000;\"readonly>");
	

//Antenna selection - title
	printf("%s", "<table width=\"746\" border=\"1\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", 	"  <tr> \n");
	printf("%s", 	"    <td height=\"29\" colspan=\"4\">\n");
	printf("%s", 	"      <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><strong>Antenna selection</strong></font></div></td>\n");
	printf("%s", 	"  </tr>\n");

	printf("%s", "<br>");

//Antenna selection
	printf("%s", 	"  <tr bgcolor=\"#FFFFFF\"> \n");
	printf("%s", 	"    <td width=\"373\" height=\"18\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">Antenna selection</font></td>\n");
	printf("%s", 	"    <td height=\"18\"> <div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s", 	"        <select name=\"combo_antenna\" style=\"WIDTH: 200pt; HEIGHT: 15pt\">\n");
	printf("%s", 	"          <option value=\"1\" selected>Antenna only(AOC setting)</option>\n");
	printf("%s", 	"          <option value=\"2\" >DAS(ALC downlink limit 7dBm setting)</option>\n");
	printf("%s", 	"          <option value=\"3\" >DAS(ALC downlink limit maximun setting)</option>\n");
	printf("%s", 	"          <option value=\"4\" >Not use(HPA off)</option>\n");
	printf("%s", 	"        </select>\n");
	printf("%s", 	"    	   </font></div></td>\n");
	printf("%s", 	"  </tr>\n");

	printf("%s", "</table>\n");

//	printf("%s", "<table width=\"746\" height=\"5\" border=\"0\" bgcolor=\"transparent\"><tr><td></td></tr></table>\n");//spacer
	printf("%s", "<br>");

	printf("%s", "<table width=\"746\" border=\"0\" bgcolor=\"transparent\"><tr><td>\n");
	printf("%s", "	<div align=\"center\">\n");
	printf("%s", "  <button type=\"button\" name=\"skip\" style=\"WIDTH: 160pt; HEIGHT: 30pt\" onClick=\"location.href='html.cgi?function=status';this.disabled=true\"><b><font color=\"#000000\" size=\"2\">Skip</font></b></button>\n");
	printf("%s", "  &nbsp;&nbsp\n");
	printf("%s", "  <button type=\"button\" name=\"apply\" style=\"WIDTH: 160pt; HEIGHT: 30pt\" onClick=\"CheckBandSelect();this.form.submit();this.disabled=true\"><b><font color=\"#000000\" size=\"2\">Apply</font></b></button>\n");
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



