#ifndef __main_h__
#define __main_h__ __main_h__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <sched.h>
#include <sys/un.h>
#include "protocol.h"

#define AUTO_DHCPD
#define ENABLE_LOG_DEBUG_MESSAGE
//#define MOUNT_TEST

#ifndef iDEN800_FUNC_ADD	/* iden800 */
#define iDEN800_FUNC_ADD
#endif

extern int g_udp_sock;
extern struct sockaddr_in g_serveraddr;
extern int g_client;

enum{
	PAGE_logout=0,
	PAGE_list,
	PAGE_tree_status,
	PAGE_status,
	PAGE_rfconfig,
	PAGE_rfconfig_1,
	PAGE_rfconfig_2,
	PAGE_rfconfig_3,
	PAGE_tree_rfconfig,
	PAGE_alarm_config,
	PAGE_alarm_config_1,
	PAGE_alarm_config_2,
	PAGE_alarm_config_3,
	PAGE_communication,
	PAGE_usermanagement,
	PAGE_logs,
	PAGE_alarmlogs_1,
	PAGE_alarmlogs_2,
	PAGE_alarmlogs_3,
	PAGE_alarmlogs_4,
	PAGE_troubleshooting,
	PAGE_system_download,
	PAGE_reset,
	PAGE_reset_module,
	PAGE_systemreset,
	PAGE_factory_default_select,
	PAGE_factory_default,
	PAGE_default_rf,
	PAGE_default_alarm,
	PAGE_modem_activate,
	PAGE_default_user,
	PAGE_default_communication,
	PAGE_transfer_except,
	PAGE_transfer_except_download,
	PAGE_transfer_except_upload,
	PAGE_upload_waiting,
	PAGE_summary_page,
	PAGE_wizard1,
	PAGE_wizard2,
	PAGE_fake_alarm_1,
	PAGE_fake_alarm_2,
	PAGE_fake_alarm_3,
	PAGE_fake_alarm_4,
};


struct get
{
	char name[128];
	char value[128];
};

struct post
{
	char* name;
	char* value;
};

extern struct post post[500];
extern int post_size;

struct cookie
{
	char name[128];
	char value[128];
};

struct user_buffer
{
	char name[256];
};

//===============================================================================
//===============================================================================
// Tracking Issue #129 Setup Wizard
void getword(char *word, char *line, char stop);
char *find_cgi_value(struct post *postvalue, char *name);

void button_apply_setup_wizard_rfconfiguration(char* post_value, int ant_set_ctrl);
void setup_wizard_rfconfig(int amp_id);
void setup_wizard_step4();
void setup_wizard_step3();
//void setup_wizard_step2_bt_apply_click(struct post *post_value);
//void setup_wizard_step2();
//void setup_wizard_step1_bt_apply_click(struct post *post_value);
//void setup_wizard_step1();
void install_wizard_select();
//===============================================================================
//===============================================================================
void alarm_configuration(int amp_id);
void fake_alarm_configuration(int amp_id);
void communications();
void login();
void rfconfig(int amp_id);
void status(int amp_id);
void tree(int is_config);
void troubleshooting();
void usermanagement();
void logs();
#if 0	/* sprint */
void logs_osc(int alarm_page);
#endif
void alarm_log(int alarm_page);

void list();
void reset();
void waiting_page(unsigned int time, char *title, char *message, char *alarm, char page_id);
//void waiting_page_fpga_upgrade(unsigned int time, char *title, char *message, char *alarm, char page_id);
void waiting_page_fpga_upgrade(char *title, char *message, char *alarm);
void message_dialog(char* message);
void button_login_login(char* value1, char* value2);
void setcookie(char* value1);
int authentication();
void getuser();
void showcookie();
void delcookie();
int button_register_usermanagement(char* user, char* password, char* passconfirm, char* authority);
void button_delete_usermanagement(char* user);
void button_apply_communications(char* post_value);
#if 0	/* sprint */
void button_apply_modem(void);
#endif
void button_apply_alarmconfiguration(char* post_value);
void button_apply_fake_alarmconfiguration(char* post_value);
void button_apply_rfconfiguration(char* post_value);
void logs_write(unsigned char type);
void get_log();
void get_alarm_log(int page);
#if 0	/* sprint */
void get_osc_log(int alarm_page);
#endif
void wizard1_bt_apply_click(char* post_value);
void wizard2_bt_apply_click(char* post_value);
void upload(void);
//int upload(void);
int upgrade();
int upgrade_fpga(char slot, char service);
int debuglog(char *str);
int get_band_open(struct web_module_status* p_status, u8 band_open[3], char band_name[3][32]);

//short crc(unsigned char* start, unsigned char* end);
unsigned short crc(unsigned char* start, unsigned char* end);
void system_download(int flag);

#if 0	/* sprint */
//void wireless_activation(char* post_value);
void wireless_activation_wave(char* post_value);
void wireless_activation_tel(char* post_value);
#endif
void factory_default();
void factory_default_select();
void factory_default_rf();
void factory_default_alarm();
void dummy(int flag);
void transfer_except(int flag);
//void modem_activate();
////////////////////////////////////////////
#if 0	/* sprint */
void modem_activate_wave();
void modem_activate_tel();
void modem_activate_sierra(void);
void modem_activate_err(int flag);
#endif
////////////////////////////////////////////
//void transfer_except_download();
void send_command(int command_protocol, int return_value);
void upload_waiting();
void summary_page();
//void modem_activate();
void reset_module();

void html_menu_head(struct protocol_web_status *p_status, char *page_title);
void html_menu_head_left(struct protocol_web_status *p_status);
void html_menu_head_gate(struct protocol_web_status *p_status, unsigned char center, unsigned char led, char *page_title);
void html_page_head(int redirect_action);

extern size_t getline(char **lineptr, size_t *n, FILE *stream);
void create_ajax_code();
void ups_configuration(char* post_value);

void wizard1();
void wizard2();
void GetAlarmColorString(char* str, unsigned char st);

int get_status(struct protocol_web_status *p_status);
int get_communication(struct protocol_communication *p_communications);
int get_alarmconfig(struct protocol_web_alarm *p_web_alarm);

#endif
