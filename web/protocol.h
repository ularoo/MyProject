#ifndef __web_protocol_h__
#define __web_protocol_h__

#include <time.h>
#include "../modular_app/typedef.h"
//#include "../modular_app/protocol.h"

#ifndef iDEN800_FUNC_ADD	/* iden800 */
#define iDEN800_FUNC_ADD
#endif

#define DOWNLOAD_PAGE							0x00
#define TRANSFER_UPLOAD_PAGE					0x01
#define WRONG_FILE								0x02

#define TRANSFER_DOWNLOAD_STEP_ONE			0x00
#define TRANSFER_DOWNLOAD_STEP_TWO			0x01

#define PROTOCOL_DEBUG	0xFF                          
#define PROTOCOL_STATUS 						0x00
#define PROTOCOL_CONTROL						0x01
#define PROTOCOL_COMMUNICATIONS_WRITE		0x20
#define PROTOCOL_COMMUNICATIONS_READ		0x21
#define PROTOCOL_USERMANAGEMENT				0x22
#define PROTOCOL_ALARMCONFIGURATION_WRITE	0x25
#define PROTOCOL_ALARMCONFIGURATION_READ	0x26
#define PROTOCOL_FAKE_CONFIGURATION_WRITE	0x46
#define PROTOCOL_FAKE_CONFIGURATION_READ	0x47
#define PROTOCOL_SYSTEMRESET_ALL				0x40
#define PROTOCOL_SYSTEMRESET_AMP1			0x41
#define PROTOCOL_SYSTEMRESET_AMP2			0x42
#define PROTOCOL_SYSTEMRESET_AMP3			0x43
#define PROTOCOL_SYSTEMRESET_AMP4			0x44
#define PROTOCOL_SYSTEMRESET_SNMP			0x45
#define PROTOCOL_DOWNLOAD						0x28
#if 1	/* fpga download*/
#define PROTOCOL_DOWNLOAD_FPGA					0x29
#endif
#define PROTOCOL_DEFAULT_LOG					0x32
#define PROTOCOL_DEFAULT_ALARM_LOG			0x33
#define PROTOCOL_DEFAULT_RF					0x35
#define PROTOCOL_DEFAULT_RF1					0x50
#define PROTOCOL_DEFAULT_RF2					0x51
#define PROTOCOL_DEFAULT_RF3					0x52
#define PROTOCOL_DEFAULT_RF4					0x53
#define PROTOCOL_DEFAULT_ALARM				0x37
#define PROTOCOL_DEFAULT_ALARM1				0x54
#define PROTOCOL_DEFAULT_ALARM2				0x55
#define PROTOCOL_DEFAULT_ALARM3				0x56
#define PROTOCOL_DEFAULT_ALARM4				0x57
#define PROTOCOL_DEFAULT_COMM				0x39
#define PROTOCOL_DEFAULT_USER					0x3A
#define PROTOCOL_TRANSFER_EXCEPT				0x3B
#define PROTOCOL_MODEM_ACTIVATE				0x3C
#define PROTOCOL_TRAPINFO						0x3D
#define PROTOCOL_DEFAULT_OSC_LOG				0x3E
//#define PROTOCOL_SNMP_ACTION             0x3F

#define TRUE										0x01
#define FALSE									0x00

#define LOG_LOGIN_CHECK						0x00
#define LOG_STATUS_CHECK						0x01
#define LOG_RF_CHECK							0x02
#define LOG_RF_SET								0x03
#define LOG_ALARM_CHECK						0x04
#define LOG_ALARM_SET							0x05
#define LOG_COMMUNICATION_CHECK				0x06
#define LOG_COMMUNICATION_SET					0x07
#define LOG_USER_ACCESS						0x08
#define LOG_USER_INSERT						0x09
#define LOG_USER_DELETE						0x0A
#define LOG_LOGS_CHECK							0x0B
#define LOG_TROUBLE_CHECK						0x0C
#define LOG_LOGOUT_CHECK						0x0D
#define LOG_SUBSYSTEM_MAILING_CHECK			0x0E
#define LOG_SYSTEM_DOWNLOAD_CHECK			0x0F
#define LOG_SYSTEMRESET_CHECK					0x10
#define LOG_ALARMCONFIGURATION_CHECK 		0x11
#define LOG_ALARMCONFIGURATION_SET			0x12
#define LOG_SUBSYSTEM_MAILING_SET			0x13
#define LOG_SYSTEM_DOWNLOAD_SET				0x14
#define LOG_ALARM_LOGS_CHECK					0x15
#define LOG_ALARM_LOGS_CLEAR_SET				0x16
#define LOG_LOGS_CLEAR_SET						0x17
#define LOG_MODEM_SET							0x18
#define LOG_OSC_LOGS_CHECK					0x19
#define LOG_OSC_LOGS_CLEAR_CHECK				0x1A

#define SERVICE_ID_UNKNOWN	0
#define SERVICE_ID_LTE700		1
#define SERVICE_ID_CELLULAR	2
#define SERVICE_ID_IDEN			3
#define SERVICE_ID_PCS			4
#define SERVICE_ID_AWS			5
#define SERVICE_ID_WIMAX		6
#define SERVICE_ID_LTE2600		7


#define PAGE_1							0
#define PAGE_2							1
#define PAGE_3							2
#define PAGE_4							3

#define MAX_SNMP_ALARM_DESCR_LEN		36

#if 0	/* sprint */
enum{
//	ALARM_UNKNOWN=0,				//unknown(0),
	ALARM_TAMPER=0,				//tamperDetected(1),
	ALARM_POWER_SUPPLY,			//powerSupplyOutOfRange(2),
	ALARM_COMMUNICATION,			//communicationFailure(3),
	ALARM_MODULE_FAILURE,		//moduleFailure(4),
	ALARM_RESET	,				//reset(5),
	ALARM_MANUAL_SHUTD,			//manualShutdown(6),
//	ALARM_HEARTBEAT,				//heartbeat(7),
	ALARM_OSCILLATION,			//oscillationDetected(8),
	ALARM_UL_COVERAGE,			//uplinkCoveragePortPowerTooHigh(9),
	ALARM_UL_SYNTHESIZER,			//uplinkSynthesizer(10),
	ALARM_UL_HARDWARE,			//uplinkHW(11),
	ALARM_UL_SOFTWARE,			//uplinkSW(12),
	ALARM_OOB_EMISSION,			//outOfBandEmission(13),
	ALARM_POWER_HIGH,			//donorPowerTooHigh(14),
	ALARM_POWER_LOW,				//donorPowerTooLow(15),
	ALARM_LOW_ISOLATION,			//lowIsolation(16),
	ALARM_DL_SYNTHESIZER,			//downlinkSynthesizer(17),
	ALARM_DL_HARDWARE,			//downlinkHW(18),
	ALARM_DL_SOFTWARE,			//downlinkSW(19),
	ALARM_SPURIOUS,				//spuriousEmission(20),
	ALARM_INTERFERER,				//interfererPowerExceeded(21),
	ALARM_VSWR,					//vswr(22),
	ALARM_TEMPERATURE,			//temperatureTooHigh(23)
	ALARM_INDEX_MAX,
};
enum{
	CATEGORY_UNKNOWN=0,			//unknown(0),
	CATEGORY_TAMPER,				//tamperDetected(1),
	CATEGORY_POWER_SUPPLY,		//powerSupplyOutOfRange(2),
	CATEGORY_COMMUNICATION,		//communicationFailure(3),
	CATEGORY_MODULE_FAILURE,		//moduleFailure(4),
	CATEGORY_RESET,				//reset(5),
	CATEGORY_MANUAL_SHUTDOWN,	//manualShutdown(6),
	CATEGORY_HEARTBEAT,			//heartbeat(7),
	CATEGORY_OSCILLATION,			//oscillationDetected(8),
	CATEGORY_UL_COVERAGE,		//uplinkCoveragePortPowerTooHigh(9),
	CATEGORY_UL_SYNTHESIZER,		//uplinkSynthesizer(10),
	CATEGORY_UL_HARDWARE,		//uplinkHW(11),
	CATEGORY_UL_SOFTWARE,		//uplinkSW(12),
	CATEGORY_OOB_EMISSION,		//outOfBandEmission(13),
	CATEGORY_POWER_HIGH,			//donorPowerTooHigh(14),
	CATEGORY_POWER_LOW,			//donorPowerTooLow(15),
	CATEGORY_LOW_ISOLATION,		//lowIsolation(16),
	CATEGORY_DL_SYNTHESIZER,		//downlinkSynthesizer(17),
	CATEGORY_DL_HARDWARE,		//downlinkHW(18),
	CATEGORY_DL_SOFTWARE,		//downlinkSW(19),
	CATEGORY_SPURIOUS,			//spuriousEmission(20),
	CATEGORY_INTERFERER,			//interfererPowerExceeded(21),
	CATEGORY_VSWR,				//vswr(22),
	CATEGORY_TEMPERATURE,		//temperatureTooHigh(23)
	CATEGORY_INDEX_MAX,
};
#else
enum{
	ALARM_TAMPER=0,				//(0)
	ALARM_UNDER_CURRENT,		//(1)
	ALARM_LINK_FAIL,			//(2)
	ALARM_SERVICE_ID_FAIL,		//(3)
	ALARM_RESET_ALARM,			//(4)
	ALARM_OSC_ALARM,			//(5)
	ALARM_OVER_INPUT,			//(6)
	ALARM_UL_PLL_ALARM,			//(7)
	ALARM_DFM_ALARM,			//(8)
	ALARM_UL_OVER_OUTPUT,		//(9)
	ALARM_DL_OVER_INPUT,		//(10)
	ALARM_LOW_RSSI,				//(11)
	ALARM_DL_PLL_ALARM,			//(12)
	ALARM_LOW_OUTPUT,			//(13)
	ALARM_DL_OVER_OUTPUT,		//(14)
	ALARM_INTERFERER_ALARM,		//(15)
	ALARM_VSWR,					//(16)
	ALARM_TEMP,					//(17)
	ALARM_FAN,					//(18)
	ALARM_INDEX_MAX,
};
enum{
	CATEGORY_RSSI=0,			//(0),
	CATEGORY_RF_POWER,			//(1),
	CATEGORY_VSWR,				//(2),
	CATEGORY_OVER_TEMPERATURE,	//(3),
	CATEGORY_UNDER_CURRENT,		//(4),
	CATEGORY_INDEX_MAX,
};
#endif

struct web_alarm_entry
{
	unsigned char status;
	unsigned char fake_status;
	unsigned char active;
	unsigned char category;
	unsigned char severity;
	char 		last_triggered[32];
}__attribute__((packed));
struct web_alarm
{
	unsigned char 	is_mount;
	unsigned char 	report_alarm;
	unsigned char 	fake_mode;
	unsigned char 	alarm_led;
	unsigned char 	shutdown_led;
	unsigned char 	category_led[CATEGORY_INDEX_MAX];
	time_t 			category_triggered[CATEGORY_INDEX_MAX];
	struct web_alarm_entry 	alarm_item[ALARM_INDEX_MAX];
}__attribute__((packed));


struct protocol_web_alarm	
{
	unsigned char 	type;
	unsigned char 	contol_amp_id;
	unsigned char 	sum_alarm_led;
	unsigned char 	sum_shutdown_led;
	unsigned char 	alarm_fan;
	unsigned char 	alarm_voltage;
	unsigned char 	alarm_current;
	unsigned char 	alarm_reset;
	struct web_alarm 	slot[4];
	unsigned short 	crc;
}__attribute__((packed));


struct web_module_status
{
	u8 	svc_info;	//0:blank, 1:LTE700, 2:Cellular, 3:iDEN, 4:PCS, 5:AWS, 6:WiMAX, 7:LTE 2600
	u8 	amp_capacity;
	u8 	serial_no[16];
	u8 	led_input_power;
	u8 	led_output_power;
	u8	ver_amp;
	u8	ver_dfm;
	u8	ver_filter;
	u32 	keep_alive;
	float temperature;
	float temperature_limit;
	u8	aoc_onoff;
	u8 	shutdown_onoff;
	u8 	gain_balance_onoff;
	u8 	agc_onoff;
	float isolation_value;
	float isolation_detect_limit;
	float gain_balance_value;
#if 1	/* sprint */
	s8 	cascade_code[51];
#endif
	u8 	alarm_delay;

	u8 	dl_amp_onoff;
	float dl_output_power;
	float dl_agc_limit;
	float dl_gain;
	float dl_attenuation;
	float dl_rssi;
	float dl_input_low_limit;
	float dl_low_output_gap;
	float dl_band_rssi[3];
	float dl_band_compensation[3];

	u8 	ul_amp_onoff;
	float ul_output;
	float ul_agc_limit;
	float ul_gain;
	float ul_attenuation;

	u8 	pcs_band[6];
	u8 	lte700_band;
	u8 	cellular_band;
	u8 	aws_band[4];
#ifdef iDEN800_FUNC_ADD	/* iden800 */
	u8  iden800_band;
#endif /* iDEN800_FUNC_ADD */
	u8 td_lte_band[3];
}__attribute__((packed));

struct protocol_debug
{
	u8 	type;
	char debug[512];
	unsigned short crc;
};
#if 1	/* fpga download */
struct protocol_fpga_download
{
	u8 	type;
	char slot;
	char service;
	unsigned short crc;
};
#endif

struct protocol_web_status
{
	u8 	type;

	u8 	shutdown_led;
	u8 	alarm_led;
	u8	ver_rcu;
	u8	ver_rcu_minor;
	char serial_no[16];
#if 0	/* sprint */
	char copy_location[64];		//site name
	char copy_contact_info[64];
#else
	char copy_latitude[16];		//site name
	char copy_longitude[16];
#endif

	struct protocol_web_alarm 	alarm;
	struct web_module_status	status[4];

	unsigned short crc;
}__attribute__((packed));


struct protocol_control_flag
{
	unsigned char dl_amp_onoff:1;	
	unsigned char dl_agc_limit:1;	
	unsigned char dl_input_low_limit:1;	
	unsigned char dl_low_output_gap:1;	
	unsigned char dl_attenuation:1;	
	unsigned char dl_band_comp_0:1;	
	unsigned char dl_band_comp_1:1;	
	unsigned char dl_band_comp_2:1;	
	unsigned char ul_amp_onoff:1;	
	unsigned char ul_agc_limit:1;	
	unsigned char ul_attenuation:1;	
	unsigned char ul_gain:1;	
	unsigned char agc_onoff:1;	
	unsigned char gain_balance_onoff:1;	
	unsigned char shutdown_onoff:1;	
	unsigned char aoc_onoff:1;	
	unsigned char temperature_limit:1;	
	unsigned char isolation_detect_limit:1;	
	unsigned char gain_balance_value:1;	
#if 1	/* sprint */
	unsigned char cascade_code:1;	
#endif
	unsigned char alarm_delay:1;	
	unsigned char pcs_band:1;	
	unsigned char lte700_band:1;	
	unsigned char cellular_band:1;	
	unsigned char aws_band:1;	
#ifdef iDEN800_FUNC_ADD /* iden800 */
	unsigned char iden800_band:1;
#endif /* iDEN800_FUNC_ADD */
}__attribute__((packed));


struct protocol_web_control
{
	unsigned char 	type;
	unsigned char 	amp_id;
	struct protocol_control_flag flag;

	unsigned char dl_amp_onoff;
	float dl_agc_limit;
	float	dl_input_low_limit;
	float	dl_low_output_gap;
	float	dl_attenuation;
	float	dl_band_comp[3];
	unsigned char ul_amp_onoff;
	float	ul_agc_limit;
	float	ul_attenuation;
	float	ul_gain;
	unsigned char agc_onoff;
	unsigned char gain_balance_onoff;
	unsigned char shutdown_onoff;
	unsigned char aoc_onoff;
	float	temperature_limit;
	float	isolation_detect_limit;
	float	gain_balance_value;
#if 1	/* sprint */
	char cascade_code[51];
#endif
	unsigned char alarm_delay;

	unsigned char pcs_band[6];
	unsigned char	lte700_band;
	unsigned char	cellular_band;
	unsigned char	aws_band[4];
#ifdef iDEN800_FUNC_ADD /* iden800 */
	unsigned char	iden800_band;
#endif /* iDEN800_FUNC_ADD */

	unsigned short crc;
}__attribute__((packed));


struct protocol_communications_flag
{
	unsigned char lan_dhcp_server_onoff:1;	
	unsigned char lan_dhcp_client_onoff:1;	
	unsigned char lan_ip:1;	
	unsigned char lan_netmask:1;	
	unsigned char lan_gateway:1;	
	unsigned char wan_modem_onoff:1;	
	unsigned char wan_dhcp_client_onoff:1;	
	unsigned char wan_ip:1;	
	unsigned char wan_netmask:1;	
	unsigned char wan_gateway:1;	
	unsigned char snmp_manager_ip:1;	
	unsigned char snmp_read_community:1;	
	unsigned char snmp_write_community:1;	
	unsigned char snmp_trap_community:1;	
	unsigned char snmp_general_port:1;	
	unsigned char snmp_trap_port:1;	
	unsigned char ntp_ip:1;
	unsigned char ntp_period:1;
	unsigned char time_zone:1;
	unsigned char set_year:1;	
	unsigned char set_month:1;	
	unsigned char set_day:1;	
	unsigned char set_hour:1;	
	unsigned char set_minute:1;	
//	unsigned char serial_no:1;	
	unsigned char heartbeat_interval:1;
#if 0	/* sprint */
	unsigned char system_id:1;	
	unsigned char coordinate:1;	
	unsigned char location:1;	
	unsigned char contact_info:1;	
#else
	unsigned char latitude:1;
	unsigned char longitude:1;	
#endif
/* C^2 Rev3 */
	unsigned char snmp_version:1;
	unsigned char engine_id:1;
	unsigned char read_user:1;
	unsigned char write_user:1;
	unsigned char auth:1;
	unsigned char priv:1;	
	unsigned char authpassphrase:1;	
	unsigned char privpassphrase:1;	
	unsigned char trap_engine_id:1;	
	unsigned char trap_user:1;	
	unsigned char trap_auth:1;	
	unsigned char trap_priv:1;	
	unsigned char trap_authpassphrase:1;	
	unsigned char trap_privpassphrase:1;	
	unsigned char configuration_url:1;
	unsigned char firmware_url:1;
}__attribute__((packed));

/* C^2 Rev3 */
#define SNMP_V2C 2
#define SNMP_V3 3
#define SNMP_NONE 0
#define SNMP_DES 1
#define SNMP_AES 2
#define SNMP_MD5 1
#define SNMP_SHA 2
#define WAN_MODEM_OFF 0
#define WAN_MODEM_ON 1

struct protocol_communication
{
	unsigned char type;
	struct protocol_communications_flag flag;

	unsigned char lan_dhcp_server_onoff;
	unsigned char lan_dhcp_client_onoff;
	char lan_ip[32];
	char lan_netmask[32];
	char lan_gateway[32];
	unsigned char wan_modem_onoff;
	unsigned char wan_dhcp_client_onoff;
	char wan_ip[32];
	char wan_netmask[32];
	char wan_gateway[32];
	char snmp_manager_ip[32];
	char snmp_read_community[32];
	char snmp_write_community[32];
	char snmp_trap_community[32];
	int snmp_general_port;
	int snmp_trap_port;
	char ntp_ip[32];
	unsigned char ntp_period;
	unsigned char time_zone;
	int current_year;
	unsigned char current_month;
	unsigned char current_day;
	unsigned char current_hour;
	unsigned char current_minute;
	int set_year;
	unsigned char set_month;
	unsigned char set_day;
	unsigned char set_hour;
	unsigned char set_minute;
//	char serial_no[16];
	int heartbeat_interval;
#if 0	/* sprint */
	char system_id[64];
	char coordinate[64];
	char location[64];
	char contact_info[64];
#else
	char latitude[16];
	char longitude[16];
#endif
/* C^2 Rev3 */
	unsigned char snmp_version;
	//char engine_id[64];
	char read_user[64];
	char write_user[64];
	unsigned char auth;
	unsigned char priv;
	char authpassphrase[64];
	char privpassphrase[64];
	//char trap_engine_id[64];
	char trap_user[64];
	unsigned char trap_auth;
	unsigned char trap_priv;
	char trap_authpassphrase[64];
	char trap_privpassphrase[64];
	char configuration_url[128];
	char firmware_url[128];
/* CRC */
	unsigned short crc;
}__attribute__((packed));


struct protocol_usermanagement
{
	unsigned char Type;
	unsigned char UserAccounts;
}__attribute__((packed));


struct protocol_ups
{
	unsigned char type;
	unsigned char ups_onoff_mode;
	unsigned char ups_lowpower_mode;
	unsigned short crc;
}__attribute__((packed));


struct protocol_modem_activate
{
	unsigned char type;
	char MIN[32];
	char Code[32];
	char activate_mode;
	char apn[32];
	char usr[32];
	char pass[32];
	unsigned short crc;
}__attribute__((packed));

struct protocol_trapinfo
{
	unsigned char type;
	int heartbeat_interval;
	unsigned char mount_and_service[4];
	u32	keep_alive[4];
#if 1	/* sprint */
	char cascade_code[4][51];
#endif
	struct protocol_communication communication;
	struct protocol_web_alarm 	alarm;
	unsigned short crc;
}__attribute__((packed));

/* C^2 Rev3 */
#define SNMP_ACTION_RESERVED 0
#define SNMP_ACTION_QUERY SNMP_ACTION_RESERVED
#define SNMP_ACTION_FW_EXEC 1
#define SNMP_ACTION_FW_DOWNLOADING 2
#define SNMP_ACTION_FW_UPGRADING 3
#define SNMP_ACTION_FW_COMPLETE 4
#define SNMP_ACTION_FW_FAILED 5
#define SNMP_ACTION_CONF_EXEC 6
#define SNMP_ACTION_CONF_PREPARING 7
#define SNMP_ACTION_CONF_COMPLETE 8
#define SNMP_ACTION_CONF_FAILED 9
#define SNMP_ACTION_TRNSFR_EXEC 10
#define SNMP_ACTION_TRNSFR_DOWNLOADING 11
#define SNMP_ACTION_TRNSFR_APPLYING 12
#define SNMP_ACTION_TRNSFR_COMPLETE 13
#define SNMP_ACTION_TRNSFR_FAILED 14
/*
struct protocol_snmp_action
{
	unsigned char type;
	unsigned char action;
	unsigned char progress;
	unsigned short crc;
}__attribute__((packed));
*/
unsigned int protocol_buffer_to_frame(unsigned char* buffer, int buffer_size, unsigned char* frame, int* frame_size);
unsigned int protocol_frame_to_buffer(unsigned char* frame, int frame_size, unsigned char* buffer, int* buffer_size);

#endif
