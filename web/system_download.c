#include <stdio.h>
#include "protocol.h"
#include "debug.h"
#include "main.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>


void system_download(int flag)
{
	int amp_id=0;
	struct protocol_web_status protocol_status;
	if(get_status(&protocol_status)<0) return;

	//char filename[32] = "\0";
	char filename[64] = "\0";
	char filesize[32] = "\0";

	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;

#if 0
	if((dp = opendir("/var/ftp")) == NULL)
		return;
#else
	if((dp = opendir("/dev/shm")) == NULL)
		return;
#endif

	while((entry = readdir(dp)) != NULL)
	{
		char path[256];
//		sprintf(path, "/var/ftp/%s", entry->d_name);
		sprintf(path, "/dev/shm/%s", entry->d_name);
		lstat(path, &statbuf);
		if(S_ISREG(statbuf.st_mode))
		{
			if(flag == DOWNLOAD_PAGE)
			{
#if 0
				if(!strncasecmp(entry->d_name, "smc_v", 5))
#else
				if((!strncasecmp(entry->d_name, "smc_sprint_v", 12)) || (!strncasecmp(entry->d_name, "dfm_", 4)))
#endif
				{
					sprintf(filename, "%s", entry->d_name);
					sprintf(filesize, "%d", statbuf.st_size);
				}
				else
				{
//					strcpy(filename, "Upgrade Error (Wrong file)\0");
//					system("rm /var/ftp/*");
					system("rm /dev/shm/*");
				}
			}
			else if(flag == TRANSFER_UPLOAD_PAGE)
			{
				if(!strncasecmp(entry->d_name, "transfer",8 ))
				{
					sprintf(filename, "%s", entry->d_name);
					sprintf(filesize, "%d", statbuf.st_size);
				}
				else
				{
//					strcpy(filename, "Upgrade Error (Wrong file)\0");
//					system("rm /var/ftp/*");
					system("rm /dev/shm/*");
				}
			}
		}
	}

	closedir(dp);

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

	printf("function changeType()\n");
	printf("{\n");
	printf("	document.getElementById(\"fpga_sel\").style.visibility = \"hidden\";\n");
	printf("	document.getElementById(\"fpga_sel\").style.display = \"none\";\n");
	printf("	document.getElementById(\"fpga_sel\").style.disable = \"true\";\n");

	printf("	if (document.form_system_download.upgradeType[1].checked) {\n");
	printf("		document.getElementById(\"fpga_sel\").style.visibility = \"visible\";\n");
	printf("		document.getElementById(\"fpga_sel\").style.display = \"\";\n");
	printf("		document.getElementById(\"fpga_sel\").style.disable = \"false\";\n");
	printf("	}\n");
	printf("}\n");

	printf("function CheckAll() {\n");
	printf("	var fileSearchVal, iValue;\n");
	printf("	var fileValue = document.form_system_download.txt_filename.value\n");
	printf("	if( (fileValue == \"\") || (fileValue.length == 0) ) {\n");
	printf("		alert(\"You have not selected the upgrade file\");\n");
	printf("		return;\n");
	printf("	} \n");
	printf("	if (document.form_system_download.upgradeType[1].checked) {\n");	// fpga upgrade
	printf("		var s1=%d;\n", protocol_status.status[0].svc_info);
	printf("		var s2=%d;\n", protocol_status.status[1].svc_info);
	printf("		var s3=%d;\n", protocol_status.status[2].svc_info);
	printf("		var s4=%d;\n", protocol_status.status[3].svc_info);
	//printf("		alert(\"mount svc : s1=\"+s1+\", s2=\"+s2+\", s3=\"+s3+\", s4=\"+s4);\n");
	//printf("		alert(\"sel slot=\"+document.form_system_download.slotNum.value+\", sel svc=\"+document.form_system_download.serviceType.value); \n");
	printf("		if (document.form_system_download.slotNum.value == 1) {\n");
	printf("			if (s1 != document.form_system_download.serviceType.value) {\n");
	printf("				alert(\"slot 1 : service type fail\");\n");
	printf("				return;\n");
	printf("			}\n");
	printf("		}\n");
	printf("		else if (document.form_system_download.slotNum.value == 2) {\n");
	printf("			if (s2 != document.form_system_download.serviceType.value) {\n");
	printf("				alert(\"slot 2 : service type fail\");\n");
	printf("				return;\n");
	printf("			}\n");
	printf("		}\n");
	printf("		else if (document.form_system_download.slotNum.value == 3) {\n");
	printf("			if (s3 != document.form_system_download.serviceType.value) {\n");
	printf("				alert(\"slot 3 : service type fail\");\n");
	printf("				return;\n");
	printf("			}\n");
	printf("		}\n");
	printf("		else if (document.form_system_download.slotNum.value == 4) {\n");
	printf("			if (s4 != document.form_system_download.serviceType.value) {\n");
	printf("				alert(\"slot 4 : service type fail\");\n");
	printf("				return;\n");
	printf("			}\n");
	printf("		}\n");
	printf("		fileSearchVal = \"dfm_\";\n");
	printf("		iValue = fileValue.indexOf(fileSearchVal);\n");
	printf("		if(iValue == -1) {\n");
	printf("			alert(\"Warning: this file is the Software upgrade files.\");\n");
	printf("			return;\n");
	printf("		} \n");
	printf("	} else {\n");	// software upgrade
	printf("		fileSearchVal = \"smc_sprint_v\";\n");
	printf("		iValue = fileValue.indexOf(fileSearchVal);\n");
	printf("		if(iValue == -1) {\n");
	printf("			alert(\"Warning: this file is the DFM Filter upgrade file.\");\n");
	printf("			return;\n");
	printf("		} \n");
	printf("	}\n");
	printf("	document.form_system_download.submit();\n");
	printf("}\n");


	printf("\n");
	printf("//--> \n");
	printf("</script> \n");
	printf("\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("</head>\n");
	printf("\n");

	printf("%s", "<body bgcolor=\"#FFFFFF\" text=\"#000000\" onLoad = \"reload()\">\n");
//--------------------------------------------------------------------------------
	if(flag == DOWNLOAD_PAGE)
	{
		html_menu_head(&protocol_status, "Remote Software Upgrade");//TITLE_UPGRADE);	//neil: simplify
	}
	else if(flag == TRANSFER_UPLOAD_PAGE)
	{ html_menu_head(&protocol_status, "Configuration Transfer");//TITLE_UPGRADE);	//neil: simplify
	}

//--------------------------------------------------------------------------------

	printf("%s", "<form method=\"POST\" enctype=\"multipart/form-data\" action=\"html.cgi\">\n");
	printf("%s", "<table width=\"551\" border=\"1\">\n");
	printf("%s", "  <tr> \n");
	printf("%s", "      <td width=\"541\"> <font face=\"Arial, Helvetica, sans-serif\" size=\"2\">\n");

	printf("%s", "        <input type=\"file\" name=\"file\" style=\"WIDTH: 400pt; HEIGHT: 20pt\">\n");

	printf("%s", "        </font> </td>\n");
	printf("%s", "  </tr>\n");
	printf("%s", "  <tr> \n");
	printf("%s", "    <td><div align=\"center\"><font color=\"#006699\" face=\"Arial, Helvetica, sans-serif\" size=\"2\"><font face=\"Arial, Helvetica, sans-serif\"> \n");
	printf("%s", "          <button type=\"submit\" name=\"upload\" style=\"WIDTH: 100pt; HEIGHT: 20pt\">");
	printf("%s", "          <font size=\"2\">Upload</font></button></font></font></div></td>\n");
	printf("%s", "  </tr>\n");
	printf("%s", "</table>\n");
	printf("%s", "</form>\n");
	printf("%s", "<form name=\"form_system_download\" method=\"POST\" action=\"html.cgi\">\n");
#if 1
	if(flag == DOWNLOAD_PAGE) {
		printf("<table width=\"551\" border=\"0\">\n");
		printf("	<tr>\n");
		printf("		<td>\n");
		printf("			<input type=\"radio\" name=\"upgradeType\" value=\"0\" onClick=\"changeType();\" checked> Upgrade Software &nbsp;&nbsp;\n");
		printf("			<input type=\"radio\" name=\"upgradeType\" value=\"1\" onClick=\"changeType();\"> Upgrade DFM Filter \n");
		printf("		</td>\n");
		printf("	</tr>\n");
		printf("	<tr id=\"fpga_sel\" style=\"display:none\">\n");
		printf("		<td>\n");
		printf("			slot<select name=\"slotNum\" style=\"WIDTH: 50; HEIGHT: 20pt\">\n");
		printf("				<option value=\"1\">1</option>\n");
		printf("				<option value=\"2\">2</option>\n");
		printf("				<option value=\"3\">3</option>\n");
		printf("				<option value=\"4\">4</option>\n");
		printf("			</select>&nbsp;&nbsp;\n");
		printf("			service<select name=\"serviceType\" style=\"WIDTH: 100; HEIGHT: 20pt\">\n");
		printf("				<option value=\"1\">LTE700</option>\n");
		printf("				<option value=\"2\">Cellular</option>\n");
		printf("				<option value=\"3\">iDEN</option>\n");
		printf("				<option value=\"4\">PCS</option>\n");
		printf("				<option value=\"5\">AWS</option>\n");
		printf("				<option value=\"6\">WiMAX</option>\n");
		printf("				<option value=\"7\">LTE2600</option>\n");
		printf("		</select></td>\n");
		printf("	</tr>\n");
		printf("</table></br>\n");
	}
#endif
	printf("%s", "<table width=\"551\" border=\"1\">\n");
	printf("%s", "  <tr> \n");
	printf("%s", "    <td width=\"25%\" bgcolor=\"cccccc\"><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">File \n");
	printf("%s", "        Name</font></div></td>\n");
	printf("%s", "    <td width=\"75%\"><div align=\"left\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");

//	printf("%s%s%s", "        <input type=\"text\" name=\"txt_filename\" style=\"WIDTH: 295pt; HEIGHT: 15pt\" value=\"",filename,"\" readonly>\n");
	if(flag == DOWNLOAD_PAGE)
	{
		printf("%s%s%s", "        <input type=\"text\" name=\"txt_filename\" style=\"WIDTH: 295pt; HEIGHT: 15pt\" value=\"",filename,"\" readonly>\n");
	}
	else if(flag == TRANSFER_UPLOAD_PAGE)
	{
		printf("%s%s%s", "        <input type=\"text\" name=\"txt_filename_transfer\" style=\"WIDTH: 295pt; HEIGHT: 15pt\" value=\"",filename,"\" readonly>\n");
	}

	printf("%s", "          </font></b> </font></div></td>\n");
	printf("%s", "  </tr>\n");
	printf("%s", "  <tr> \n");
	printf("%s", "    <td bgcolor=\"cccccc\"><div align=\"center\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">File \n");
	printf("%s", "        Size</font></div></td>\n");
	printf("%s", "    <td><div align=\"left\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"> \n");
	printf("%s%s%s", "        <input type=\"text\" name=\"txt_filesize\" style=\"WIDTH: 295pt; HEIGHT: 15pt\" value=\"",filesize,"\" readonly>\n");
	printf("%s", "          </font></b></font></div></td>\n");
	printf("%s", "  </tr>\n");
	printf("%s", "  <tr> \n");
	printf("%s", "    <td colspan=\"2\"><div align=\"center\"><font face=\"Arial, Helvetica, sans-serif\"> \n");

	if(flag == DOWNLOAD_PAGE) {	
		printf("%s", "  <input type=\"button\" name=\"upgrade\" style=\"WIDTH: 100pt; HEIGHT: 20pt\" value=\"Upgrade\" onClick=\"CheckAll()\" />\n");
		//printf("%s", "  <button type=\"submit\" name=\"upgrade\" style=\"WIDTH: 100pt; HEIGHT: 20pt\" onClick=\"this.form.submit();this.disabled=true\">");
		//printf("%s", "	<font size=\"2\">Upgrade</font></button>\n");
	} else if(flag == TRANSFER_UPLOAD_PAGE) {
		printf("%s", "  <button type=\"submit\" name=\"upgrade\" style=\"WIDTH: 100pt; HEIGHT: 20pt\" onClick=\"this.form.submit();this.disabled=true\">");
		printf("%s", "	<font size=\"2\">Transfer</font></button>\n");
	}

	printf("%s", "          </font> </div></td>\n");
	printf("%s", "  </tr>\n");
	printf("%s", "</table>\n");
	printf("%s", "</p>\n");
	printf("%s", "</p>\n");
	printf("%s", "<font color=\"#FF0000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b>");

	printf("%s", " Uploading via Wireless Modem may take a few minutes.</br>\n");
	printf("%s", " Please, do not reboot the repeater during uploading or upgrading process.</br>\n");
	printf("%s", " When uploading process is finished, the upgrade file name will appear in \"File Name\" menu.</br>\n");
	printf("%s", " After upload is done, click Upgrade.</br>\n");

	printf("%s", "</form>\n");
	printf("%s", "</body>\n");

//	close(sockfd);
}
