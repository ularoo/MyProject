#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "cgiutil.h"
#include "protocol.h"
#include "debug.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/file.h>

int g_udp_sock = -1;
struct sockaddr_in g_serveraddr;
int g_client=0;


struct get get[50];
struct post post[500];
int post_size = 0;
static char* message="Sorry! Your access is not permitted.";

//===============================================================================
//===============================================================================
// Tracking Issue #129 Setup Wizard
void pages_setup_wizard_rf_config(void)
{
	int index=0;
	unsigned char amp_id=0;
	
	if(strcmp(get[0].value, "setup_wizard_rfconfig") == 0)
		amp_id=0;
	else if(strcmp(get[0].value, "setup_wizard_rfconfig_1") == 0)
		amp_id=1;
	else if(strcmp(get[0].value, "setup_wizard_rfconfig_2") == 0)
		amp_id=2;
	else if(strcmp(get[0].value, "setup_wizard_rfconfig_3") == 0)
		amp_id=3;
	
	index=authentication();
	if((index==101)||(index==201))
	{
		setup_wizard_rfconfig(amp_id);
		logs_write(LOG_RF_CHECK);
	}
	else
		message_dialog(message);
}

void pages_setup_wizard_step4(void)
{
	if(strcmp(get[0].value, "setup_wizard_step4_status") == 0)
		setup_wizard_step4(0);
	else if(strcmp(get[0].value, "setup_wizard_step4_rfconfig") == 0)
		setup_wizard_step4(1);
}
//===============================================================================
//===============================================================================

void page_list(void)
{
	list();
}
void page_logout(void)
{
	login();
	logs_write(LOG_LOGOUT_CHECK);
}
void pages_tree(void)
{
	if(strcmp(get[0].value, "tree_status") == 0)
		tree(0);
	else if(strcmp(get[0].value, "tree_rfconfig") == 0)
		tree(1);
}
void pages_status(void)
{
	if(strcmp(get[0].value, "status") == 0)
		status(0);
	else if(strcmp(get[0].value, "status_1") == 0)
		status(1);
	else if(strcmp(get[0].value, "status_2") == 0)
		status(2);
	else if(strcmp(get[0].value, "status_3") == 0)
		status(3);
}
void pages_rf_config(void)
{
	int index=0;
	unsigned char amp_id=0;
	
	if(strcmp(get[0].value, "rfconfig") == 0)
		amp_id=0;
	else if(strcmp(get[0].value, "rfconfig_1") == 0)
		amp_id=1;
	else if(strcmp(get[0].value, "rfconfig_2") == 0)
		amp_id=2;
	else if(strcmp(get[0].value, "rfconfig_3") == 0)
		amp_id=3;
	
	index=authentication();
	if((index==101)||(index==201))
	{
		rfconfig(amp_id);
		logs_write(LOG_RF_CHECK);
	}
	else
		message_dialog(message);
}
void pages_alarm_config(void)
{
	int index=0;
	unsigned char amp_id=0;

	index=authentication();
	if((index!=101)&&(index!=201)) 
	{
		message_dialog(message);
		return;
	}
	
	if(strcmp(get[0].value, "alarm_config") == 0)
		amp_id=0;
	else if(strcmp(get[0].value, "alarm_config_1") == 0)
		amp_id=1;
	else if(strcmp(get[0].value, "alarm_config_2") == 0)
		amp_id=2;
	else if(strcmp(get[0].value, "alarm_config_3") == 0)
		amp_id=3;

	alarm_configuration(amp_id);
	logs_write(LOG_ALARM_CHECK);
}
void pages_fake_alarm_config(void)
{
	int index=0;
	unsigned char amp_id=0;

	index=authentication();
	if((index!=101)&&(index!=201)) 
	{
		message_dialog(message);
		return;
	}
	
	if(strcmp(get[0].value, "fake_alarm_config") == 0)
		amp_id=0;
	else if(strcmp(get[0].value, "fake_alarm_config_1") == 0)
		amp_id=1;
	else if(strcmp(get[0].value, "fake_alarm_config_2") == 0)
		amp_id=2;
	else if(strcmp(get[0].value, "fake_alarm_config_3") == 0)
		amp_id=3;

	fake_alarm_configuration(amp_id);
	logs_write(LOG_ALARM_CHECK);
}
void page_communication(void)
{
	int index=0;
	index=authentication();
	if((index!=101)&&(index!=201)) 
	{
		message_dialog(message);
		return;
	}
	communications();
	logs_write(LOG_COMMUNICATION_CHECK);
}
void page_user_management(void)
{
	int index=0;
	index=authentication();
	if(index==101)
	{
		usermanagement();
		logs_write(LOG_USER_ACCESS);
	}
	else
		message_dialog(message);
}
void page_logs(void)
{
	logs();
	logs_write(LOG_LOGS_CHECK);
}
void pages_alarm_log(void)
{
	unsigned char amp_id=0;

	if(strcmp(get[0].value, "alarm_log") == 0) amp_id=0;
	else if(strcmp(get[0].value, "alarm_log2") == 0) amp_id=1;
	else if(strcmp(get[0].value, "alarm_log3") == 0) amp_id=2;
	else if(strcmp(get[0].value, "alarm_log4") == 0) amp_id=3;

	if(amp_id==0) logs_write(LOG_ALARM_LOGS_CHECK);
	alarm_log(PAGE_1+amp_id);
}
#if 0	/* sprint */
void pages_osc_log(void)
{
	unsigned char amp_id=0;

	if(strcmp(get[0].value, "logs_osc") == 0) amp_id=0;
	else if(strcmp(get[0].value, "logs_osc2") == 0) amp_id=1;
	else if(strcmp(get[0].value, "logs_osc3") == 0) amp_id=2;
	else if(strcmp(get[0].value, "logs_osc4") == 0) amp_id=3;

	if(amp_id==0) logs_write(LOG_ALARM_LOGS_CHECK);
	logs_osc(PAGE_1+amp_id);
}
#endif
void page_troubleshooting(void)
{
	troubleshooting();
	logs_write(LOG_TROUBLE_CHECK);
}
void page_system_download(void)
{
	int index=0;
	index=authentication();
	if((index==101)||(index==201))
	{
		system_download(DOWNLOAD_PAGE);
		logs_write(LOG_SYSTEM_DOWNLOAD_CHECK);
	}
	else
		message_dialog(message);
}
void pages_reset(void)
{
	int index;
	index=authentication();
	if((index!=101)&&(index!=201)) 
	{
		message_dialog(message);
		return;
	}
	if(strcmp(get[0].value, "reset") == 0)
	{
   		reset();
	}
	else if(strcmp(get[0].value, "reset_module") == 0)
	{
   		reset_module();
	}
	else if(strcmp(get[0].value, "reset_amp_0") == 0)
	{
		waiting_page(4, "System Reset", "AMP1 reset", NULL, PAGE_reset_module);
		logs_write(LOG_SYSTEMRESET_CHECK);
		send_command(PROTOCOL_SYSTEMRESET_AMP1, FALSE);
	}
	else if(strcmp(get[0].value, "reset_amp_1") == 0)
	{
		waiting_page(4, "System Reset", "AMP2 reset", NULL, PAGE_reset_module);
		logs_write(LOG_SYSTEMRESET_CHECK);
		send_command(PROTOCOL_SYSTEMRESET_AMP2, FALSE);
	}
	else if(strcmp(get[0].value, "reset_amp_2") == 0)
	{
		waiting_page(4, "System Reset", "AMP3 reset", NULL, PAGE_reset_module);
		logs_write(LOG_SYSTEMRESET_CHECK);
		send_command(PROTOCOL_SYSTEMRESET_AMP3, FALSE);
	}
	else if(strcmp(get[0].value, "reset_amp_3") == 0)
	{
		waiting_page(4, "System Reset", "AMP4 reset", NULL, PAGE_reset_module);
		logs_write(LOG_SYSTEMRESET_CHECK);
		send_command(PROTOCOL_SYSTEMRESET_AMP4, FALSE);
	}
	else if(strcmp(get[0].value, "reset_snmp") == 0)
	{
		waiting_page(10, "System Rebooting", "SNMP module reboot", NULL, PAGE_logout);
		logs_write(LOG_SYSTEMRESET_CHECK);
		send_command(PROTOCOL_SYSTEMRESET_SNMP, FALSE);
	}
	else if(strcmp(get[0].value, "reset_all") == 0)
	{
		waiting_page(10, "System Rebooting", "reset all", NULL, PAGE_logout);
		logs_write(LOG_SYSTEMRESET_CHECK);
		send_command(PROTOCOL_SYSTEMRESET_ALL, FALSE);
	}
}
void pages_factory_default(void)
{
	int index;
	index=authentication();
	if((index!=101)&&(index!=201)) 
	{
		message_dialog(message);
		return;
	}
	if(strcmp(get[0].value, "factory_default_select") == 0)
	{
		factory_default_select();
	}
	else if(strcmp(get[0].value, "factory_default_rf") == 0)
	{
		factory_default_rf();
	}
	else if(strcmp(get[0].value, "factory_default_alarm") == 0)
	{
		factory_default_alarm();
	}
	else if(strcmp(get[0].value, "factory_default") == 0)
	{
		factory_default();
	}
}
#if 0	/* sprint */
void page_modem_activate(void)
{
	int index;
	index=authentication();
	if((index==101)||(index==201))
	{
#if 0
		FILE* modem_read = NULL;
		size_t len=0;
		char* line=NULL;

		modem_read=fopen("/var/tmp/modem/modem_info", "r");	
		if(modem_read)
		{
			getline(&line, &len, modem_read);

			if(strstr(line,"wavecom")!=NULL)
				modem_activate_wave();
			else if(strstr(line,"telit")!=NULL)
				modem_activate_tel();
			else if(strstr(line,"sierra")!=NULL)
				modem_activate_sierra();
			else	
				modem_activate_err(1);
			
		}
		else
			modem_activate_err(0);

		if(modem_read) 
			fclose(modem_read);
#else
		modem_activate_sierra();
#endif
	}
	else
		message_dialog(message);
}
#endif
void page_default(void)
{
	int index;
	index=authentication();
	if((index!=101)&&(index!=201)) 
	{
		message_dialog(message);
		return;
	}
	if(strcmp(get[0].value, "default_rf1") == 0)
	{
		waiting_page(1, "Factory Default Setting", "RF Configuration", NULL, PAGE_rfconfig);
		send_command(PROTOCOL_DEFAULT_RF1, TRUE);
	}
	else if(strcmp(get[0].value, "default_rf2") == 0)
	{
		waiting_page(1, "Factory Default Setting", "RF Configuration", NULL, PAGE_rfconfig_1);
		send_command(PROTOCOL_DEFAULT_RF2, TRUE);
	}
	else if(strcmp(get[0].value, "default_rf3") == 0)
	{
		waiting_page(1, "Factory Default Setting", "RF Configuration", NULL, PAGE_rfconfig_2);
		send_command(PROTOCOL_DEFAULT_RF3, TRUE);
	}
	else if(strcmp(get[0].value, "default_rf4") == 0)
	{
		waiting_page(1, "Factory Default Setting", "RF Configuration", NULL, PAGE_rfconfig_3);
		send_command(PROTOCOL_DEFAULT_RF4, TRUE);
	}
	else if(strcmp(get[0].value, "default_rf") == 0)
	{
		waiting_page(1, "Factory Default Setting", "RF Configuration", NULL, PAGE_rfconfig);
		send_command(PROTOCOL_DEFAULT_RF, TRUE);
	}

	else if(strcmp(get[0].value, "default_alarm1") == 0)
	{
		waiting_page(1, "Factory Default Setting", "Alarm Configuration", NULL, PAGE_alarm_config);
		send_command(PROTOCOL_DEFAULT_ALARM1, TRUE);
	}
	else if(strcmp(get[0].value, "default_alarm2") == 0)
	{
		waiting_page(1, "Factory Default Setting", "Alarm Configuration", NULL, PAGE_alarm_config_1);
		send_command(PROTOCOL_DEFAULT_ALARM2, TRUE);
	}
	else if(strcmp(get[0].value, "default_alarm3") == 0)
	{
		waiting_page(1, "Factory Default Setting", "Alarm Configuration", NULL, PAGE_alarm_config_2);
		send_command(PROTOCOL_DEFAULT_ALARM3, TRUE);
	}
	else if(strcmp(get[0].value, "default_alarm4") == 0)
	{
		waiting_page(1, "Factory Default Setting", "Alarm Configuration", NULL, PAGE_alarm_config_3);
		send_command(PROTOCOL_DEFAULT_ALARM4, TRUE);
	}
	else if(strcmp(get[0].value, "default_alarm") == 0)
	{
		waiting_page(1, "Factory Default Setting", "Alarm Configuration", NULL, PAGE_alarm_config);
		send_command(PROTOCOL_DEFAULT_ALARM, TRUE);
	}

	else if(strcmp(get[0].value, "default_user") == 0)
	{
		waiting_page(1, "Factory Default Setting", "User Configuration", NULL, PAGE_usermanagement);
		send_command(PROTOCOL_DEFAULT_USER, TRUE);
	}

	else if(strcmp(get[0].value, "default_communication") == 0)
	{
		waiting_page(10, "Communication Configuration", "set default", NULL, PAGE_logout);
		send_command(PROTOCOL_DEFAULT_COMM, FALSE);
	}
}
void pages_transfer(void)
{
	int index;
	if(strcmp(get[0].value, "transfer_except") == 0)
	{
		index=authentication();
		if((index==101)||(index==201))
		{
			transfer_except(TRANSFER_DOWNLOAD_STEP_ONE);
		}
		else
			message_dialog(message);
	}
	else if(strcmp(get[0].value, "transfer_except_download") == 0)
  	{
		send_command(PROTOCOL_TRANSFER_EXCEPT, TRUE);
		sleep(2);
		transfer_except(TRANSFER_DOWNLOAD_STEP_TWO);
	}
	else if(strcmp(get[0].value, "transfer_except_upload") == 0)
  	{
       	system_download(TRANSFER_UPLOAD_PAGE);
	}
}
void page_upload_waiting(void)
{
	upload_waiting();
}
void page_summary_page(void)
{
	summary_page();
}
void pages_wizard(void)
{
	if(strcmp(get[0].value, "wizard1") == 0)
	{
		wizard1();
	}
	else if(strcmp(get[0].value, "wizard2") == 0)
	{
		wizard2();
	}
}

int main()
{
	char *qs = NULL;
	int i=0;
	int cl=0;
	int index=0;
	int lock = -1;
	int lock_count = 0;
	char lock_master_pid[10] = " ";
	char command[128] = " ";

	if((lock = open("/var/tmp/socket_web.lock", O_CREAT|O_RDWR, 0644)) == -1)
	{
		perror("fd openfail");
		exit(0);
	}
	while(flock(lock, LOCK_EX | LOCK_NB) != 0)
	{
		lock_count++;
		sleep(1);
		if(lock_count > 3)
		{
			read(lock, &lock_master_pid, 5);
			sprintf(command, "kill -9 %s", lock_master_pid);
			system(command);
		}
		if(lock_count > 10)
		{
			system("killall html.cgi");
		}
	}

	sprintf(lock_master_pid, "%d\n", getpid());
	lseek(lock, 0, SEEK_SET);
	write(lock, &lock_master_pid, strlen(lock_master_pid));

	if(!getenv("REQUEST_METHOD")) goto LOGIN;

	if(!strcmp(getenv("REQUEST_METHOD"), "GET"))
	{
		qs = getenv("QUERY_STRING");
		if(!qs) goto LOGIN;
		qs = strdup(qs);

		for(i=0; qs[0]!='\0'; i++)
		{
			getword(get[i].value, qs, '&');	
			plustospace(get[i].value);		
			unescape_url(get[i].value);		
			getword(get[i].name, get[i].value, '=');
		}
		free(qs);

		if(strcmp(get[0].name, "function") == 0)
		{
			if(strcmp(get[0].value, "list") == 0) 
			{
				page_list();
			}
			else if(strcmp(get[0].value, "logout") == 0) 
			{
				page_logout();
			}
			else if(strncmp(get[0].value, "tree_", 5) == 0) 
			{
				pages_tree();
			}
			else if(strncmp(get[0].value, "status", 6) == 0)
			{
				pages_status();
			}
			else if(strncmp(get[0].value, "rfconfig", 8) == 0)
			{
				pages_rf_config();
			}
			else if(strncmp(get[0].value, "alarm_config", 12) == 0)
			{
				pages_alarm_config();
			}
			else if(strncmp(get[0].value, "fake_alarm_config", 17) == 0)
			{
				pages_fake_alarm_config();
			}
			else if(strcmp(get[0].value, "communications") == 0)
			{
				page_communication();
			}
			else if(strcmp(get[0].value, "usermanagement") == 0)
			{
				page_user_management();
			}
			else if(strcmp(get[0].value, "logs") == 0)
			{
				page_logs();
			}
			else if(strncmp(get[0].value, "alarm_log", 9) == 0)
			{
				pages_alarm_log();
			}
#if 0	/* sprint */
			else if(strncmp(get[0].value, "logs_osc", 8) == 0)
			{
				pages_osc_log();
			}
#endif
			else if(strcmp(get[0].value, "troubleshooting") == 0)
			{
				page_troubleshooting();
			}
			else if(strcmp(get[0].value, "system_download") == 0)
			{
				page_system_download();
			}
       		else if(strncmp(get[0].value, "reset", 5) == 0)
       		{
				pages_reset();
       		}
			else if(strncmp(get[0].value, "factory_default", 15) == 0)
			{
				pages_factory_default();
			}
			else if(strncmp(get[0].value, "default_", 8) == 0)
			{
				page_default();
			}
#if 0	/* sprint */
			else if(strcmp(get[0].value, "modem_activate") == 0)
			{
				page_modem_activate();
			}
#endif
/*
			else if(strcmp(get[0].value, "autogainsetting") == 0)
			{
				autogainsetting(1);
			}
*/
			else if(strncmp(get[0].value, "transfer_except", 15) == 0)
			{
				pages_transfer();
       		}
       		else if(strcmp(get[0].value, "upload_waiting") == 0)
      			{
				page_upload_waiting();
			}
       		else if(strcmp(get[0].value, "summary_page") == 0)
      			{
				page_summary_page();
			}
       		else if(strncmp(get[0].value, "wizard", 6) == 0)
      			{
				pages_wizard();
			}
//===============================================================================
//===============================================================================
// Tracking Issue #129 Setup Wizard
			/*else if(strcmp(get[0].value, "setup_wizard_step1") == 0)
			{
				setup_wizard_step1();
			}*/
			else if(strcmp(get[0].value, "setup_wizard_step4") == 0)
			{
				setup_wizard_step4(1);
			}		
			else if(strncmp(get[0].value, "setup_wizard_step4_", 19) == 0) 
			{
				pages_setup_wizard_step4();
			}			
			else if(strncmp(get[0].value, "setup_wizard_rfconfig", 21) == 0)
			{
				pages_setup_wizard_rf_config();
			}
//===============================================================================
//===============================================================================
		}
		else
		{
			login();
		}
	}

	else if(!strcmp(getenv("REQUEST_METHOD"), "POST"))
	{
		char* value;
		cl = atoi(getenv("CONTENT_LENGTH"));

		if(!(value = getenv("CONTENT_TYPE")))
		{
			exit(-1);
		}
		else if(!strncmp(value, "multipart/form-data;", strlen("multipart/form-data;")))
		{
#if 0
			if(4*1024* 1024 < cl)
				exit(-1);
#else
			if((8*1024)*1000 < cl)
				exit(-1);
#endif
			upload();
			sync();
			return 0;
		}
		else if(!strncmp(value, "application/x-www-form-urlencoded", strlen("application/x-www-form-urlencoded")))
		{
			for(i=0; (0 < cl) && (!feof(stdin)); i++)
	         	{
		            	post[i].value = fmakeword(stdin,'&',&cl);
		            	plustospace(post[i].value);
		            	unescape_url(post[i].value);
		            	post[i].name = makeword(post[i].value,'=');
						post_size++;
	        	}
		}

		if(strcmp(post[0].name, "login_txt_username") == 0)
		{
			button_login_login(post[0].value, post[1].value);
		}
		else if(strcmp(post[0].name, "check_user_register")==0)
		{
			if(strcmp(post[1].value, "1")==0)
			{
				button_delete_usermanagement(post[3].value);
			}
			else if(strcmp(post[0].value, "1")==0)
			{
				int n=0;
				n=button_register_usermanagement(post[2].value,post[3].value,post[4].value,post[5].value);
				if(n==0)
					usermanagement();
				else
					message_dialog("Write Error: Checking ID, PW, PW Confirm");
			}
		}
#if 0	/* sprint */
		else if(strcmp(post[0].name, "txt_activate_wavecom")==0)
		{
			char buf[128]="";
			int i=0;
			memset(buf, 0, sizeof(buf));
			for(i=1; i<5; i++)
			{
				strcat(buf,post[i].value);
				strcat(buf,"=");
			}
			wireless_activation_wave(buf);
		}
		else if(strcmp(post[0].name, "txt_activate_telit")==0)
		{
			char buf[128]="";
			int i=0;
			memset(buf, 0, sizeof(buf));
			for(i=1; i<6; i++)
			{
				strcat(buf,post[i].value);
				strcat(buf,"=");
			}
			wireless_activation_tel(buf);
		}
#endif
		else if(strcmp(post[0].name, "flag_amp_id")==0)
		{
			char buf[4096]="";
			int i=0;
			memset(buf, 0, sizeof(buf));

			for(i=0; i<56; i++)
			{
#if 0
{
	char tmp[256];
	sprintf(tmp, "[%d] %s=%s", i, post[i].name, post[i].value);
	debuglog(tmp);
}
#endif
				if(post[i].value)
					strcat(buf,post[i].value);
				else
					strcat(buf, "0");
				strcat(buf,"=");
			}
			button_apply_rfconfiguration(buf);
		}
		else if(strcmp(post[0].name, "check_combo_lan_dhcp_client_onoff")==0)
		{
			//char buf[4096]="";
			//int i=0;
			//memset(buf, 0, sizeof(buf));
	
			//for(i=0; i<60; i++)
			//{
				//if(post[i].value)
					//strcat(buf,post[i].value);
				//else
					//strcat(buf, "0");
				//strcat(buf,"=");
			//}
			waiting_page(10, "Communication configuration", "system restarting...", NULL, PAGE_logout);
			sync();
			//button_apply_communications(buf);
			button_apply_communications(NULL);
		}
#if 0	/* sprint */
		else if(strcmp(post[0].name, "page_id_modem_activation")==0)
		{
			waiting_page(65, "Modem activation", "activating", NULL, PAGE_logout);
			sync();
			button_apply_modem();
		}
#endif
		else if(strcmp(post[0].name, "check_amp_id_alarm")==0)
		{
			char buf[4096]="";
			int i=0;
			memset(buf, 0, sizeof(buf));

			for(i=0; i<59; i++)
			{
#if 0
{
	char tmp[256];
	sprintf(tmp, "[%d] %s=%s", i, post[i].name, post[i].value);
	debuglog(tmp);
}
#endif
				if(post[i].value)
					strcat(buf,post[i].value);
				else
					strcat(buf, "0");
				strcat(buf,"=");
			}
			sync();
			button_apply_alarmconfiguration(buf);
		}
		else if(strcmp(post[0].name, "check_amp_id_fake")==0)
		{
			char buf[4096]="";
			int i=0;
			memset(buf, 0, sizeof(buf));

			for(i=0; i<40; i++)
			{
#if 0
{
	char tmp[256];
	sprintf(tmp, "[%d] %s=%s", i, post[i].name, post[i].value);
	debuglog(tmp);
}
#endif
				if(post[i].value)
					strcat(buf,post[i].value);
				else
					strcat(buf, "0");
				strcat(buf,"=");
			}
			sync();
			button_apply_fake_alarmconfiguration(buf);
		}
/*
		else if(strcmp(post[0].name, "combo_ups_onoff")==0)
		{
			char buf[128]="";
			int i=0;
			memset(buf, 0, sizeof(buf));

			for(i=0; i<3; i++)
			{
				strcat(buf,post[i].value);
				strcat(buf,"=");
			}
			ups_configuration(buf);
		}
*/

//===============================================================================
//===============================================================================
// Tracking Issue #129 Setup Wizard		
		/*else if(strcmp(post[0].name, "wizard_repeater_location_company")==0) // Setup Wizard Step 1 Apply
		{
			char buf[4096]="";
			int i=0;
			memset(buf, 0, sizeof(buf));
			for(i=0; i < 80; i++)
			{
				if(post[i].value)
				{
					strcat(buf,post[i].value);
					strcat(buf,"=");
				}
			}
			setup_wizard_step1_bt_apply_click(post);
		}		
		else if(strcmp(post[0].name, "setup_wizard_step1_skip")==0) // Setup Wizard Step 1 Skip
		{		
			setup_wizard_step2();
		}		
		else if(strcmp(post[0].name, "wizard_installer_contact_info_company")==0) // Setup Wizard Step 2 Apply
		{
			char buf[4096]="";
			int i=0;
			memset(buf, 0, sizeof(buf));
			for(i=0; i < 80; i++)
			{
				if(post[i].value)
				{
					strcat(buf,post[i].value);
					strcat(buf,"=");
				}
			}
			setup_wizard_step2_bt_apply_click(post);
		}		
		else if(strcmp(post[0].name, "setup_wizard_step2_skip")==0) // Setup Wizard Step 2 Skip
		{	
			setup_wizard_step3();
		}*/
		else if(strcmp(post[0].name, "flag_setup_wizard_rf_configuration")==0)
		{
			char buf[4096]="";
			int i=0;
			memset(buf, 0, sizeof(buf));
			// flag_setup_wizard_rf_configuration, combo_antenna_setting_control :: +2 
			// flag_setup_wizard_rf_configuration(1) + :: 45+9+14
			//for(i=0; i<45+9+14; i++)
			for(i=0; i<1+45+9+14; i++)
			{
				if(post[i].value)
					strcat(buf,post[i].value);
				else
					strcat(buf, "0");
				strcat(buf,"=");
			}
			
			// Setup Wizard :: Antenna Setting Control	
			int ant_set_ctrl = 0;
			for(i=0; i<500; i++)
			{
				if(strcmp(post[i].name, "combo_antenna_setting_control")==0)
				{
					ant_set_ctrl = atoi(post[i].value);
					break;
				}
			}
			
			button_apply_setup_wizard_rfconfiguration(buf, ant_set_ctrl);
		}
//===============================================================================
//===============================================================================

/*
	download time
	total	5'56"
	snmp: 	45"
	amp4:	30"	
	dfm4:	50"
	amp3:	24"
	dfm3:	52"
	amp2:	26"
	dfm2:	50"
	amp1:	28"
	dfm1:	52"
 */
//		else if(strcmp(post[0].name, "txt_filename") == 0)
		else if(strcmp(post[0].name, "upgradeType") == 0)
		{
			char upgradeType=0, slotNum=0, serviceType=0;

			if(post[0].value) upgradeType = atoi(post[0].value);
			if(post[1].value) slotNum = atoi(post[1].value);
			if(post[2].value) serviceType = atoi(post[2].value);
#if 0
			{
				char tmp[256];
				sprintf(tmp, "uType=%d, sNum=%d, sType=%d", upgradeType, slotNum, serviceType);
				debuglog(tmp);
			}
#endif
			//was 65
			if (upgradeType == 0) {		// software upgrade
				waiting_page(90, "System Upgrading", "System Upgrading", "Do not turn off during upgrade", PAGE_logout);
				sync();
				upgrade();
			} else {					// fpga upgrade
				//waiting_page_fpga_upgrade(90, "DFM Filter Upgrading", "DFM Filter Upgrading", "Do not turn off during upgrade", PAGE_logout);
				waiting_page_fpga_upgrade("DFM Filter Upgrading", "DFM Filter Upgrading", "Do not turn off during upgrade");
				sync();
				upgrade_fpga(slotNum, serviceType);
			}
		}
		else if(strcmp(post[0].name, "txt_filename_transfer") == 0)
		{
			waiting_page(10, "Configuration transfer", "Configuration transfer", "Do not turn off during configuration", PAGE_logout);
			sync();
			upgrade();
		}
		else if(strcmp(post[0].name, "alarm_clear") == 0)
		{
			index=authentication();
			if((index==101)||(index==201))
			{
				send_command(PROTOCOL_DEFAULT_ALARM_LOG, TRUE);
				alarm_log(0);
				logs_write(LOG_ALARM_LOGS_CLEAR_SET);
			}
			else
				message_dialog(message);
		}
		else if(strcmp(post[0].name, "log_clear") == 0)
		{
			index=authentication();
			if((index==101)||(index==201))
			{
				send_command(PROTOCOL_DEFAULT_LOG, TRUE);
				logs();
				logs_write(LOG_LOGS_CLEAR_SET);
			}
			else
				message_dialog(message);
		}
#if 0	/* sprint */
		else if(strcmp(post[0].name, "osc_log_clear") == 0)
		{
			index=authentication();
			if((index==101)||(index==201))
			{
				send_command(PROTOCOL_DEFAULT_OSC_LOG, TRUE);
				logs_osc(0);
				logs_write(LOG_OSC_LOGS_CLEAR_CHECK);
			}
			else
				message_dialog(message);
		}
#endif
		else if(strcmp(post[0].name, "txt_engrno")==0)
		{
			char buf[1024]="";
			int i=0;
			memset(buf, 0, sizeof(buf));
			for(i=0; i<2; i++)	
			{
				if(post[i].value)
					strcat(buf,post[i].value);
				else
					strcat(buf, "0");
				strcat(buf,"=");
			}
			wizard1_bt_apply_click(buf);
		}
		else if(strcmp(post[0].name, "check_band")==0)
		{
			char buf[1024]="";
			int i=0;

			memset(buf, 0, sizeof(buf));
			for(i=0; i<3+1; i++)	
			{
				if(post[i].value)
					strcat(buf,post[i].value);
				else
					strcat(buf, "0");
				strcat(buf,"=");
			}

			waiting_page(15, "Configuring system ", NULL, NULL, PAGE_status);
			sync();
			wizard2_bt_apply_click(buf);
		}
	}

	else
	{
LOGIN:
		login();
	}

	if(-1 != lock)
	{
		if(flock(lock, LOCK_UN) != 0)
		{
			perror("fd unlock error");
		}
		close(lock);
	}

	return 0;
}

