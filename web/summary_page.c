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

int maxof3(int a, int b, int c)
{
	int ret;
	ret=a;
	if(ret<b) ret=b;
	if(ret<c) ret=c;
	return ret;
}


void summary_page()
{
	int sockfd;
	int client;
	int len=maxof3(sizeof(struct protocol_web_status), sizeof(struct protocol_communication), sizeof(struct protocol_web_alarm));
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;

	struct protocol_web_status protocol_status;
//	struct protocol_communication protocol_communication;
//	struct protocol_web_alarm protocol_web_alarm;

	unsigned char frame[len*2];
	char protocol_buffer[len*2];

	int frame_size;
	int buffer_size;

//	unsigned int bits_band;
//	int i, n;
	int n;

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


//====================================
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
	printf("%s", "<body bgcolor=\"#FFFFFF\" text=\"#000000\">\n");


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

	printf("%s", "<form name = \"form1\">\n");
//--------------------------------------------------------------------------------
	html_menu_head_left(&protocol_status);
	html_menu_head_gate(&protocol_status,0, 0, "Configuration Summary");
//--------------------------------------------------------------------------------
	printf("%s", "</form>\n");

//====================================================================================================================
// STATUS
//====================================================================================================================
	printf("%s", "<table width=\"746\" border=\"1\" height=\"45\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td colspan=\"4\" bgcolor=\"#CCCCCC\" height=\"40\" > <div align=\"center\"> \n");
	printf("%s", "          <font color=\"#000000\" size=\"3\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", "          <b>RF Status</b></font></td>");
	printf("%s", "    </tr> \n");
	printf("%s", "</table>\n");
#if 0
	printf("%s", 	"<table width=\"746\" border=\"1\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//Downlink output power
	printf("%s", 		"    <td width=\"200\" height=\"18\"><font color=\"#000000\" face=\"Arial, Helvetica, sans-serif\" size=\"2\">Downlink output power<br></font></td>\n");
	printf("%s", 		"    <td width=\"167\" height=\"18\"> <div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f%s","        <input type=\"text\" name=\"status_dl_output_power\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"", 
							protocol_status.dl_output_power ,"\">dBm</font></div></td>\n");
//Uplink output power
	printf("%s", 		"    <td width=\"202\" height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">Uplink output power</font></td>\n");
	printf("%s", 		"    <td width=\"165\" height=\"18\"> <div align=\"left\"> <font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f%s","        <input type=\"text\" name=\"status_ul_output_power\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly  value=\"", 
							protocol_status.ul_output_power ,	"\">dBm </font></div></td>\n");
	printf("%s", 		"  </tr>\n");
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//Downlink attenuation
	printf("%s", 		"    <td width=\"200\" height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">Downlink gain(attenuation)</font></td>\n");
	printf("%s", 		"    <td width=\"167\" height=\"18\"> <div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f(%.1f)%s","        <input type=\"text\" name=\"status_dl_system_att\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"", 
							90.-protocol_status.dl_system_att, protocol_status.dl_system_att ,"\">dB</font></div></td>\n");
//Uplink attenuation
	printf("%s", 		"    <td width=\"202\" height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">Uplink gain(attenuation)</font></td>\n");
	printf("%s", 		"    <td width=\"165\" height=\"18\"> <div align=\"left\"> <font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f(%.1f)%s","        <input type=\"text\" name=\"status_ul_system_att\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"",
							90.-protocol_status.ul_system_att ,protocol_status.ul_system_att ,"\">dB</font></div></td>\n");
	printf("%s", 		"  </tr>\n");
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
/*
//Downlink inband1 rssi
	printf("%s", 		"    <td width=\"200\" height=\"18\"><font color=\"#000000\" face=\"Arial, Helvetica, sans-serif\" size=\"2\">Downlink inband1 RSSI<br></font></td>\n");
	printf("%s", 		"    <td width=\"167\" height=\"18\"> <div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f%s","        <input type=\"text\" name=\"status_dl_inband1_rssi_power\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"", 
							protocol_status.dl_inband1_rssi_power ,"\">dBm</font></div></td>\n");
//Uplink inband2 rssi
	printf("%s", 		"    <td width=\"202\" height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">Downlink inband2 RSSI</font></td>\n");
	printf("%s", 		"    <td width=\"165\" height=\"18\"> <div align=\"left\"> <font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f%s","        <input type=\"text\" name=\"status_dl_inband2_rssi_power\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly  value=\"", 
							protocol_status.dl_inband2_rssi_power ,	"\">dBm </font></div></td>\n");
	printf("%s", 		"  </tr>\n");
*/
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//Downlink HPA
	printf("%s", 		"    <td height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">Downlink HPA</font></td>\n");
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", 		"      <input type=\"text\" name=\"status_dl_hpa_onoff\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"");
						if(protocol_status.dl_hpa_onoff){ printf("ON"); }else{ printf("OFF"); }
	printf("%s", 		"\"></font></td>\n");
//Uplink  HPA 
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">Uplink  HPA</font></td>\n");
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s", 		"      <input type=\"text\" name=\"status_ul_hpa_onoff\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"");
						if(protocol_status.ul_hpa_onoff){ printf("ON"); }else{ printf("OFF"); }
	printf("%s", 		"\"></font></td>\n");
	printf("%s", 		"  </tr>\n");
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//Downlink ALC power limit
	printf("%s", 		"    <td width=\"200\" height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">Downlink ALC power limit</font></td>\n");
	printf("%s", 		"    <td width=\"167\" height=\"18\"> <div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f%s","        <input type=\"text\" name=\"status_dl_alc_limit\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"", 
							protocol_status.dl_alc_limit ,"\">dBm </font></div></td>\n");
//Uplink ALC power limit
	printf("%s", 		"    <td width=\"200\" height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">Uplink ALC power limit</font></td>\n");
	printf("%s", 		"    <td width=\"167\" height=\"18\"> <div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f%s","        <input type=\"text\" name=\"status_ul_alc_limit\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"", 
							protocol_status.ul_alc_limit ,"\">dBm </font></div></td>\n");
	printf("%s", 		"  </tr>\n");
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//ALC control
	printf("%s", 		"    <td height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">ALC control</font></td>\n");
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", 		"      <input type=\"text\" name=\"status_alc_onoff\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=");
						if(protocol_status.alc_onoff){ printf("\"ON\""); }else{ printf("\"OFF\""); }
	printf("%s", 		"></font></td>\n");
//Shutdown control
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">Shutdown control</font></td>\n");
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s", 		"      <input type=\"text\" name=\"status_shutdown_onoff\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=");
						if(protocol_status.shutdown_onoff){ printf("\"ON\""); }else{ printf("\"OFF\""); }
	printf("%s", 		"></font></td>\n");
	printf("%s", 		"  </tr>\n");
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//Gain balance control
	printf("%s", 		"    <td height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">Gain balance control</font></td>\n");
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", 		"      <input type=\"text\" name=\"status_gain_balance_onoff\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=");
						if(protocol_status.gain_balance_onoff){ printf("\"ON\""); }else{ printf("\"OFF\""); }
	printf("%s", 		"></font></td>\n");
//Gain balance offset
	printf("%s", 		"    <td width=\"200\" height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">Gain balance offset</font></td>\n");
	printf("%s", 		"    <td width=\"167\" height=\"18\"> <div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f%s","        <input type=\"text\" name=\"status_gain_balance_offset\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"", 
							protocol_status.gain_balance_offset ,"\">dB </font></div></td>\n");
	printf("%s", 		"  </tr>\n");
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//AOC control
	printf("%s", 		"    <td height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">AOC control</font></td>\n");
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", 		"      <input type=\"text\" name=\"status_alosc_onoff\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=");
						if(protocol_status.alosc_onoff){ printf("\"ON\""); }else{ printf("\"OFF\""); }
	printf("%s", 		"></font></td>\n");
//AOC limit
	printf("%s", 		"    <td width=\"200\" height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">AOC limit</font></td>\n");
	printf("%s", 		"    <td width=\"167\" height=\"18\"> <div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f%s","        <input type=\"text\" name=\"status_isolation_detect_level\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"", 
							protocol_status.isolation_detect_level ,"\">dB </font></div></td>\n");
	printf("%s", 		"  </tr>\n");
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//Digital module version
	printf("%s", 		"    <td height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">Digital module version</font></td>\n");
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s%.1f%s","        <input type=\"text\" name=\"status_digital_mcu_version\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"", 
							protocol_status.digital_mcu_version ,"\"></font></div></td>\n");
//AOC value
	printf("%s", 		"    <td width=\"200\" height=\"18\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\" color=\"#000000\">AOC value</font></td>\n");
	printf("%s", 		"    <td width=\"167\" height=\"18\"> <div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f%s","        <input type=\"text\" name=\"status_isolation_detect_value\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"", 
							protocol_status.isolation_detect_value ,"\">dB </font></div></td>\n");
	printf("%s", 		"  </tr>\n");
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//Temperature Limit
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif size=\"2\"><font color=\"#000000\">Temperature limit </font></td>\n");
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.0f%s","      <input type=\"text\" name=\"status_temperature_limit\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"", 
							protocol_status.temperature_limit ,"\">`C </font></td>\n");
//Temperature
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif size=\"2\" color=\"#000000\">Temperature</font></td>\n");
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.0f%s","      <input type=\"text\" name=\"status_temperature\" style=\"WIDTH: 90pt; HEIGHT: 15pt \"  readonly value=\"", 
							protocol_status.temperature,"\">`C</font></td>\n");
	printf("%s", 		"  </tr>\n");
	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//Delay alarm report
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">Delay alarm report</font></td>\n");
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");

	printf("%s%d%s",	"        <input name=\"status_alarm_delay_min\" type=\"text\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"",
							protocol_status.alarm_delay_min," \">min </font></td>\n");
//Input RSSI limit
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">Downlink RSSI limit</font></td>\n");
	printf("%s", 		"    <td width=\"165\" height=\"18\"> <div align=\"left\"> <font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f%s","        <input type=\"text\" name=\"status_dl_input_rssi_limit\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly  value=\"", 
							protocol_status.dl_rssi_limit ,"\">dBm </font></div></td>\n");
	printf("%s", 	"  </tr>\n");

	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//Alarm incidence
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">Alarm incidence</font></td>\n");
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%d%s",	"        <input name=\"status_alarm_incedence\" type=\"text\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"",
							protocol_status.alarm_incidence," \">&#37 </font></td>\n");
//Downlink low output limit
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">Downlink low output limit</font></td>\n");
	printf("%s", 		"    <td height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s%.1f%s","        <input type=\"text\" name=\"status_dl_low_output_limit\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"", 
							protocol_status.dl_low_output_limit," \">dBm</font></td>\n");
	printf("%s", 	"  </tr>\n");

	printf("%s", "</table>\n");

	printf("%s", "<table width=\"746\" height=\"5\" border=\"0\" bgcolor=\"transparent\"><tr><td></td></tr></table>\n");


	char ch3_bit, ch_inband1[32], ch_inband2[32], val_inband1[32], val_inband2[32];
	ch3_bit=0;
	if(bits_band & (1<<16)) ch3_bit|=0x01;
	if(bits_band & (1<<14)) ch3_bit|=0x02;
	if(bits_band & (1<<12)) ch3_bit|=0x04;

	switch(ch3_bit)
	{
		case 1:	// 100
			strcpy(ch_inband1, "F2");		sprintf(val_inband1, "%.1f", protocol_status.dl_inband1_rssi_power);
			strcpy(ch_inband2, "none");		sprintf(val_inband2, "---");
			break;
		case 2:	// 010
			strcpy(ch_inband1, "F1");		sprintf(val_inband1, "%.1f", protocol_status.dl_inband1_rssi_power);
			strcpy(ch_inband2, "none");		sprintf(val_inband2, "---");
			break;
		case 3:	// 110
			strcpy(ch_inband1, "F1,F2");		sprintf(val_inband1, "%.1f", protocol_status.dl_inband1_rssi_power);
			strcpy(ch_inband2, "none");		sprintf(val_inband2, "---");
			break;
		case 4:	// 001
			strcpy(ch_inband1, "E");		sprintf(val_inband1, "%.1f", protocol_status.dl_inband1_rssi_power);
			strcpy(ch_inband2, "none");		sprintf(val_inband2, "---");
			break;
		case 5:	// 101
			strcpy(ch_inband1, "E");		sprintf(val_inband1, "%.1f", protocol_status.dl_inband1_rssi_power);
			strcpy(ch_inband2, "F2");		sprintf(val_inband2, "%.1f", protocol_status.dl_inband2_rssi_power);
			break;
		case 6:	//011
			strcpy(ch_inband1, "E,F1");		sprintf(val_inband1, "%.1f", protocol_status.dl_inband1_rssi_power);
			strcpy(ch_inband2, "none");		sprintf(val_inband2, "---");
			break;
		case 7:	
			strcpy(ch_inband1, "E,F1,F2,");	sprintf(val_inband1, "%.1f", protocol_status.dl_inband1_rssi_power);
			strcpy(ch_inband2, "none");		sprintf(val_inband2, "---");
			break;
		default:	
			strcpy(ch_inband1, "none");		sprintf(val_inband1, "---");
			strcpy(ch_inband2, "none");		sprintf(val_inband2, "---");
			break;
	}

	printf("%s", "<table width=\"746\" height=\"5\" border=\"0\" bgcolor=\"transparent\"><tr><td></td></tr></table>\n");
//RSSI
	printf("%s", "<table width=\"746\" border=\"1\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", 	"  <tr> \n");
	printf("%s", 	"    <td height=\"29\" colspan=\"4\">\n");
	printf("%s", 	"    <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><strong>Downlink inband RSSI</strong></font></div></td>\n");
	printf("%s", 	"  </tr>\n");

	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//Downlink inband1 -ch
	printf("%s", 		"    <td width=\"200\" height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">Inband1 band select</font></td>\n");
	printf("%s", 		"    <td width=\"167\" height=\"18\"><div align=\"left\"> <font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf(			"    <input name=\"status_inband1_ch\" type=\"text\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"%s \"> </font></td>\n", ch_inband1);
//Downlink inband1 RSSI-value
	printf("%s", 		"    <td width=\"202\" height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">RSSI</font></td>\n");
	printf("%s", 		"    <td width=\"165\" height=\"18\"> <div align=\"left\"> <font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
//	printf(			"    <input type=\"text\" name=\"status_inband1_val\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly  value=\"%s\">dBm </font></div></td>\n", val_inband1);
	printf(			"    <input type=\"text\" name=\"status_dl_inband1_rssi_power\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly  value=\"%s\">dBm </font></div></td>\n", val_inband1);
	printf("%s", 		"  </tr>\n");

	printf("%s", 		"  <tr bgcolor=\"#FFFFFF\"> \n");
//Downlink inband2 -ch
	printf("%s", 		"    <td width=\"200\" height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">Inband2 band select</font></td>\n");
	printf("%s", 		"    <td width=\"167\" height=\"18\"><div align=\"left\"> <font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
	printf(			"    <input name=\"status_inband2_ch\" type=\"text\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly value=\"%s \"> </font></td>\n", ch_inband2);
//Downlink inband2 RSSI-value
	printf("%s", 		"    <td width=\"202\" height=\"18\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">RSSI</font></td>\n");
	printf("%s", 		"    <td width=\"165\" height=\"18\"> <div align=\"left\"> <font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"> \n");
//	printf(			"    <input type=\"text\" name=\"status_inband2_val\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly  value=\"%s\">dBm </font></div></td>\n", val_inband2);
	printf(			"    <input type=\"text\" name=\"status_dl_inband2_rssi_power\" style=\"WIDTH: 90pt; HEIGHT: 15pt \" readonly  value=\"%s\">dBm </font></div></td>\n", val_inband2);
	printf("%s", 		"  </tr>\n");

	printf("%s", "</table>\n");
	printf("%s", "<table width=\"746\" height=\"5\" border=\"0\" bgcolor=\"transparent\"><tr><td></td></tr></table>\n");


//Band Select Status
	printf("%s", "<table width=\"746\" border=\"1\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", 	"  <tr> \n");
	printf("%s", 	"    <td height=\"29\" colspan=\"4\"><div align=\"left\"></div>\n");
	printf("%s", 	"      <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><strong>Band Select Status</strong></font></div></td>\n");
	printf("%s", 	"  </tr>\n");
//	printf("%s", "</table>\n");

//	printf("%s", "<table width=\"746\" border=\"1\" bgcolor=\"#CCCCCC\">\n");

	printf("%s", "  <tr bgcolor=\"#FFFFFF\"> \n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">A1</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">A2</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">B1</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">B2</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">C</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">D</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">E</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">F1</font></div></td>\n");
	printf("%s", "    <td><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">F2</font></div></td>\n");
	printf("%s", "  </tr>\n");

	printf("%s", "  <tr bgcolor=\"#FFFFFF\"> \n");
for(i=0; i<=16; i+=2)
{
	if((bits_band>>i)&1)
	{
		printf("%s", "    <td><div align=\"center\"> <font face=\"Arial, Helvetica, sans-serif\"> \n");
		printf("%s%d%s", "        <input readonly name=\"status_check_freq\" type=\"checkbox\" value=\"",i,"\" checked></font></div></td>\n");
	}
	else
	{
		printf("%s", "    <td><div align=\"center\"> <font face=\"Arial, Helvetica, sans-serif\"> \n");
		printf("%s%d%s", "        <input readonly name=\"status_check_freq\" type=\"checkbox\" value=\"",i,"\"></font></div></td>\n");
	}
} 
	printf("%s", "  </tr>\n");
	printf("%s", "</table>\n");
/*
	printf("%s", "<table width=\"746\" height=\"30\" border=\"1\" cellpading=\"1\" cellspacing=\"1\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", "  <tr bgcolor=\"#FFFFFF\"> \n");
	printf("%s", "      <td><div align=\"center\"> <font face=\"Arial, Helvetica, sans-serif\">&nbsp;&nbsp;</font></div></td>\n");
for(i=1; i<=15; i+=2)
{
	if((bits_band>>i)&1)
	{
		printf("%s", "    <td><div align=\"center\"> <font face=\"Arial, Helvetica, sans-serif\"> \n");
		printf("%s%d%s", "        <input name=\"status_check_freq\" type=\"checkbox\" disabled value=\"",i,"\" checked></font></div></td>\n");
	}
	else
	{
		printf("%s", "    <td><div align=\"center\"> <font face=\"Arial, Helvetica, sans-serif\"> \n");
		printf("%s%d%s", "        <input name=\"status_check_freq\" type=\"checkbox\" disabled value=\"",i,"\"></font></div></td>\n");
	}
}
	printf("%s", "      <td><div align=\"center\"> <font face=\"Arial, Helvetica, sans-serif\">&nbsp;&nbsp;</font></div></td>\n");
	printf("%s", "  </tr>\n");
	printf("%s", "</table>\n");
*/
	printf("%s", "<table width=\"746\" height=\"5\" border=\"0\" bgcolor=\"transparent\"><tr><td></td></tr></table>\n");
	printf("%s", "<table width=\"746\" height=\"160\" border=\"1\" cellpading=\"1\" cellspacing=\"1\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", "  <tr> \n");
	printf("%s", "    <td height=\"30\" colspan=\"4\"><div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Alarm Status</b></font></div></td>\n");
	printf("%s", "  </tr>\n");
	printf("%s", "  <tr> \n");
	printf("%s", "    <td width=\"40\" height=\"30\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Status</b></font></div></td>\n");
	printf("%s", "    <td width=\"309\"><div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Name</b></font></div></td>\n");
	printf("%s", "    <td width=\"40\"><div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Status</b></font></div></td>\n");
	printf("%s", "    <td width=\"328\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Name</b></font></div></td>\n");
	printf("%s", "  </tr>\n");

	printf("%s", "  <tr bgcolor=\"#FFFFFF\"> \n");
if(protocol_status.web_alarm.category_led[0] == 2){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_power_supply\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FF0000; border:0\" readonly></font></div></td>\n");
}else if(protocol_status.web_alarm.category_led[0]  == 1){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_power_supply\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FFFF00; border:0\" readonly></font></div></td>\n");
}else{
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_power_supply\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #00FF00; border:0\" readonly></font></div></td>\n");
}

	printf("%s", "    <td height=\"20\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">Power Supply</td> \n");
if(protocol_status.web_alarm.category_led[1]  == 2){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_ul_pwr_amp\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FF0000; border:0\" readonly></font></div></td>\n");
}else if(protocol_status.web_alarm.category_led[1]  == 1){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_ul_pwr_amp\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FFFF00; border:0\" readonly></font></div></td>\n");
}else{
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_ul_pwr_amp\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #00FF00; border:0\" readonly></font></div></td>\n");
}
	printf("%s", "    <td height=\"20\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">Uplink Power Amplifier</td> \n");
	printf("%s", "  </tr>\n");

	printf("%s", "  <tr bgcolor=\"#FFFFFF\"> \n");
if(protocol_status.web_alarm.category_led[2] == 2)	{
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_dl_pwr_amp\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FF0000; border:0\" readonly></font></div></td>\n");
}else if(protocol_status.web_alarm.category_led[2] == 1){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_dl_pwr_amp\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FFFF00; border:0\" readonly></font></div></td>\n");
}else{
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_dl_pwr_amp\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #00FF00; border:0\" readonly></font></div></td>\n");
}
	printf("%s", "    <td height=\"20\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">Downlink Power Amplifier</td> \n");
	
if(protocol_status.web_alarm.category_led[3]  == 2){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_lna\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FF0000; border:0\" readonly></font></div></td>\n");
}else if(protocol_status.web_alarm.category_led[3] == 1){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_lna\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FFFF00; border:0\" readonly></font></div></td>\n");
}else{
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_lna\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #00FF00; border:0\" readonly></font></div></td>\n");
}
	printf("%s", "    <td height=\"20\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">LNA</td> \n");
	printf("%s", "  </tr>\n");

	printf("%s", "  <tr bgcolor=\"#FFFFFF\"> \n");
if(protocol_status.web_alarm.category_led[4] == 2){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_ch_module\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FF0000; border:0\" readonly></font></div></td>\n");
}else if(protocol_status.web_alarm.category_led[4] == 1){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_ch_module\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FFFF00; border:0\" readonly></font></div></td>\n");
}else{
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_ch_module\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #00FF00; border:0\" readonly></font></div></td>\n");
}
	printf("%s", "    <td height=\"20\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">Digital Channel Module</td> \n");

if(protocol_status.web_alarm.category_led[5] == 2){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_modem\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FF0000; border:0\" readonly></font></div></td>\n");
}else if(protocol_status.web_alarm.category_led[5] == 1){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_modem\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FFFF00; border:0\" readonly></font></div></td>\n");
}else{
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_modem\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #00FF00; border:0\" readonly></font></div></td>\n");
}
	printf("%s", "    <td height=\"20\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">Modem, Electronic Door Label</td> \n");
	printf("%s", "  </tr>\n");

	printf("%s", "  <tr bgcolor=\"#FFFFFF\"> \n");
if(protocol_status.web_alarm.category_led[6] == 2){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_vswr\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FF0000; border:0\" readonly></font></div></td>\n");
}else if(protocol_status.web_alarm.category_led[6] == 1){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_vswr\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FFFF00; border:0\" readonly></font></div></td>\n");
}else{
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_vswr\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #00FF00; border:0\" readonly></font></div></td>\n");
}
	printf("%s", "    <td height=\"20\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">VSWR, ALC, RSSI</td> \n");

if(protocol_status.web_alarm.category_led[7] == 2){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_external\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FF0000; border:0\" readonly></font></div></td>\n");
}else if(protocol_status.web_alarm.category_led[7] == 1){
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_external\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #FFFF00; border:0\" readonly></font></div></td>\n");
}else{
	printf("%s", "    <td width=\"40\" height=\"20\" bgcolor=\"#FFFFFF\"><div align=\"center\">\n");
	printf("%s", "	<input type=\"text\" name=\"alarm_external\" style=\"WIDTH: 30pt; HEIGHT: 12pt; BACKGROUND-COLOR: #00FF00; border:0\" readonly></font></div></td>\n");
}
	printf("%s", "    <td height=\"20\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">External, Fan, and Door</td> \n");
	printf("%s", "  </tr>\n");

	printf("%s", "</table>\n");










	printf("%s", "<br>\n");
//====================================================================================================================
// Alarm Configuration
//====================================================================================================================
	memset(&protocol_web_alarm, 0, sizeof(protocol_web_alarm));
	protocol_web_alarm.type = PROTOCOL_ALARMCONFIGURATION_READ;
	protocol_buffer_to_frame((void*)&protocol_web_alarm,1,frame,&frame_size);
	sched_yield();
	sendto(sockfd, frame, 3, 0,(struct sockaddr*)&serveraddr,client);
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame),0,NULL,NULL);
	alarm(0);
	protocol_frame_to_buffer(frame,n,(unsigned char*)protocol_buffer,&buffer_size);
	memmove(&protocol_web_alarm,protocol_buffer,buffer_size);	
//====================================================================================================================
/*
{
char tmp[64];
sprintf(tmp, "n=%d, buffer_size=%d, sizeof(protocol_web_alarm)=%d\n", n, buffer_size, sizeof(protocol_web_alarm));
debuglog(tmp);
}
*/
	printf("%s", "<form name=\"form_alarmconfiguration\" method=\"POST\" action=\"html.cgi\">\n");
	printf("%s", "  <table width=\"746\" border=\"1\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"746\" height=\"30\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><font color=\"#000000\">Report Alarms&nbsp;</font></b>\n");
	printf("%s", "        <select name=\"combo_report_alarm\" style=\"WIDTH: 80; HEIGHT: 15pt\">\n");
	if(protocol_web_alarm.report_alarm)
	{
		printf("%s", "        <option value=\"1\" selected>ON</option>\n");
		printf("%s", "        <option value=\"0\">OFF</option>\n");
	}
	else
	{
		printf("%s", "        <option value=\"1\">ON</option>\n");
		printf("%s", "        <option value=\"0\"selected>OFF</option>\n");	
	}
	printf("%s", "        </select>\n");
	printf("%s", "      <font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", "          <b><font size=\"3\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	printf("%s", "          Alarm Configuration</font></b></td>\n");
	printf("%s", "    </tr>\n");
	printf("%s", "  </table>\n");

	printf("%s", "  <table width=\"746\" border=\"1\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"50\" height=\"30\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>No</b></font></div></td>\n");
	printf("%s", "      <td width=\"220\" height=\"30\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Name</b></font></div></td>\n");
	printf("%s", "      <td width=\"50\" height=\"30\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>State</b></font></div></td>\n");
	printf("%s", "      <td width=\"80\" height=\"30\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Active</b></font></div></td>\n");
	printf("%s", "      <td width=\"170\" height=\"30\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>SNMP Map</b></font></div></td>\n");
	printf("%s", "      <td width=\"50\" height=\"30\"> <div align=\"center\"><font color=\"#000000\" size=\"1\" face=\"Arial, Helvetica, sans-serif\"><b>Severity</b></font></div></td>\n");
	printf("%s", "      <td width=\"126\" height=\"30\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Sent  Time</b></font></div></td>\n");
	printf("%s", "    </tr>\n");
for(i=0; i<ALARM_INDEX_MAX; i++)
{
	printf("%s", "    <tr bgcolor=\"#FFFFFF\"> \n");
	printf("%s%d%s", "      <td height=\"18\"><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">",i,"</font></div></td>\n");

	printf("%s", "      <td height=\"18\"><div align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s", "          </font><font color=\"#FFFFCC\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	if(protocol_web_alarm.alarm_entry[i].description)
		printf("%s%d%s%s%s", "          <input readonly name=\"txt_alarmconfig_Description",i,"\" type=\"text\" style=\"WIDTH:220; HEIGHT: 14pt ;background: white \"  maxlength=\"31\" value=\"",protocol_web_alarm.alarm_entry[i].description,"\" >\n");
	else 
		printf("%s%d%s", "          <input readonly name=\"txt_alarmconfig_Description",i,"\" type=\"text\" style=\"WIDTH:220; HEIGHT: 14pt ;background: white \"  maxlength=\"32\" value=\"Not Value\" >\n");

	printf("%s", "          </font> </b></font></div></td>\n");

	printf("%s", "      <td height=\"18\"><div align=\"center\"><font color=\"#FFFFCC\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\">\n");

//____alarm configuration state 1____
	if(protocol_web_alarm.alarm_entry[i].state==2)
	{
		printf("%s%d%s", "          <input name=\"txt_alarmconfig_state",i,"\" type=\"text\" style=\"WIDTH:50; HEIGHT: 14pt ;background: white \" value=\"Alarm\" readonly >\n");
	}
	else 
	{
		printf("%s%d%s", "          <input name=\"txt_alarmconfig_state",i,"\" type=\"text\" style=\"WIDTH:50; HEIGHT: 14pt ;background: white \" value=\"Normal\" readonly>\n");
	}
	printf("%s", "          </font></b></font> </div></td>\n");
	printf("%s", "      <td height=\"18\"><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b><font color=\"#000000\"> \n");
	printf("%s%d%s", "          <select name=\"combo_active_",i,"\" style=\"WIDTH:70; HEIGHT: 15pt\">\n");
//____alarm active 1____
	if(!protocol_web_alarm.alarm_entry[i].active || (protocol_web_alarm.report_alarm == 0))
	{
//		printf("%s", "            <option value=\"1\">Enable</option>\n");
		printf("%s", "            <option value=\"0\"selected>Disable</option>\n");
	}
	else
	{
		printf("%s", "            <option value=\"1\"selected>Enable</option>\n");
//		printf("%s", "            <option value=\"0\">Disable</option>\n");
	}

	printf("%s", "          </select>\n");
	printf("%s", "          </font></b></font></div></td>\n");
//____ snmp_mapping 1____
	printf("%s", "      <td height=\"18\"><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b><font color=\"#000000\"> \n");
	printf("%s%d%s", "          <select name=\"combo_mapping_",i,"\" style=\"WIDTH:170; HEIGHT:15pt\">\n");
	int selno;
	if(protocol_web_alarm.report_alarm==0) 
	{
		selno=0;
	}
	else
	{
		switch(protocol_web_alarm.alarm_entry[i].category)
		{
			case CATEGORY_POWER_SUPPLY:
			case CATEGORY_UL_PWR_AMP:
			case CATEGORY_DL_PWR_AMP:
			case CATEGORY_LNA:
			case CATEGORY_CH_MODULE:
			case CATEGORY_MODEM:
			case CATEGORY_VSWR:
			case CATEGORY_EXTERNAL:		
				selno=protocol_web_alarm.alarm_entry[i].category; 
				break;
			default:
				selno=0;
				break;
		}
	}
	if(selno==CATEGORY_POWER_SUPPLY)	printf("<option value=\"1\"selected	>Power supply</option>\n");				
//	else									printf("<option value=\"1\"			>Power supply</option>\n");
	if(selno==CATEGORY_UL_PWR_AMP)		printf("<option value=\"3\"selected	>Uplink power amplifier</option>\n");		
//	else 								printf("<option value=\"3\"			>Uplink power amplifier</option>\n");
	if(selno==CATEGORY_DL_PWR_AMP)		printf("<option value=\"5\"selected	>Downlink power amplifier</option>\n");		
//	else 								printf("<option value=\"5\"			>Downlink power amplifier</option>\n");
	if(selno==CATEGORY_LNA)				printf("<option value=\"7\"selected	>LNA</option>\n");						
//	else 								printf("<option value=\"7\"			>LNA</option>\n");
	if(selno==CATEGORY_CH_MODULE)		printf("<option value=\"9\" selected	>Digital channel module</option>\n");		
//	else 								printf("<option value=\"9\" 			>Digital channel module</option>\n");
	if(selno==CATEGORY_MODEM)			printf("<option value=\"11\"selected	>Modem, electronic door</option>\n"); //Modem, electronic door label
//	else 								printf("<option value=\"11\"			>Modem, electronic door</option>\n");
	if(selno==CATEGORY_VSWR)			printf("<option value=\"13\"selected	>VSWR, ALC, RSSI</option>\n"); 			
//	else 								printf("<option value=\"13\"			>VSWR, ALC, RSSI</option>\n");
	if(selno==CATEGORY_EXTERNAL)		printf("<option value=\"15\"selected	>External, fan and door</option>\n"); 		
//	else 								printf("<option value=\"15\"			>External, fan and door</option>\n");

	printf("%s", "          </select>\n");
	printf("%s", "          </font></b></font></div></td>\n");
	printf("%s", "      <td height=\"18\"><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b><font color=\"#000000\"> \n");
	
	printf("%s%d%s%d%s", "          <input readonly name=\"txt_alarmconfig_severity",i,"\" type=\"text\" style=\"WIDTH:40; HEIGHT: 14pt ;background: white \" value=\"",protocol_web_alarm.alarm_entry[i].severity,"\">\n");
	printf("%s", "          </font></b></font></div></td>\n");

	printf("%s", "      <td height=\"18\"><div align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s", "          </font><font color=\"#FFFFCC\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
//____alarm cofig last trigged 1____
	printf("%s%d%s%s%s", "          <input name=\"txt_alarmconfig_trigger",i,"\" type=\"text\" style=\"WIDTH: 126; HEIGHT: 14pt ;background: white \" value=\"",protocol_web_alarm.alarm_entry[i].last_triggered,"\" readonly>\n");
	printf("%s", "          </font> </b></font></div></td>\n");
	printf("%s", "    </tr>\n");
}

	printf("%s", "  </table>\n");
	printf("%s", "</form>\n");










//====================================================================================================================
// Communication Configuration
//====================================================================================================================
	memset(&protocol_communication, 0, sizeof(protocol_communication));
	protocol_communication.Type=PROTOCOL_COMMUNICATIONS_READ;
	protocol_buffer_to_frame((void*)&protocol_communication,1,frame, &frame_size);
	sched_yield();
	sendto(sockfd, frame, 3,0,(struct sockaddr*)&serveraddr, client);
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);
	protocol_frame_to_buffer(frame, n, (unsigned char*)protocol_buffer, &buffer_size);
	memmove(&protocol_communication,protocol_buffer,buffer_size);

	char *Canada_Timezone[] = 
	{
		"Atlantic", 
		"Central", 
		"Eastern",
		"East-Saskatchewan",
		"Mountain",
		"Newfoundland",
		"Pacific",
		"Saskatchewan",
		"Yukon"
	};
	char buf[5]="";
//====================================================================================================================
	printf("%s", "<form name=\"form_communications\" method=\"POST\" action=\"html.cgi\">\n");
	printf("%s", "<table width=\"594\" border=\"1\" height=\"45\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td colspan=\"4\" bgcolor=\"#CCCCCC\" height=\"40\" > <div align=\"center\"> \n");
	printf("%s", "          <font color=\"#000000\" size=\"3\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", "          <b>Communication Configuration</b></font></td>");
	printf("%s", "    </tr> \n");
	printf("%s", "</table>\n");
	printf("%s", "  <table width=\"594\" border=\"1\" height=\"660\" bgcolor=\"#CCCCCC\">\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <p align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>LAN \n");
	printf("%s", "          port IP address (RJ45 port for local access)</b></font></p></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
//_____LanportIPAddress_____
//	printf("%s%s%s", "          <input type=\"text\" name=\"txt_lanport\" style=\"WIDTH: 120pt; HEIGHT: 15pt; \" maxlength=\"32\" readonly value=\"", protocol_communication.LanportIPAddress ,"\">\n");
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_lanport\" style=\"WIDTH: 120pt; HEIGHT: 15pt;background: silver \" maxlength=\"32\" readonly value=\"", protocol_communication.LanportIPAddress ,"\">\n");

	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <p align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Obtain \n");
	printf("%s", "          an IP address automatically</b></font></p></td>\n");
	printf("%s", "      <td width=\"106\" height=\"29\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
//_____IPAddressAutomatically_____
	if(protocol_communication.IPAddressAutomatically)
	{
//		printf("%s", "          <input type=\"radio\" name=\"radio_dhcp\" value=\"1\" onClick=\"radio_dhcp_click(value)\" checked>\n");
		printf("%s", "          <input type=\"radio\" name=\"radio_dhcp\" value=\"1\"  checked>\n");
		printf("%s", "          DHCP </font></div></td>\n");
		printf("%s", "      <td width=\"108\" height=\"29\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
//		printf("%s", "          <input type=\"radio\" name=\"radio_dhcp\" value=\"0\" onClick=\"radio_dhcp_click(value)\" >\n");
		printf("%s", "          <input type=\"radio\" name=\"radio_dhcp\" value=\"0\">\n");
		printf("%s", "          STATIC </font></div></td>\n");
	}
	else
	{
//		printf("%s", "          <input type=\"radio\" name=\"radio_dhcp\" value=\"1\" onClick=\"radio_dhcp_click(value)\" >\n");
		printf("%s", "          <input type=\"radio\" name=\"radio_dhcp\" value=\"1\">\n");
		printf("%s", "          DHCP </font></div></td>\n");
		printf("%s", "      <td width=\"108\" height=\"29\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
//		printf("%s", "          <input type=\"radio\" name=\"radio_dhcp\" value=\"0\" onClick=\"radio_dhcp_click(value)\"checked >\n");
		printf("%s", "          <input type=\"radio\" name=\"radio_dhcp\" value=\"0\" checked >\n");
		printf("%s", "          STATIC </font></div></td>\n");
	}

	printf("%s", "    </tr>\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><strong>IP \n");
	printf("%s", "          address</strong></font></div></td>\n");
	printf("%s", "      <td colspan=\"2\" height=\"29\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
//_____IPAddress_____
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_ipaddress\" style=\"WIDTH: 120pt; HEIGHT: 15pt; \" maxlength=\"32\" value=\"", protocol_communication.IPAddress ,"\">\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><strong>Netmask</strong></font></div></td>\n");
	printf("%s", "      <td colspan=\"2\" height=\"29\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
//_____Netmask_____
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_netmask\" style=\"WIDTH: 120pt; HEIGHT: 15pt; \"  maxlength=\"32\" value=\"", protocol_communication.Netmask ,"\">\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td height=\"29\"><div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><strong>Gateway</strong></font></div></td>\n");
	printf("%s", "      <td colspan=\"2\" height=\"29\"><div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
//_____Gateway_____
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_gateway\" style=\"WIDTH: 120pt; HEIGHT: 15pt; \"  maxlength=\"32\" value=\"", protocol_communication.Gateway ,"\">\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td height=\"29\"><div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><strong>WAN \n");
	printf("%s", "          Address </strong></font></div></td>\n");
	printf("%s", "      <td colspan=\"2\" height=\"29\"><div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\">\n");
//_____DNS_Sever_____
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_dns_server\" style=\"WIDTH: 120pt; HEIGHT: 15pt;background: silver\"  maxlength=\"32\" readonly value=\"", protocol_communication.WAN_address ,"\">\n");

	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td colspan=\"3\" bgcolor=\"#006699\" height=\"8\" bordercolor=\"#006699\"> <div align=\"right\"><font color=\"#006699\" size=\"1\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");


	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Wireless \n");
	printf("%s", "          Modem IP address</b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
//_____WirelessModemIPAddress_____
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_wirelessipaddress\" style=\"WIDTH: 120pt; HEIGHT: 15pt;background: silver\" maxlength=\"32\" value=\"", protocol_communication.WirelessModemIPAddress ,"\" readonly>\n");

	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>PPP \n");
	printf("%s", "          Connection</b></font></div></td>\n");
	printf("%s", "      <td width=\"106\" height=\"29\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
//_____PPPconfiguration_____
	if(protocol_communication.PPPconfiguration)
	{
		printf("%s", "          <input type=\"radio\" name=\"radio_ppp\" value=\"0\">\n");
		printf("%s", "          OFF</font></div></td>\n");
		printf("%s", "      <td width=\"108\" height=\"29\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
		printf("%s", "          <input type=\"radio\" name=\"radio_ppp\" value=\"1\" checked >\n");
		printf("%s", "          ON</font></div></td>\n");
	}
	else
	{
		printf("%s", "          <input type=\"radio\" name=\"radio_ppp\" value=\"0\" checked >\n");
		printf("%s", "          OFF</font></div></td>\n");
		printf("%s", "      <td width=\"108\" height=\"29\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
		printf("%s", "          <input type=\"radio\" name=\"radio_ppp\" value=\"1\">\n");
		printf("%s", "          ON</font></div></td>\n");
	}
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Modem \n");
	printf("%s", "          Reset Interval</b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s", "          <select name=\"combo_modem_reconnect_time\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" >\n");
	for(i=0; i<25; i++)
	{
		if(i == protocol_communication.modem_reconnect)
		{
			printf("%s%d%s%d%s", "          <option value=\"", i, "\" selected>", i ," hour</option>\n");
		}
		else
		{
			printf("%s%d%s%d%s", "          <option value=\"", i, "\">", i ," hour</option>\n");
		}
	
	}
	printf("%s", "        </select>\n");
	printf("%s", "    </tr>\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td colspan=\"3\" bgcolor=\"#006699\" height=\"8\" bordercolor=\"#006699\"> <div align=\"right\"><font color=\"#006699\" size=\"1\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Read SNMP \n");
	printf("%s", "          Community</b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_snmpconfiguration\" style=\"WIDTH: 120pt; HEIGHT: 15pt\"  maxlength=\"32\" value=\"", protocol_communication.RoSnmpCommunity ,"\">\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Write SNMP \n");
	printf("%s", "          Community</b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_snmpconfiguration\" style=\"WIDTH: 120pt; HEIGHT: 15pt;background: silver\"  maxlength=\"32\" value=\"", protocol_communication.RwSnmpCommunity ,"\">\n");
	printf("%s", "          </font></div></td>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Trap SNMP \n");
	printf("%s", "          Community</b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_snmpconfiguration\" style=\"WIDTH: 120pt; HEIGHT: 15pt\"  maxlength=\"32\" value=\"", protocol_communication.TrapSnmpCommunity ,"\">\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Alarm \n");
	printf("%s", "          Reporting IP address</b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
//_____AlarmReportingIPAddress_____
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_alarmreporting\" style=\"WIDTH: 120pt; HEIGHT: 15pt\"  maxlength=\"32\" value=\"", protocol_communication.AlarmReportingIPAddress ,"\">\n");

	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Alarming \n");
	printf("%s", "          Port</b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_alarmingport\" style=\"WIDTH: 120pt; HEIGHT: 15pt\"  maxlength=\"32\" value=\"", protocol_communication.AlarmingPort ,"\">\n");

	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");


	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>SNMP Receive \n");
	printf("%s", "          Port</b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_alarmingport\" style=\"WIDTH: 120pt; HEIGHT: 15pt\"  maxlength=\"9\" value=\"", protocol_communication.SNMP_Receive_Port ,"\">\n");

	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");


	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Alarm Send \n");
	printf("%s", "          Retry Time</b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");

	printf("%s", "          <select name=\"combo_snmp_retry_time\" style=\"WIDTH: 120pt; HEIGHT: 15pt\">\n");
	for(i=1; i<91; i++)
	{
		if(i == protocol_communication.snmp_retry_time)
		{
			printf("%s%d%s%d%s", "          <option value=\"", i, "\" selected>", i ," sec</option>\n");
		}
		else
		{
			printf("%s%d%s%d%s", "          <option value=\"", i, "\">", i ," sec</option>\n");
		}
	
	}
	printf("%s", "        </select>\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");


	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Alarm Send \n");
	printf("%s", "          Retry</b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s", "          <select name=\"combo_snmp_retry\" style=\"WIDTH: 120pt; HEIGHT: 15pt\">\n");
	for(i=0; i<31; i++)
	{
		if(i == protocol_communication.snmp_retry)
		{
			printf("%s%d%s%d%s", "          <option value=\"", i, "\" selected>", i ,"</option>\n");
		}
		else
		{
			printf("%s%d%s%d%s", "          <option value=\"", i, "\">", i ,"</option>\n");
		}
	
	}
	printf("%s", "        </select>\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td colspan=\"3\" bgcolor=\"#006699\" height=\"8\" bordercolor=\"#006699\"> <div align=\"right\"><font color=\"#006699\" size=\"1\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>  \n");
	printf("%s", "          Engineering Number </br> </b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_id\" style=\"WIDTH: 120pt; HEIGHT: 15pt\"  maxlength=\"35\" value=\"", protocol_communication.engr_no, "\">\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Site Name   \n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_longitude\" style=\"WIDTH: 120pt; HEIGHT: 15pt\"  maxlength=\"32\" value=\"", protocol_communication.site_name, "\">\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td colspan=\"3\" bgcolor=\"#006699\" height=\"8\" bordercolor=\"#006699\"> <div align=\"right\"><font color=\"#006699\" size=\"1\" face=\"Arial, Helvetica, sans-serif\">\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>NTP Server   \n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s%s%s", "          <input type=\"text\" name=\"txt_longitude\" style=\"WIDTH: 120pt; HEIGHT: 15pt\"  maxlength=\"32\" value=\"", protocol_communication.NTP_server, "\">\n");

	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Timezone \n");
	printf("%s", "          </b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s", "          <select name=\"combo_timezone\" style=\"WIDTH: 120pt; HEIGHT: 15pt\">\n");
	for(i=0; i<9; i++)
	{
		if(i == protocol_communication.timezone)
		{
			printf("%s%d%s%s%s", "          <option value=\"", i, "\" selected>", Canada_Timezone[i] ,"</option>\n");
		}
		else
		{
			printf("%s%d%s%s%s", "          <option value=\"", i, "\">", Canada_Timezone[i] ,"</option>\n");
		}
	}
	printf("%s", "        </select>\n");
	printf("%s", "          </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>NTP Reset Interval \n");
	printf("%s", "          </b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s", "          <select name=\"combo_ntp_period\" style=\"WIDTH: 120pt; HEIGHT: 15pt\" onChange=\"select_ntp_period_change(value)\">\n");
	for(i=0; i<25; i++)
	{
		if(i == protocol_communication.ntp_period)
		{
			printf("%s%d%s%d%s", "          <option value=\"", i, "\" selected>", i ," hour</option>\n");
		}
		else
		{
			printf("%s%d%s%d%s", "          <option value=\"", i, "\">", i ," hour</option>\n");
		}
	}
	printf("%s", "        </select>\n");
	printf("%s", "         </font></div></td>\n");
	printf("%s", "    </tr>\n");

	printf("%s", "    <tr> \n");
	printf("%s", "      <td width=\"358\" height=\"29\"> <div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>Current \n");
	printf("%s", "          Time </b></font></div></td>\n");
	printf("%s", "      <td height=\"29\" colspan=\"2\"> <div align=\"left\">\n");
	printf("%s", "          <p><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");

	if(protocol_communication.ntp_period)
	{
		buf[0]=protocol_communication.currenttime[8];
		buf[1]=protocol_communication.currenttime[9];
		buf[2]=protocol_communication.currenttime[10];
		buf[3]=protocol_communication.currenttime[11];
		buf[4] = '\0';
		printf("%s%s%s", "            <input type=\"text\" name=\"txt_year\" style=\"WIDTH: 40pt; HEIGHT: 15pt; background: silver\" readonly  maxlength=\"4\" value=\"",buf,"\">\n");
		printf("%s", "            _ \n");
		buf[0]=protocol_communication.currenttime[0];
		buf[1]=protocol_communication.currenttime[1];
		buf[2] = '\0';
		printf("%s%s%s", "            <input name=\"txt_month\" type=\"text\" style=\"WIDTH: 20pt; HEIGHT: 15pt; background: silver\"readonly maxlength=\"2\" value=\"",buf,"\">\n");
		printf("%s", "            / \n");
		buf[0]=protocol_communication.currenttime[2];
		buf[1]=protocol_communication.currenttime[3];
		buf[2] = '\0';
		printf("%s%s%s", "            <input name=\"txt_day\" type=\"text\" style=\"WIDTH: 20pt; HEIGHT: 15pt; background: silver\" readonly   maxlength=\"2\" value=\"",buf,"\">\n");
		printf("%s", "            _ \n");
		buf[0]=protocol_communication.currenttime[4];
		buf[1]=protocol_communication.currenttime[5];
		buf[2] = '\0';
		printf("%s%s%s", "            <input type=\"text\" name=\"txt_hour\" style=\"WIDTH: 20pt; HEIGHT: 15pt; background: silver\" readonly maxlength=\"2\" value=\"",buf,"\">\n");
		printf("%s", "            : \n");
		buf[0]=protocol_communication.currenttime[6];
		buf[1]=protocol_communication.currenttime[7];
		buf[2] = '\0';
		printf("%s%s%s", "            <input type=\"text\" name=\"txt_min\" style=\"WIDTH: 20pt; HEIGHT: 15pt; background: silver\" readonly maxlength=\"2\"value=\"",buf,"\">\n");
	}

	else
	{
		buf[0]=protocol_communication.currenttime[8];
		buf[1]=protocol_communication.currenttime[9];
		buf[2]=protocol_communication.currenttime[10];
		buf[3]=protocol_communication.currenttime[11];
		buf[4] = '\0';
		printf("%s%s%s", "            <input type=\"text\" name=\"txt_year\" style=\"WIDTH: 40pt; HEIGHT: 15pt\"  maxlength=\"4\" value=\"",buf,"\">\n");
		printf("%s", "            _ \n");
		buf[0]=protocol_communication.currenttime[0];
		buf[1]=protocol_communication.currenttime[1];
		buf[2] = '\0';
		printf("%s%s%s", "            <input name=\"txt_month\" type=\"text\" style=\"WIDTH: 20pt; HEIGHT: 15pt\"  maxlength=\"2\" value=\"",buf,"\">\n");
		printf("%s", "            / \n");
		buf[0]=protocol_communication.currenttime[2];
		buf[1]=protocol_communication.currenttime[3];
		buf[2] = '\0';
		printf("%s%s%s", "            <input name=\"txt_day\" type=\"text\" style=\"WIDTH: 20pt; HEIGHT: 15pt\"  maxlength=\"2\" value=\"",buf,"\">\n");
		printf("%s", "            _ \n");
		buf[0]=protocol_communication.currenttime[4];
		buf[1]=protocol_communication.currenttime[5];
		buf[2] = '\0';
		printf("%s%s%s", "            <input type=\"text\" name=\"txt_hour\" style=\"WIDTH: 20pt; HEIGHT: 15pt\"  maxlength=\"2\" value=\"",buf,"\">\n");
		printf("%s", "            : \n");
		buf[0]=protocol_communication.currenttime[6];
		buf[1]=protocol_communication.currenttime[7];
		buf[2] = '\0';
		printf("%s%s%s", "            <input type=\"text\" name=\"txt_min\" style=\"WIDTH: 20pt; HEIGHT: 15pt\"   maxlength=\"2\"value=\"",buf,"\">\n");
	}

	printf("%s", "            </font></p>\n");

	printf("%s", "          <p><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s", "            ex)year_month/day_hour:min</font></p>\n");
	printf("%s", "          <p><font color=\"#006699\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">2009_02/11_00:00</font></p>\n");
	printf("%s", "        </div></td>\n");
	printf("%s", "    </tr>\n");


	printf("%s", "  </table>\n");
	printf("%s", "</form>\n");
#endif





	printf("%s", "</body>\n");
	printf("%s", "</html>\n");
	close(sockfd);
}


