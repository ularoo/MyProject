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

void modem_activate_err(int flag)
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
	html_menu_head(&protocol_status, "Modem Activation");//TITLE_MODEM);	//neil: simplify
//--------------------------------------------------------------------------------


	printf("%s", "<form name=\"form_modem_activate\" method=\"POST\" action=\"html.cgi\">\n");
	printf("%s", "  <table width=\"500\" border=\"1\">\n");

	if(flag)
	{
			printf("%s", "    <tr> \n");
			printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">MODEM</font></b></td>\n");
			printf("%s", "      <td height=\"35\" width=\"295\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
			printf("%s", "          <input type=\"text\" name=\"txt_activate_modem\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\" readonly value=\"unknown modem\">\n");
			printf("%s", "          </font></b><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#FF0000\" size=\"1\">\n");
			printf("%s", "          </font></b></p></td>\n");
			printf("%s", "    </tr>\n");				
	}
	else
	{
			printf("%s", "    <tr> \n");
			printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">MODEM</font></b></td>\n");
			printf("%s", "      <td height=\"35\" width=\"295\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
			printf("%s", "          <input type=\"text\" name=\"txt_activate_modem\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\" readonly value=\"no modem detected\">\n");
			printf("%s", "          </font></b><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#FF0000\" size=\"1\">\n");
			printf("%s", "          </font></b></p></td>\n");
			printf("%s", "    </tr>\n");				
	}

///////////////////
	
	printf("%s", "  </table>\n");
	printf("%s", "</body>\n");
	printf("%s", "</html>\n");
	printf("%s", "</font>");
//	close(sockfd);
}

void modem_activate_wave()
{
//	int amp_id=0;
	struct protocol_web_status protocol_status;
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
	FILE* modem_read = NULL;
	//	ssize_t len=0;
	size_t len=0;
	char* line=NULL;

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
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);

	protocol_frame_to_buffer(frame, n, (unsigned char*)protocol_buffer, &buffer_size);
	memmove(&protocol_status,protocol_buffer,buffer_size);

	crc_data = crc((void*)&protocol_status.type, (void*)&protocol_status.crc);
	if(crc_data != protocol_status.crc)
		message_dialog("crc error");
		
//=====================================
	printf("%s", "Content-type: text/html\r\n\r\n");
	printf("%s", "<font size=1><html>\n");
	printf("%s", "<head>\n");
	printf("%s", "<title>Sprint SmartCell</title>\n");
	printf("%s", "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("%s", "<script language=\"JavaScript\">\n");
	printf("%s", "<!--\n");
	printf("%s", "<!-- Begin\n");
	printf("%s", "var requiredfrom = \"html.cgi\"; //  required prev. page\n");
	printf("%s", "if (document.referrer.indexOf(requiredfrom) == -1) {\n");
	printf("%s", "alert(\"Sorry! You should register to access. Please go back to log-in.\" );\n");
	printf("%s", "window.location=requiredfrom;\n");
	printf("%s", "}\n");
	printf("%s", "//  End -->\n");
	printf("%s", "\n");
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

	create_ajax_code();
	printf("%s", "\n");
	printf("%s", "function reload() {\n");
	printf("%s", "   sendRequest(status_load,'','GET','/web_status',true,true);\n");
	printf("%s", "time_id = setTimeout(\"reload()\", 60000);\n");
	printf("%s", "}\n");
	printf("%s", "\n");
	printf("%s", "  function status_load(oj)\n");
	printf("%s", "  {\n");
	printf("%s", "      var detect = decodeURIComponent(oj.responseText);\n");
	printf("%s", "          var shutdown_status = detect.substring(detect.indexOf(\"shutdown_status\") + 16, detect.indexOf(\"shutdown_status\") + 17);\n");
	printf("%s", "          var alarm_status = detect.substring(detect.indexOf(\"alarm_status\") + 13, detect.indexOf(\"alarm_status\") + 14);\n");

	printf("%s", "          if (shutdown_status == 1) document.form1.shutdown_status.style.background = \"#FF0000\";\n");
	printf("%s", "          else document.form1.shutdown_status.style.background = \"#00FF00\";\n");
	printf("%s", "          if (alarm_status == 1) document.form1.alarm_status.style.background = \"#FF0000\";\n");
	printf("%s", "          else document.form1.alarm_status.style.background = \"#00FF00\";\n");
	printf("%s", "\n");
	printf("%s", "  clearTimeout(time_id);\n");
	printf("%s", "  time_id = setTimeout(\"reload()\", 3000);\n");
	printf("%s", "  }\n");

	printf("%s", "\n");
	printf("%s", "function rst_func()\n");   
	printf("%s", "{\n");  
	printf("%s", "	document.form_modem_activate.txt_activate_code.value = \" \";\n");  
	printf("%s", "	document.form_modem_activate.txt_activate_min.value = \" \";\n");             
	printf("%s", "}\n"); 

	printf("%s", "\n");
	printf("%s", "//--></script>\n");
	printf("%s", "</head>\n");
	printf("%s", "\n");

	printf("%s", "<body bgcolor=\"#FFFFFF\" text=\"#000000\" onLoad = \"reload()\">\n");
	printf("%s", "<form name = \"form1\">\n");
//--------------------------------------------------------------------------------
	html_menu_head(&protocol_status, "Modem Activation");//neil: simplify
//--------------------------------------------------------------------------------
	printf("%s", "  </form>");

//======================================================================
	printf("%s", "<form name=\"form_modem_activate\" method=\"POST\" action=\"html.cgi\">\n");
	printf("%s", "  <table width=\"500\" border=\"1\">\n");	

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">MODEM</font></b></td>\n");
	printf("%s", "      <td height=\"35\" width=\"295\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s", "          <input type=\"text\" name=\"txt_activate_wavecom\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\" readonly value=\"Wavecom\">\n");
	printf("%s", "          </font></b><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#FF0000\" size=\"1\">\n");
	printf("%s", "          </font></b></p></td>\n");
	printf("%s", "    </tr>\n");

	if(access("/var/tmp/modem_esn", 0) == 0)	//modem_esn_read
	{
		modem_read=fopen("/var/tmp/modem_esn", "r");	//modem_esn_read
		getline(&line, &len, modem_read);
	}
	if(modem_read) fclose(modem_read);

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">ESN</font></b></td>\n");
	printf("%s", "      <td height=\"35\" width=\"295\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	if(line)
		printf("%s%s%s", "          <input type=\"text\" name=\"txt_activate_esn\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\" readonly value=\"",line,"\">\n");
	else
		printf("%s", "          <input type=\"text\" name=\"txt_activate_esn\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" readonly maxlength=\"32\">\n");
	printf("%s", "          </font></b><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#FF0000\" size=\"1\">\n");
	printf("%s", "          </font></b></p></td>\n");
	printf("%s", "    </tr>\n");

	line = NULL;
	len = 0;
	if(access("/var/tmp/modem_msl", 0) == 0)	//modem_msl_read
	{
		modem_read=fopen("/var/tmp/modem_msl", "r");	//modem_msl_read
		getline(&line, &len, modem_read);
	}
	if(modem_read) fclose(modem_read);

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">MSL CODE</font></b></td>\n");
	printf("%s", "      <td height=\"35\" width=\"295\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	if(line)
		printf("%s%s%s", "          <input type=\"text\" name=\"txt_activate_code\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\" value=\"", line, "\">\n");
	else
		printf("%s", "          <input type=\"text\" name=\"txt_activate_code\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\">\n");
	printf("%s", "          </font></b><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#FF0000\" size=\"1\">\n");
	printf("%s", "          </font></b></p></td>\n");
	printf("%s", "    </tr>\n");


	line = NULL;
	len = 0;
	if(access("/var/tmp/modem_min", 0) == 0)	//modem_min_read
	{
		modem_read=fopen("/var/tmp/modem_min", "r");	//modem_min_read
		getline(&line, &len, modem_read);
	}
	if(modem_read) fclose(modem_read);
	
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">MIN</font></b></td>\n");
	printf("%s", "      <td height=\"35\" width=\"295\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	if(line)
		printf("%s%s%s", "          <input type=\"text\" name=\"txt_activate_min\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\" value=\"", line, "\">\n");
	else
		printf("%s", "          <input type=\"text\" name=\"txt_activate_min\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\">\n");
	printf("%s", "          </font></b><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#FF0000\" size=\"1\">\n");
	printf("%s", "          </font></b></p></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">Read / Activation</font></b></td>\n");
	printf("%s", "      <td height=\"35\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s", "          <input type=\"radio\" name=\"radio_dhcp\" value=\"0\"  checked>\n");
	printf("%s", "          Read \n");
	printf("%s", "          <input type=\"radio\" name=\"radio_dhcp\" value=\"1\">\n");
	printf("%s", "          Activation </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b> \n");
	printf("%s", "          <button type=\"submit\" name=\"bt_register\" style=\"WIDTH: 60pt; HEIGHT: 15pt\" onClick=\"this.form.submit();this.disabled=true\"><b>Register</b></button>\n");
	printf("%s", "          </b></font></div></td>\n");
	printf("%s", "      <td height=\"35\" width=\"295\"> <div align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><font color=\"#006699\">\n");
	printf("%s", "          <button type=\"reset\" name=\"bt_reset\" style=\"WIDTH: 60pt; HEIGHT: 15pt\"><font color=\"#000000\"><b> \n");
	printf("%s", "          Reset</b></font></button>\n");
	printf("%s", "          </font></b></font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "  </table>\n");
	printf("%s", "</body>\n");
	printf("%s", "</html>\n");
	printf("%s", "</font>");
	close(sockfd);			
}


void modem_activate_tel()
{
//	int amp_id=0;
	struct protocol_web_status protocol_status;
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
	FILE* modem_read = NULL;
//	ssize_t len=0;
	size_t len=0;
	char* line=NULL;

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
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);

	protocol_frame_to_buffer(frame, n, (unsigned char*)protocol_buffer, &buffer_size);
	memmove(&protocol_status,protocol_buffer,buffer_size);

	crc_data = crc((void*)&protocol_status.type, (void*)&protocol_status.crc);
	if(crc_data != protocol_status.crc) message_dialog("crc error");

//=====================================
	printf("%s", "Content-type: text/html\r\n\r\n");
	printf("%s", "<font size=1><html>\n");
	printf("%s", "<head>\n");
	printf("%s", "<title>Sprint SmartCell</title>\n");
	printf("%s", "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("%s", "<script language=\"JavaScript\">\n");
	printf("%s", "<!--\n");
	printf("%s", "<!-- Begin\n");
	printf("%s", "var requiredfrom = \"html.cgi\"; //  required prev. page\n");
	printf("%s", "if (document.referrer.indexOf(requiredfrom) == -1) {\n");
	printf("%s", "alert(\"Sorry! You should register to access. Please go back to log-in.\" );\n");
	printf("%s", "window.location=requiredfrom;\n");
	printf("%s", "}\n");
	printf("%s", "//  End -->\n");
	printf("%s", "\n");
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

	create_ajax_code();

	printf("%s", "\n");
	printf("%s", "function reload() {\n");
	printf("%s", "   sendRequest(status_load,'','GET','/web_status',true,true);\n");
	printf("%s", "time_id = setTimeout(\"reload()\", 60000);\n");
	printf("%s", "}\n");
	printf("%s", "\n");
	printf("%s", "  function status_load(oj)\n");
	printf("%s", "  {\n");
	printf("%s", "      var detect = decodeURIComponent(oj.responseText);\n");
	printf("%s", "          var shutdown_status = detect.substring(detect.indexOf(\"shutdown_status\") + 16, detect.indexOf(\"shutdown_status\") + 17);\n");
	printf("%s", "          var alarm_status = detect.substring(detect.indexOf(\"alarm_status\") + 13, detect.indexOf(\"alarm_status\") + 14);\n");

	printf("%s", "          if (shutdown_status == 1) document.form1.shutdown_status.style.background = \"#FF0000\";\n");
	printf("%s", "          else document.form1.shutdown_status.style.background = \"#00FF00\";\n");
	printf("%s", "          if (alarm_status == 1) document.form1.alarm_status.style.background = \"#FF0000\";\n");
	printf("%s", "          else document.form1.alarm_status.style.background = \"#00FF00\";\n");
	printf("%s", "\n");
	printf("%s", "  clearTimeout(time_id);\n");
	printf("%s", "  time_id = setTimeout(\"reload()\", 3000);\n");
	printf("%s", "  }\n");
	printf("%s", "\n");
	printf("%s", "function rst_func() \n");   
	printf("%s", "{\n");  
	printf("%s", "	document.form_modem_activate.txt_activate_apn.value = \" \";\n");  
	printf("%s", "	document.form_modem_activate.txt_activate_usr.value = \" \";\n");          
	printf("%s", "	document.form_modem_activate.txt_activate_pass.value = \" \";\n");    
	printf("%s", "}\n");  

	printf("%s", "\n");
	printf("%s", "//--></script>\n");
	printf("%s", "</head>\n");
	printf("%s", "\n");

	printf("%s", "<body bgcolor=\"#FFFFFF\" text=\"#000000\" onLoad = \"reload()\">\n");
	printf("%s", "<form name = \"form1\">\n");

//--------------------------------------------------------------------------------
	html_menu_head(&protocol_status, "Modem Activation");//TITLE_MODEM);	//neil: simplify
//--------------------------------------------------------------------------------
    printf("%s", "  </form>");

//======================================================================
	printf("%s", "<form name=\"form_modem_activate\" method=\"POST\" action=\"html.cgi\">\n");
	printf("%s", "  <table width=\"500\" border=\"1\">\n");	

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">MODEM</font></b></td>\n");
	printf("%s", "      <td height=\"35\" width=\"295\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s", "          <input type=\"text\" name=\"txt_activate_telit\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\" readonly value=\"Telit\">\n");
	printf("%s", "          </font></b><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#FF0000\" size=\"1\">\n");
	printf("%s", "          </font></b></p></td>\n");
	printf("%s", "    </tr>\n");


	if(access("/var/tmp/modem_esn", 0) == 0)//modem_esn_read
	{
		modem_read=fopen("/var/tmp/modem_esn", "r");	//modem_esn_read
		getline(&line, &len, modem_read);
	}
	if(modem_read) fclose(modem_read);

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">ESN</font></b></td>\n");
	printf("%s", "      <td height=\"35\" width=\"295\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
if(line)
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_activate_esn\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\" readonly value=\"",line,"\">\n");
else
	printf("%s", "          <input type=\"text\" name=\"txt_activate_esn\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" readonly maxlength=\"32\">\n");
	printf("%s", "          </font></b><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#FF0000\" size=\"1\">\n");
	printf("%s", "          </font></b></p></td>\n");
	printf("%s", "    </tr>\n");

	line = NULL;
	len = 0;
	if(access("/var/tmp/modem_sim", 0) == 0)	//modem_sim_read
	{
		modem_read=fopen("/var/tmp/modem_sim", "r");	//modem_sim_read
		getline(&line, &len, modem_read);
	}
	if(modem_read) fclose(modem_read);
	
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">SIM ID</font></b></td>\n");
	printf("%s", "      <td height=\"35\" width=\"295\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
if(line)
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_activate_sim\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\" readonly value=\"", line, "\">\n");
else
	printf("%s", "          <input type=\"text\" name=\"txt_activate_sim\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" readonly maxlength=\"32\">\n");
	printf("%s", "          </font></b><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#FF0000\" size=\"1\">\n");
	printf("%s", "          </font></b></p></td>\n");
	printf("%s", "    </tr>\n");

	line = NULL;
	len = 0;
	if(access("/var/tmp/modem_apn", 0) == 0)
	{
		modem_read=fopen("/var/tmp/modem_apn", "r");	
		getline(&line, &len, modem_read);
	}
	if(modem_read) fclose(modem_read);
	
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">PPP APN</font></b></td>\n");
	printf("%s", "      <td height=\"35\" width=\"295\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
if(line)
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_activate_apn\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\" value=\"", line, "\">\n");
else
	printf("%s", "          <input type=\"text\" name=\"txt_activate_apn\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\">\n");
	printf("%s", "          </font></b><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#FF0000\" size=\"1\">\n");
	printf("%s", "          </font></b></p></td>\n");
	printf("%s", "    </tr>\n");	

	line = NULL;
	len = 0;
	if(access("/var/tmp/modem_usr", 0) == 0)
	{
		modem_read=fopen("/var/tmp/modem_usr", "r");	
		getline(&line, &len, modem_read);
	}
	if(modem_read) fclose(modem_read);
	
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">PPP USER</font></b></td>\n");
	printf("%s", "      <td height=\"35\" width=\"295\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
if(line)
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_activate_usr\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\" value=\"", line, "\">\n");
else
	printf("%s", "          <input type=\"text\" name=\"txt_activate_usr\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\">\n");
	printf("%s", "          </font></b><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#FF0000\" size=\"1\">\n");
	printf("%s", "          </font></b></p></td>\n");
	printf("%s", "    </tr>\n");		

	line = NULL;
	len = 0;
	if(access("/var/tmp/modem_pass", 0) == 0)
	{
		modem_read=fopen("/var/tmp/modem_pass", "r");	
		getline(&line, &len, modem_read);
	}
	if(modem_read) fclose(modem_read);
	
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#000000\" size=\"2\">PPP P/W</font></b></td>\n");
	printf("%s", "      <td height=\"35\" width=\"295\"> <p><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
if(line)
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_activate_pass\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\" value=\"", line, "\">\n");
else
	printf("%s", "          <input type=\"text\" name=\"txt_activate_pass\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" maxlength=\"32\">\n");
	printf("%s", "          </font></b><b><font face=\"Arial, Helvetica, sans-serif\" color=\"#FF0000\" size=\"1\">\n");
	printf("%s", "          </font></b></p></td>\n");
	printf("%s", "    </tr>\n");				

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"156\" height=\"35\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b> \n");
	printf("%s", "          <button type=\"submit\" name=\"bt_register\" style=\"WIDTH: 60pt; HEIGHT: 15pt\" onClick=\"this.form.submit();this.disabled=true\"><b>Register</b></button>\n");
	printf("%s", "          </b></font></div></td>\n");
	printf("%s", "      <td height=\"35\" width=\"295\"> <div align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><font color=\"#006699\">\n");
	printf("%s", "          <button type=\"button\" name=\"bt_reset\" style=\"WIDTH: 60pt; HEIGHT: 15pt\" onClick=\"rst_func();\"><font color=\"#000000\"><b> \n");
	printf("%s", "          Reset</b></font></button>\n");
	printf("%s", "          </font></b></font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "  </table>\n");
	printf("%s", "</body>\n");
	printf("%s", "</html>\n");
	printf("%s", "</font>");
	
	close(sockfd);			
}

void modem_activate_sierra()
{
	int amp_id=0, file_exist=0;
	struct protocol_web_status protocol_status;
	FILE* modem_read = NULL;
	size_t len=0;
	char* line=NULL;
	char tmp_str[64], tmp_str2[64];

	if(get_status(&protocol_status)<0) return;

//=====================================
	printf("%s", "Content-type: text/html\r\n\r\n");
	printf("%s", "<font size=1><html>\n");
	printf("%s", "<head>\n");
	printf("%s", "<title>Sprint SmartCell</title>\n");
	printf("%s", "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");

	printf("<style type=\"text/css\">\n");
	printf("body{\n");
	printf("	font-size:12px;\n");
	printf("	font-family:\"Arial, Helvetica, sans-serif\";\n");
	printf("	color:black;\n");
	printf("	line-height:20px;\n");
	printf("	text-indent:20px;\n");
	printf("	border-style:ridge;\n");
	printf("}\n");
	printf("</style>\n");
	
	printf("%s", "<script language=\"JavaScript\">\n");
	printf("%s", "<!--\n");
	printf("%s", "<!-- Begin\n");
	printf("%s", "var requiredfrom = \"html.cgi\"; //  required prev. page\n");
	printf("%s", "if (document.referrer.indexOf(requiredfrom) == -1) {\n");
	printf("%s", "alert(\"Sorry! You should register to access. Please go back to log-in.\" );\n");
	printf("%s", "window.location=requiredfrom;\n");
	printf("%s", "}\n");
	printf("%s", "//  End -->\n");
	printf("%s", "\n");

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

	printf("%s", "function rst_func() \n");   
	printf("%s", "{\n");  
	printf("%s", "	document.form_modem_activate.txt_activate_apn.value = \" \";\n");  
	printf("%s", "	document.form_modem_activate.txt_activate_usr.value = \" \";\n");          
	printf("%s", "	document.form_modem_activate.txt_activate_pass.value = \" \";\n");    
	printf("%s", "}\n");  

	printf("%s", "\n");
	printf("%s", "//--></script>\n");
	printf("%s", "</head>\n");
	printf("%s", "\n");

	printf("%s", "<body bgcolor=\"#FFFFFF\" text=\"#000000\" onLoad = \"reload()\">\n");

//--------------------------------------------------------------------------------
	html_menu_head(&protocol_status, "Modem Activation");//TITLE_MODEM);	//neil: simplify
//--------------------------------------------------------------------------------
	printf("<form name=\"form_modem_activate\" method=\"POST\" action=\"html.cgi\">\n");
	printf("<table cellspacing=\"0\" border=\"0\" width=\"740\">\n");
	printf("	<tr height=22 style='height:16.5pt'>\n");
	printf("		<td height=\"50\" colspan=\"2\" width=\"529\" rowspan=\"2\" bgcolor=\"#CCCCCC\" style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\">\n");
	printf("		<p align=\"left\"><font size=\"5\"><b>&nbsp;Modem Activation</b></font></p></td>\n");
	printf("		<td width=\"136\" bgcolor=\"#CCCCCC\" colspan=\"3\" style=\"border-width:2; border-top-style:ridge; border-right-style:ridge; border-bottom-style:hidden; border-left-style:hidden;\">\n");
	printf("		<p align=\"right\"><input type=\"button\" style=\"width:136; font-size: 10pt; name=\"apply\" value=\"Activate\" onclick=\"this.form.submit();\"></p></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=22>\n");
	printf("		<td bgcolor=\"#CCCCCC\" style=\"border-width:2; border-top-style:hidden; border-right-style:ridge; border-bottom-style:ridge; border-left-style:hidden;\">&nbsp</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");
	printf("	<input name=\"page_id_modem_activation\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");

	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>Modem information</b></font></td>\n");
	printf("	</tr>\n");

	memset(tmp_str, 0, sizeof(tmp_str));
	modem_read=fopen("/var/tmp/modem/modem_info", "r");
	if(modem_read)
	{
		file_exist=1;
		getline(&line, &len, modem_read);
		if( (strstr(line,"sierra")!=NULL) )
		{
			sprintf(tmp_str, "Sierra Wireless, Inc.");
		}
		fclose(modem_read);
	}
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Manufacturer</td>\n");
	printf("		<td width=\"70\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%s</td>\n", tmp_str);
	printf("	</tr>\n");

	memset(tmp_str, 0, sizeof(tmp_str));
	modem_read=fopen("/var/tmp/modem/modem_info", "r");
	if(modem_read)
	{
		file_exist=1;
		getline(&line, &len, modem_read);
		if( (strstr(line,"mc5728v")!=NULL) )
		{
			sprintf(tmp_str, "MC5728V");
		}
		fclose(modem_read);
	}
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Model</td>\n");
	printf("		<td width=\"70\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%s</td>\n", tmp_str);
	printf("	</tr>\n");

	memset(tmp_str, 0, sizeof(tmp_str));
	if(file_exist)
	{
		modem_read=fopen("/var/tmp/modem/modem_esn", "r");
		if(modem_read)
		{
			getline(&line, &len, modem_read);
			memcpy(tmp_str, line, len);
			fclose(modem_read);
		}
	}
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ESN</td>\n");
	printf("		<td width=\"70\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%s</td>\n", tmp_str);
	printf("	</tr>\n");

	memset(tmp_str, 0, sizeof(tmp_str));
	if(file_exist)
	{
		modem_read=fopen("/var/tmp/modem/modem_reg", "r");
		if(modem_read)
		{
			getline(&line, &len, modem_read);
			if(line[0]=='1') 
				sprintf(tmp_str, "800MHz Cellular");
			else if(line[0]=='2') 
				sprintf(tmp_str, "1900MHz PCS");
			else
				sprintf(tmp_str, "unsupported");
			fclose(modem_read);
		}
	}
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Band class</td>\n");
	printf("		<td width=\"70\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%s</td>\n", tmp_str);
	printf("	</tr>\n");

	memset(tmp_str, 0, sizeof(tmp_str));
	if(file_exist)
	{
		modem_read=fopen("/var/tmp/modem/modem_reg", "r");
		if(modem_read)
		{
			getline(&line, &len, modem_read);
			if(line[1]=='Z') 
				sprintf(tmp_str, "Modem has NOT registered");
			else
				sprintf(tmp_str, "Modem has registered");
			fclose(modem_read);
		}
	}
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Registration</td>\n");
	printf("		<td width=\"70\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%s</td>\n", tmp_str);
	printf("	</tr>\n");

	memset(tmp_str, 0, sizeof(tmp_str));
	if(file_exist)
	{
		modem_read=fopen("/var/tmp/modem/modem_status", "r");
		if(modem_read)
		{
			int ipn;
			float frssi;
			
			getline(&line, &len, modem_read);
			sscanf(line, "%x %f", &ipn, &frssi);
			sprintf(tmp_str, "%d", ipn);
			sprintf(tmp_str2, "%.1f dBm", frssi);
			fclose(modem_read);
		}
	}
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Pilot PN</td>\n");
	printf("		<td width=\"70\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%s</td>\n", tmp_str);
	printf("	</tr>\n");
	
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;RSSI</td>\n");
	printf("		<td width=\"70\%\" >&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%s</td>\n", tmp_str2);
	printf("	</tr>\n");

	printf("%s", "  </table>\n");
	printf("%s", "</body>\n");
	printf("%s", "</html>\n");
	printf("%s", "</font>");

}

