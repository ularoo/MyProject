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

extern const char* service_name[];
extern const char* service_color[];

#if 1	/* sprint */
char *alarm_color_str[] =
{
	"[Green Alarm]",	//Notused
	"[Yellow Alarm]",
	"[Red Alarm]"
};
char *alarm_category_str[] =
{
	"[RSSI]",
	"[RF Power]",
	"[VSWR]",
	"[Over Temperature]",
	"[Under Currnet]"
};
char *alarm_name_str[] =
{
	"Tamper Detected Alarm",	//ALARM_TAMPER				(0)
	"Under Current Alarm",		//ALARM_UNDER_CURRENT		(1)
	"Link Fail Alarm",			//ALARM_LINK_FAIL			(2)
	"Service ID Fail Alarm",	//ALARM_SERVICE_ID_FAIL		(3)
	"Reset Alarm",				//ALARM_RESET_ALARM			(4)
	"OSC Alarm",				//ALARM_OSC_ALARM			(5)
	"Over Input Alarm",			//ALARM_OVER_INPUT			(6)
	"UL PLL Alarm",				//ALARM_UL_PLL_ALARM		(7)
	"DFM Alarm",				//ALARM_DFM_ALARM			(8)
	"UL Over Output Alarm",		//ALARM_UL_OVER_OUTPUT		(9)
	"DL Over Input Alarm",		//ALARM_DL_OVER_INPUT		(10)
	"Low RSSI Alarm",			//ALARM_LOW_RSSI			(11)
	"DL PLL Alarm",				//ALARM_DL_PLL_ALARM		(12)
	"Low Output Alarm",			//ALARM_LOW_OUTPUT			(13) 
	"DL Over Output Alarm",		//ALARM_DL_OVER_OUTPUT		(14)
	"Interferer Alarm",			//ALARM_INTERFERER_ALARM	(15)
	"VSWR Alarm",				//ALARM_VSWR				(16)
	"TEMP Alarm",				//ALARM_TEMP				(17)
	"FAN Alarm"					//ALARM_FAN					(18)
};
void print_current_alarm_msg(int amp_id, struct protocol_web_alarm *p_web_alarm)
{
	int i, status;

	for(i=0;i<ALARM_INDEX_MAX;i++) {
		switch(p_web_alarm->slot[amp_id].alarm_item[i].category)
		{
			case CATEGORY_RSSI:
			case CATEGORY_RF_POWER:
			case CATEGORY_VSWR:
			case CATEGORY_OVER_TEMPERATURE:
			case CATEGORY_UNDER_CURRENT:
				status = p_web_alarm->slot[amp_id].alarm_item[i].status;
				if ( (status==1) || (status==2) ) {
					printf("%s : %s - %s \n", alarm_color_str[status], alarm_category_str[p_web_alarm->slot[amp_id].alarm_item[i].category], alarm_name_str[i]);
				}
				/* test print
				else if (status==0) {
					printf("%s : %s - %s \n", alarm_color_str[status], alarm_category_str[p_web_alarm->slot[amp_id].alarm_item[i].category], alarm_name_str[i]);
				}
				*/
				break;
		}
	}
}
#endif

int get_nth_bit(u8 ary[], int n)
{
	int ibyte, ibit;
	ibyte=n/8;
	ibit = n%8;

	if(ary[ibyte]&(1<<ibit)) return 1;
	return 0;
}

void get_aws_band_name(char band_name[], u8 idx)
{
	switch(idx)
	{
		case 0: sprintf(band_name, "A1"); break;
		case 2: sprintf(band_name, "A2"); break;
		case 4: sprintf(band_name, "B1"); break;
		case 6: sprintf(band_name, "B2"); break;
		case 8: sprintf(band_name, "C"); break;
		case 10: sprintf(band_name, "D"); break;
		case 12: sprintf(band_name, "E"); break;
		case 14: sprintf(band_name, "F1"); break;
		case 16: sprintf(band_name, "F2"); break;
		default: sprintf(band_name, ""); break;
	}
}

void get_aws_band_open(struct web_module_status* p_status, u8 band_open[3], char band_name[3][32])
{
#if 0
	int i;
	u8 bit_prev=0, bit_curr, band_cnt=0, aws_band[4];
	band_open[0]=band_open[1]=band_open[2]=0;

	for(i=0;i<4;i++) aws_band[i]=p_status->aws_band[3-i];
	for(i=0;i<17;i++)
	{
		bit_curr=get_nth_bit(aws_band, i);
		if(bit_prev==0 && bit_curr==1) band_cnt++;
		bit_prev=bit_curr;
	}
	if(band_cnt>=3) band_open[2]=1;
	if(band_cnt>=2) band_open[1]=1;
	if(band_cnt>=1) band_open[0]=1;
#else
	int i;
	u8 bit_prev=0, bit_curr, band_from_cnt=0, band_to_cnt=0, aws_band[4];
	u8 band_from_idx[3], band_to_idx[3];
	char tmp_str[32];

	for(i=0;i<3;i++)
	{
		band_open[i]=0;
		band_from_idx[i]=0;
		band_to_idx[i]=0;
	}

	for(i=0;i<4;i++) //swap dspone style to gsi protocol
		aws_band[i]=p_status->aws_band[3-i];

	for(i=0;i<18;i++)//add last dummy 
	{
		bit_curr=get_nth_bit(aws_band, i);
		if(i==17) bit_curr=0;
		
		if(bit_prev==0 && bit_curr==1) 
		{
			band_from_idx[band_from_cnt]=i;
			band_open[band_from_cnt]=1;
			band_from_cnt++;
		}
		else if(bit_prev==1 && bit_curr==0)
		{
			band_to_idx[band_to_cnt]=i-1;
			band_to_cnt++;
		}
		bit_prev=bit_curr;
	}

	for(i=0;i<3;i++)
	{
		if(band_open[i])
		{
			get_aws_band_name(band_name[i], band_from_idx[i]);
			if(band_from_idx[i]!=band_to_idx[i])
			{
				tmp_str[0]='-';
				get_aws_band_name(&tmp_str[1], band_to_idx[i]);
				strcat(band_name[i], tmp_str);
			}
		}
	}
#endif
}

static const char* td_lte_band_name[] =
	{
		"A1,A2,A3,B1(2513)",
		"B2,B3,C1,C2(2535)",
		"C3,D1,D2,D3(2557)",
		"GB,A4,B4,C4,D4(2580.5)",
		"D4,G4,F4,D4,GB(2603.5)",
		"E1,E2,E3,F1(2635)",
		"F2,F3,H1,H2(2657)",
		"H3,G1,G2,G3(2679)"
	};
enum { td_lte_band_name_size = sizeof(td_lte_band_name[0])/ sizeof(td_lte_band_name) };

void get_td_lte_band_open(struct web_module_status* p_status, u8 band_open[3], char band_name[3][32])
{
	int i;
	for(i = 0; i < 3; i++)
	{
		if((0 <= p_status->td_lte_band[i]) 
		&& (p_status->td_lte_band[i] < td_lte_band_name_size))
		{
			band_open[i] = 1;
			strcpy(band_name[i], td_lte_band_name[i]);
		}
		else
		{
			band_open[i] = 0;
			band_name[i][0] = '\0';
		}
	}
}

void get_pcs_band_name(char band_name[], u8 idx)
{
	switch(idx)
	{
	//A1 // 25 50 75 100 
		case 0: 
		case 1: 
		case 2:
		case 3:	sprintf(band_name, "A1.%d", 25*(idx+1)); break;
	//A2 // 125 150 175 200 
		case 4: 
		case 5: 
		case 6:
		case 7:	sprintf(band_name, "A2.%d", 25*(idx+1)); break;
	//A3 // 225 250 275 300 
		case 8: 
		case 9: 
		case 10:
		case 11:	sprintf(band_name, "A3.%d", 25*(idx+1)); break;
	//D // 325 350 375 400 
		case 12: 
		case 13: 
		case 14:
		case 15:	sprintf(band_name, "D.%d", 25*(idx+1)); break;
	//B1 // 425 450 475 500 
		case 16: 
		case 17: 
		case 18:
		case 19:	sprintf(band_name, "B1.%d", 25*(idx+1)); break;
	//B2 // 525 550 575 600 
		case 20: 
		case 21: 
		case 22:
		case 23:	sprintf(band_name, "B2.%d", 25*(idx+1)); break;
	//B3 // 625 650 675 700 
		case 24: 
		case 25: 
		case 26:
		case 27:	sprintf(band_name, "B3.%d", 25*(idx+1)); break;
	//E // 725 740 775 800 
		case 28: 
		case 29: 
		case 30:
		case 31:	sprintf(band_name, "E.%d", 25*(idx+1)); break;
	//F // 825 850 875 900 
		case 32: 
		case 33: 
		case 34:
		case 35:	sprintf(band_name, "F.%d", 25*(idx+1)); break;
	//C3 // 925 950 975 1000 
		case 36: 
		case 37: 
		case 38:
		case 39:	sprintf(band_name, "C3.%d", 25*(idx+1)); break;
	//C4 // 1025 1050 1075 1100 
		case 40: 
		case 41: 
		case 42:
		case 43:	sprintf(band_name, "C4.%d", 25*(idx+1)); break;
	//C5 // 1125 1150 1175 1200 
		case 44: 
		case 45: 
		case 46:
		case 47:	sprintf(band_name, "C5.%d", 25*(idx+1)); break;
	//G // 1225 1250 1275 
		case 48: 
		case 49: 
#if 0	/* sprint */
		case 50:
		case 51:	sprintf(band_name, "G.%d", 25*(idx+1)); break;
#else
		case 50:	sprintf(band_name, "G.%d", 25*(idx+1)); break;
	//LTE // 1250
		//case 51:	sprintf(band_name, "LTE.%d", 25*(idx+1)); break;
		case 51:	sprintf(band_name, "LTE.1250"); break;
#endif
	//unknown
		default: sprintf(band_name, ""); break;
	}
}

void get_pcs_band_open(struct web_module_status* p_status, u8 band_open[3], char band_name[3][32])
{
	int i;
	u8 band_from[3], band_cnt[3], band_to[3];
	char tmp_str[32];

	for(i=0;i<3;i++)
	{
		band_cnt[i]=p_status->pcs_band[i*2+1];
		if(band_cnt[i]!=0) 
		{
			band_open[i]=1;
			band_from[i]=p_status->pcs_band[i*2];
			band_to[i]=band_from[i]+band_cnt[i]-1;

			get_pcs_band_name(band_name[i], band_from[i]);
			if(band_cnt[i]>1)
			{
				tmp_str[0]='~';
				get_pcs_band_name(&tmp_str[1], band_to[i]);
				strcat(band_name[i], tmp_str);
			}
		}
	}
}

void get_lte_band_open(struct web_module_status* p_status, u8 band_open[3], char band_name[3][32])
{
	switch(p_status->lte700_band)
	{
		case 0x01: 
			band_open[0]=1; 
			sprintf(band_name[0], "LowerA"); 
			break;
		case 0x02: 
			band_open[0]=1; 
			sprintf(band_name[0], "LowerB"); 
			break;
		case 0x03: 
			band_open[0]=1; 
			sprintf(band_name[0], "LowerA~LowerB");
			break;
		case 0x04: 
			band_open[0]=1; 
			sprintf(band_name[0], "LowerC");
			break;
		case 0x05: 
			band_open[0]=1; 
			band_open[1]=1; 
			sprintf(band_name[0], "LowerA");
			sprintf(band_name[1], "LowerC");
			break;
		case 0x06: 
			band_open[0]=1;
			sprintf(band_name[0], "LowerB~LowerC");
			break;
		case 0x07: 
			band_open[0]=1;
			sprintf(band_name[0], "LowerA~LowerC");
			break;
		case 0x08: 
			band_open[0]=1;
			sprintf(band_name[0], "UpperC");
			break;
		case 0x09: 
			band_open[0]=1; 
			band_open[1]=1;
			sprintf(band_name[0], "LowerA");
			sprintf(band_name[1], "UpperC");
			break;
		case 0x0a: 
			band_open[0]=1; 
			band_open[1]=1;
			sprintf(band_name[0], "LowerB");
			sprintf(band_name[1], "UpperC");
			break;
		case 0x0b: 
			band_open[0]=1; 
			band_open[1]=1;
			sprintf(band_name[0], "LowerA~LowerB");
			sprintf(band_name[1], "UpperC");
			break;
		case 0x0c: 
			band_open[0]=1; 
			band_open[1]=1;
			sprintf(band_name[0], "LowerC");
			sprintf(band_name[1], "UpperC");
			break;
		case 0x0d: 
			band_open[0]=1; 
			band_open[1]=1; 
			band_open[2]=1; 	
			sprintf(band_name[0], "LowerA");
			sprintf(band_name[1], "LowerC");
			sprintf(band_name[2], "UpperC");
			break;
		case 0x0e: 
			band_open[0]=1; 
			band_open[1]=1;
			sprintf(band_name[0], "LowerB~LowerC");
			sprintf(band_name[1], "UpperC");
			break;
		case 0x0f: 
			band_open[0]=1; 
			band_open[1]=1;
			sprintf(band_name[0], "LowerA~LowerC");
			sprintf(band_name[1], "UpperC");
			break;
		default:	
			break;
	}
}


void get_cell_band_open(struct web_module_status* p_status, u8 band_open[3], char band_name[3][32])
{
	switch(p_status->cellular_band)
	{
		case 0x01: 
			band_open[0]=1;
			sprintf(band_name[0], "A1");
			break;
		case 0x02: 
			band_open[0]=1;
			sprintf(band_name[0], "B1");
			break;
		case 0x03: 
			band_open[0]=1;
			sprintf(band_name[0], "A1~B1");
			break;
		case 0x04: 
			band_open[0]=1;
			sprintf(band_name[0], "A2");
			break;
		case 0x05: 
			band_open[0]=1; 
			band_open[1]=1;
			sprintf(band_name[0], "A1");
			sprintf(band_name[1], "A2");
			break;
		case 0x06: 
			band_open[0]=1;
			sprintf(band_name[0], "B1~A2");
			break;
		case 0x07: 
			band_open[0]=1;
			sprintf(band_name[0], "A1~A2");
			break;
		case 0x08: 
			band_open[0]=1;
			sprintf(band_name[0], "B2");
			break;
		case 0x09: 
			band_open[0]=1; 
			band_open[1]=1;
			sprintf(band_name[0], "A1");
			sprintf(band_name[1], "B2");
			break;
		case 0x0a: 
			band_open[0]=1; 
			band_open[1]=1;
			sprintf(band_name[0], "B1");
			sprintf(band_name[1], "B2");
			break;
		case 0x0b: 
			band_open[0]=1; 
			band_open[1]=1;
			sprintf(band_name[0], "A1~B1");
			sprintf(band_name[1], "B2");
			break;
		case 0x0c: 
			band_open[0]=1;
			sprintf(band_name[0], "A2~B2");
			break;
		case 0x0d: 
			band_open[0]=1; 
			band_open[1]=1;
			sprintf(band_name[0], "A1");
			sprintf(band_name[1], "A2~B2");
			break;
		case 0x0e: 
			band_open[0]=1;
			sprintf(band_name[0], "B1~B2");
			break;
		case 0x0f: 
			band_open[0]=1;
			sprintf(band_name[0], "A1~B2");
			break;
		default:	
			break;
	}
}

#ifdef iDEN800_FUNC_ADD	/* iden800 */
void get_iden_band_open(struct web_module_status* p_status, u8 band_open[3], char band_name[3][32])
{
	switch(p_status->iden800_band)
	{
		case 0x01: 
			band_open[0]=1;
			sprintf(band_name[0], "18M");
			break;
		case 0x02: 
			band_open[0]=1;
			sprintf(band_name[0], "17.8M");
			break;
		case 0x03: 
			band_open[0]=1;
			sprintf(band_name[0], "17.6M");
			break;
		case 0x04: 
			band_open[0]=1;
			sprintf(band_name[0], "15M");
			break;
		case 0x05: 
			band_open[0]=1; 
			sprintf(band_name[0], "14.8M");
			break;
		case 0x06: 
			band_open[0]=1;
			sprintf(band_name[0], "14.6M");
			break;
		case 0x07: 
			band_open[0]=1; 
			sprintf(band_name[0], "7M");
			break;
		case 0x08: 
			band_open[0]=1;
			sprintf(band_name[0], "6.8M");
			break;
		case 0x09: 
			band_open[0]=1;
			sprintf(band_name[0], "6.6M");
			break;
		default:	
			break;
	}
}
#endif /* iDEN800_FUNC_ADD */

int get_band_open(struct web_module_status* p_status, u8 band_open[3], char band_name[3][32])
{
	int i;
	int ret;
	for(i=0;i<3;i++)
	{
		band_open[i]=0;
		sprintf(band_name[i], "none");
	}
	switch(p_status->svc_info)//0:blank, 1:LTE700, 2:Cellular, 3:iDEN, 4:PCS, 5:AWS, 6:WiMAX, 7:LTE 2600
	{
		case 1: // 1:LTE700
			ret=0;
			get_lte_band_open(p_status, band_open, band_name);
			break;
		case 2: // 2:Cellular
			ret=0;
			get_cell_band_open(p_status, band_open, band_name);
			break;
#ifdef iDEN800_FUNC_ADD	/* iden800 */
		case 3: // 3:iDEN
			ret=0;
			get_iden_band_open(p_status, band_open, band_name);
			break;
#endif /* iDEN800_FUNC_ADD */
		case 4: // 4:PCS
			ret=0;
			get_pcs_band_open(p_status, band_open, band_name);
			break;
		case 5: //5:AWS
			ret=0;
			get_aws_band_open(p_status, band_open, band_name);
			break;
		case 6: //6: TD-LTE
			ret=0;
			get_td_lte_band_open(p_status, band_open, band_name);
#if 0
{
	char tmp[256];
	sprintf(tmp, "aws band info: %02X %02X %02X %02X", 
		p_status->aws_band[0], p_status->aws_band[1], p_status->aws_band[2], p_status->aws_band[3]);
	debuglog(tmp);
}
#endif

			break;

		default:
			ret=-1;
			break;
	}

	return ret;
}

#define PCS_BAND_MAX		52
void status(int amp_id)
{
	struct protocol_web_status protocol_status;
	if(get_status(&protocol_status)<0) return;
//	if(amp_id>=4) return;
#if 0
{
	char tmp[256];
	sprintf(tmp, "amp%d-ver: %02X %02X", amp_id, protocol_status.ver_rcu, protocol_status.ver_rcu_minor);
	debuglog(tmp);
}
#endif


	int i, idx;
	char str[64], str_tmp[64];
#if 0	/* sprint */
	char pcs_band_check[51], 	//
#else
	char pcs_band_check[PCS_BAND_MAX], 	//
#endif
		lte700_band_check[4],	//lowA,lowB,lowC,lowD
		cellular_band_check[4],	//A1,A2,B1,B2
#ifdef iDEN800_FUNC_ADD	/* iden800 */
		iden_band_check[9],	//6.6M,6.8M,7M,14.6M,14.8M,15M,17.6M,17.8M,18M (only one selected)
#endif /* iDEN800_FUNC_ADD */
		aws_band_check[17];		//A1,+,A2,+,B1,+,B2,+,C,+,D,+,E,+,F1,+,F2

	memset(pcs_band_check, 0, sizeof(pcs_band_check));
	for(i=0; i<protocol_status.status[amp_id].pcs_band[1]; i++) 
	{
		idx=protocol_status.status[amp_id].pcs_band[0]+i;
		if(idx>=PCS_BAND_MAX) continue;
		pcs_band_check[idx]=1;
	}
	for(i=0; i<protocol_status.status[amp_id].pcs_band[3]; i++) 
	{
		idx=protocol_status.status[amp_id].pcs_band[2]+i;
		if(idx>=PCS_BAND_MAX) continue;
		pcs_band_check[idx]=1;
	}
	for(i=0; i<protocol_status.status[amp_id].pcs_band[5]; i++) 
	{
		idx=protocol_status.status[amp_id].pcs_band[4]+i;
		if(idx>=PCS_BAND_MAX) continue;
		pcs_band_check[idx]=1;
	}

	memset(lte700_band_check, 0, sizeof(lte700_band_check));
	if(protocol_status.status[amp_id].lte700_band & (1<<0)) lte700_band_check[0]=1;
	if(protocol_status.status[amp_id].lte700_band & (1<<1)) lte700_band_check[1]=1;
	if(protocol_status.status[amp_id].lte700_band & (1<<2)) lte700_band_check[2]=1;
	if(protocol_status.status[amp_id].lte700_band & (1<<3)) lte700_band_check[3]=1;

	memset(cellular_band_check, 0, sizeof(cellular_band_check));
	if(protocol_status.status[amp_id].cellular_band & (1<<0)) cellular_band_check[0]=1;
	if(protocol_status.status[amp_id].cellular_band & (1<<1)) cellular_band_check[1]=1;
	if(protocol_status.status[amp_id].cellular_band & (1<<2)) cellular_band_check[2]=1;
	if(protocol_status.status[amp_id].cellular_band & (1<<3)) cellular_band_check[3]=1;

#ifdef iDEN800_FUNC_ADD	/* iden800 */
	memset(iden_band_check, 0, sizeof(iden_band_check));
	if(protocol_status.status[amp_id].iden800_band == 0x01) iden_band_check[0]=1;		//18M
	else if(protocol_status.status[amp_id].iden800_band == 0x02) iden_band_check[1]=1;	//17.8M
	else if(protocol_status.status[amp_id].iden800_band == 0x03) iden_band_check[2]=1;	//17.6M
	else if(protocol_status.status[amp_id].iden800_band == 0x04) iden_band_check[3]=1;	//15M
	else if(protocol_status.status[amp_id].iden800_band == 0x05) iden_band_check[4]=1;	//14.8M
	else if(protocol_status.status[amp_id].iden800_band == 0x06) iden_band_check[5]=1;	//14.6M
	else if(protocol_status.status[amp_id].iden800_band == 0x07) iden_band_check[6]=1;	//7M
	else if(protocol_status.status[amp_id].iden800_band == 0x08) iden_band_check[7]=1;	//6.8M
	else if(protocol_status.status[amp_id].iden800_band == 0x09) iden_band_check[8]=1;	//6.6M
#endif /* iDEN800_FUNC_ADD */

	memset(aws_band_check, 0, sizeof(aws_band_check));
	if(protocol_status.status[amp_id].aws_band[1] & (1<<0)) aws_band_check[16]=1;	//f2
	if(protocol_status.status[amp_id].aws_band[2] & (1<<7)) aws_band_check[15]=1;	//cont
	if(protocol_status.status[amp_id].aws_band[2] & (1<<6)) aws_band_check[14]=1;	//f1
	if(protocol_status.status[amp_id].aws_band[2] & (1<<5)) aws_band_check[13]=1;	//cont
	if(protocol_status.status[amp_id].aws_band[2] & (1<<4)) aws_band_check[12]=1;	//e
	if(protocol_status.status[amp_id].aws_band[2] & (1<<3)) aws_band_check[11]=1;	//cont
	if(protocol_status.status[amp_id].aws_band[2] & (1<<2)) aws_band_check[10]=1;	//d
	if(protocol_status.status[amp_id].aws_band[2] & (1<<1)) aws_band_check[9]=1;	//cont
	if(protocol_status.status[amp_id].aws_band[2] & (1<<0)) aws_band_check[8]=1;	//c
	if(protocol_status.status[amp_id].aws_band[3] & (1<<7)) aws_band_check[7]=1;	//cont
	if(protocol_status.status[amp_id].aws_band[3] & (1<<6)) aws_band_check[6]=1;	//b2
	if(protocol_status.status[amp_id].aws_band[3] & (1<<5)) aws_band_check[5]=1;	//cont
	if(protocol_status.status[amp_id].aws_band[3] & (1<<4)) aws_band_check[4]=1;	//b1
	if(protocol_status.status[amp_id].aws_band[3] & (1<<3)) aws_band_check[3]=1;	//cont
	if(protocol_status.status[amp_id].aws_band[3] & (1<<2)) aws_band_check[2]=1;	//a2
	if(protocol_status.status[amp_id].aws_band[3] & (1<<1)) aws_band_check[1]=1;	//cont
	if(protocol_status.status[amp_id].aws_band[3] & (1<<0)) aws_band_check[0]=1;	//a1

#ifdef MOUNT_TEST
	protocol_status.status[0].svc_info=1;// 1:LTE700
	protocol_status.status[1].svc_info=2;// 2:Cellular
	protocol_status.status[2].svc_info=4;// 4:PCS
	protocol_status.status[3].svc_info=3;// 3:iDEN

	protocol_status.status[0].amp_capacity=24;
	protocol_status.status[1].amp_capacity=24;
	protocol_status.status[2].amp_capacity=33;
	protocol_status.status[3].amp_capacity=30;

	sprintf((char*)protocol_status.status[0].serial_no, "12345");
	sprintf((char*)protocol_status.status[1].serial_no, "23456");
	sprintf((char*)protocol_status.status[2].serial_no, "34567");
	sprintf((char*)protocol_status.status[3].serial_no, "45678");
#endif
#if 0
{
	char tmp[256];
	sprintf(tmp, "amp%d+ver: %02X %02X", amp_id, protocol_status.ver_rcu, protocol_status.ver_rcu_minor);
	debuglog(tmp);
}
#endif

	//==============================================================
	html_page_head(0);  // arg==requiredfrom true(1), false(0)
	printf("<!-- \n");
	create_ajax_code();

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
	printf("\n");

	printf("function reload() {\n");
	printf("	sendRequest(status_load,'','GET','/web_status%d',true,true);\n", amp_id);	// amp_id=0~3
	printf("	time_id = setTimeout(\"reload()\", 60000);\n");
	printf("}\n");
	printf("\n");

	printf("function check_no_click(oj){\n");
	printf("	if(oj.checked==true) oj.checked=false;\n");
	printf("	else oj.checked=true;\n");
	printf("}\n");

	printf("function status_load(oj){\n");
	printf("	var i=0;\n");
	printf("	var yellow_cnt = [0,0,0,0,0];\n");
	printf("	var red_cnt = [0,0,0,0,0];\n");
	printf("	var category_color = [0,0,0,0,0];\n");
	printf("	var arr_alarm_status = new Array();\n");
	printf("	var arr_alarm_category = new Array();\n");
	printf("	var str_alarm_color = [\"[Green Alarm]\", \"[Yellow Alarm]\", \"[Red Alarm]\"];\n");
	printf("	var str_alarm_category = [\"RSSI\", \"RF Power\", \"VSWR\", \"Over Temperature\", \"Under Currnet\"]\n");
	printf("	var str_alarm_name = [\"Tamper Detected Alarm\", \"Under Current Alarm\", \"Link Fail Alarm\", \"Service ID Fail Alarm\", \"Reset Alarm\", \"OSC Alarm\", \"Over Input Alarm\", \"UL PLL Alarm\", \"DFM Alarm\", \"UL Over Output Alarm\", \"DL Over Input Alarm\", \"Low RSSI Alarm\", \"DL PLL Alarm\", \"Low Output Alarm\", \"DL Over Output Alarm\", \"Interferer Alarm\", \"VSWR Alarm\", \"TEMP Alarm\", \"FAN Alarm\"];\n");
	printf("	var alarm_msg_result=\"\";\n");

	printf("	var detect = decodeURIComponent(oj.responseText);\n");
	printf("	var s00=\"alarm_led\";\n");
	printf("	var i00=detect.indexOf(s00)+s00.length+1;\n");
	printf("	var val_alarm_led = detect.substring(i00, i00+1);\n");//i00 to i00; 1 character
	printf("	var s01=\"shutdown_led\";\n");
	printf("	var i01=detect.indexOf(s01)+s01.length+1;\n");
	printf("	var val_shutdown_led = detect.substring(i01, i01+1);\n");//0
/*
	printf("	var s02=\"svc_info\";\n");
	printf("	var i02=detect.indexOf(s02)+s02.length+1;\n");
	printf("	var val_svc_info = detect.substring(i02, i02+1);\n");//0
*/
	printf("	var s03=\"led_input_power\";\n");
	printf("	var i03=detect.indexOf(s03)+s03.length+1;\n");
	printf("	var val_led_input_power = detect.substring(i03, i03+1);\n");//0
	printf("	var s04=\"led_output_power\";\n");
	printf("	var i04=detect.indexOf(s04)+s04.length+1;\n");
	printf("	var val_led_output_power = detect.substring(i04, i04+1);\n");//0
	printf("	var s05=\"dl_amp_onoff\";\n");
	printf("	var i05=detect.indexOf(s05)+s05.length+1;\n");
	printf("	var val_dl_amp_onoff = detect.substring(i05, i05+1);\n");//0
	printf("	var s06=\"dl_output_power\";\n");
	printf("	var i06=detect.indexOf(s06)+s06.length+1;\n");
	printf("	var val_dl_output_power = detect.substring(i06, i06+7);\n");//-100.0
	printf("	var s07=\"dl_agc_limit\";\n");
	printf("	var i07=detect.indexOf(s07)+s07.length+1;\n");
	printf("	var val_dl_agc_limit = detect.substring(i07, i07+7);\n");//-100.0
	printf("	var s08=\"dl_gain\";\n");
	printf("	var i08=detect.indexOf(s08)+s08.length+1;\n");
	printf("	var val_dl_gain = detect.substring(i08, i08+7);\n");//-100.0
	printf("	var s09=\"dl_attenuation\";\n");
	printf("	var i09=detect.indexOf(s09)+s09.length+1;\n");
	printf("	var val_dl_attenuation = detect.substring(i09, i09+7);\n");//-100.0
	printf("	var s10=\"dl_rssi\";\n");
	printf("	var i10=detect.indexOf(s10)+s10.length+1;\n");
	printf("	var val_dl_rssi = detect.substring(i10, i10+7);\n");//-100.0
	printf("	var s11=\"dl_input_low_limit\";\n");
	printf("	var i11=detect.indexOf(s11)+s11.length+1;\n");
	printf("	var val_dl_input_low_limit = detect.substring(i11, i11+7);\n");//-100.0
	printf("	var s12=\"dl_low_output_gap\";\n");
	printf("	var i12=detect.indexOf(s12)+s12.length+1;\n");
	printf("	var val_dl_low_output_gap = detect.substring(i12, i12+7);\n");//-100.0
	printf("	var s13=\"dl_band1_rssi\";\n");
	printf("	var i13=detect.indexOf(s13)+s13.length+1;\n");
	printf("	var val_dl_band1_rssi = detect.substring(i13, i13+7);\n");//-100.0
	printf("	var s14=\"dl_band2_rssi\";\n");
	printf("	var i14=detect.indexOf(s14)+s14.length+1;\n");
	printf("	var val_dl_band2_rssi = detect.substring(i14, i14+7);\n");//-100.0
	printf("	var s15=\"dl_band3_rssi\";\n");
	printf("	var i15=detect.indexOf(s15)+s15.length+1;\n");
	printf("	var val_dl_band3_rssi = detect.substring(i15, i15+7);\n");//-100.0
	printf("	var s16=\"dl_band1_compensation\";\n");
	printf("	var i16=detect.indexOf(s16)+s16.length+1;\n");
	printf("	var val_dl_band1_compensation = detect.substring(i16, i16+7);\n");//-100.0
	printf("	var s17=\"dl_band2_compensation\";\n");
	printf("	var i17=detect.indexOf(s17)+s17.length+1;\n");
	printf("	var val_dl_band2_compensation = detect.substring(i17, i17+7);\n");//-100.0
	printf("	var s18=\"dl_band3_compensation\";\n");
	printf("	var i18=detect.indexOf(s18)+s18.length+1;\n");
	printf("	var val_dl_band3_compensation = detect.substring(i18, i18+7);\n");//-100.0
	printf("	var s19=\"ul_amp_onoff\";\n");
	printf("	var i19=detect.indexOf(s19)+s19.length+1;\n");
	printf("	var val_ul_amp_onoff = detect.substring(i19, i19+1);\n");//0
	printf("	var s20=\"ul_output\";\n");
	printf("	var i20=detect.indexOf(s20)+s20.length+1;\n");
	printf("	var val_ul_output = detect.substring(i20, i20+7);\n");//-100.0
	printf("	var s21=\"ul_agc_limit\";\n");
	printf("	var i21=detect.indexOf(s21)+s21.length+1;\n");
	printf("	var val_ul_agc_limit = detect.substring(i21, i21+7);\n");//-100.0
	printf("	var s22=\"ul_gain\";\n");
	printf("	var i22=detect.indexOf(s22)+s22.length+1;\n");
	printf("	var val_ul_gain = detect.substring(i22, i22+7);\n");//-100.0
	printf("	var s23=\"ul_attenuation\";\n");
	printf("	var i23=detect.indexOf(s23)+s23.length+1;\n");
	printf("	var val_ul_attenuation = detect.substring(i23, i23+7);\n");//-100.0
	printf("	var s24=\"ver_amp\";\n");
	printf("	var i24=detect.indexOf(s24)+s24.length+1;\n");
	printf("	var val_ver_amp = detect.substring(i24, i24+3);\n");//00.00
	printf("	var s25=\"ver_dfm\";\n");
	printf("	var i25=detect.indexOf(s25)+s25.length+1;\n");
	printf("	var val_ver_dfm = detect.substring(i25, i25+3);\n");//00.00
	printf("	var s26=\"temperature\";\n");
	printf("	var i26=detect.indexOf(s26)+s26.length+1;\n");
	printf("	var val_temperature = detect.substring(i26, i26+4);\n");//100
	printf("	var s27=\"temperature_limit\";\n");
	printf("	var i27=detect.indexOf(s27)+s27.length+1;\n");
	printf("	var val_temperature_limit = detect.substring(i27, i27+4);\n");//100
	printf("	var s28=\"aoc_onoff\";\n");
	printf("	var i28=detect.indexOf(s28)+s28.length+1;\n");
	printf("	var val_aoc_onoff = detect.substring(i28, i28+1);\n");//0
	printf("	var s29=\"shutdown_onoff\";\n");
	printf("	var i29=detect.indexOf(s29)+s29.length+1;\n");
	printf("	var val_shutdown_onoff = detect.substring(i29, i29+1);\n");//0
	printf("	var s30=\"gain_balance_onoff\";\n");
	printf("	var i30=detect.indexOf(s30)+s30.length+1;\n");
	printf("	var val_gain_balance_onoff = detect.substring(i30, i30+1);\n");//0
	printf("	var s31=\"agc_onoff\";\n");
	printf("	var i31=detect.indexOf(s31)+s31.length+1;\n");
	printf("	var val_agc_onoff = detect.substring(i31, i31+1);\n");//0
	printf("	var s32=\"isolation_value\";\n");
	printf("	var i32=detect.indexOf(s32)+s32.length+1;\n");
	printf("	var val_isolation_value = detect.substring(i32, i32+5);\n");//100
	printf("	var s33=\"isolation_detect_limit\";\n");
	printf("	var i33=detect.indexOf(s33)+s33.length+1;\n");
	printf("	var val_isolation_detect_limit = detect.substring(i33, i33+4);\n");//100
	printf("	var s34=\"gain_balance_value\";\n");
	printf("	var i34=detect.indexOf(s34)+s34.length+1;\n");
	printf("	var val_gain_balance_value = detect.substring(i34, i34+4);\n");//100
/*
	printf("	var s35=\"ver_rcu\";\n");
	printf("	var i35=detect.indexOf(s35)+s35.length+1;\n");
	printf("	var val_ver_rcu = detect.substring(i35, i35+6);\n");//00.00
	printf("	var s36=\"serial_no\";\n");
	printf("	var i36=detect.indexOf(s36)+s36.length+1;\n");
	printf("	var val_serial_no = detect.substring(i36, i36+17);\n");//16 chars
	printf("	var s37=\"location\";\n");
	printf("	var i37=detect.indexOf(s37)+s37.length+1;\n");
	printf("	var val_location = detect.substring(i37, i37+51);\n");//50 chars
	printf("	var s38=\"contact_info\";\n");
	printf("	var i38=detect.indexOf(s38)+s38.length+1;\n");
	printf("	var val_contact_info = detect.substring(i38, i38+51);\n");//50 chars
*/
	// alarm
	printf("	var s39=\"alarm_delay\";\n");
	printf("	var i39=detect.indexOf(s39)+s39.length+1;\n");
	printf("	var val_alarm_delay = detect.substring(i39, i39+1);\n");//0

	printf("	var s40=\"alarm_tamper\";\n");
	printf("	var i40=detect.indexOf(s40)+s40.length+1;\n");
//	printf("	var val_alarm_tamper = detect.substring(i40, i40+1);\n");//0
	printf("	arr_alarm_status[0] = detect.substring(i40, i40+1);\n");//0
	printf("	var s41=\"category_tamper\";\n");
	printf("	var i41=detect.indexOf(s41)+s41.length+1;\n");
//	printf("	var val_category_tamper = detect.substring(i41, i41+1);\n");//0
	printf("	arr_alarm_category[0] = detect.substring(i41, i41+1);\n");//0

	printf("	var s42=\"alarm_under_current\";\n");
	printf("	var i42=detect.indexOf(s42)+s42.length+1;\n");
//	printf("	var val_alarm_under_current = detect.substring(i42, i42+1);\n");//0
	printf("	arr_alarm_status[1] = detect.substring(i42, i42+1);\n");//0
	printf("	var s43=\"category_under_current\";\n");
	printf("	var i43=detect.indexOf(s43)+s43.length+1;\n");
//	printf("	var val_category_under_current = detect.substring(i43, i43+1);\n");//0
	printf("	arr_alarm_category[1] = detect.substring(i43, i43+1);\n");//0

	printf("	var s44=\"alarm_link_fail\";\n");
	printf("	var i44=detect.indexOf(s44)+s44.length+1;\n");
//	printf("	var val_alarm_link_fail = detect.substring(i44, i44+1);\n");//0
	printf("	arr_alarm_status[2] = detect.substring(i44, i44+1);\n");//0
	printf("	var s45=\"category_link_fail\";\n");
	printf("	var i45=detect.indexOf(s45)+s45.length+1;\n");
//	printf("	var val_category_link_fail = detect.substring(i45, i45+1);\n");//0
	printf("	arr_alarm_category[2] = detect.substring(i45, i45+1);\n");//0

	printf("	var s46=\"alarm_service_id_fail\";\n");
	printf("	var i46=detect.indexOf(s46)+s46.length+1;\n");
//	printf("	var val_alarm_service_id_fail = detect.substring(i46, i46+1);\n");//0
	printf("	arr_alarm_status[3] = detect.substring(i46, i46+1);\n");//0
	printf("	var s47=\"category_service_id_fail\";\n");
	printf("	var i47=detect.indexOf(s47)+s47.length+1;\n");
//	printf("	var val_category_service_id_fail = detect.substring(i47, i47+1);\n");//0
	printf("	arr_alarm_category[3] = detect.substring(i47, i47+1);\n");//0

	printf("	var s48=\"alarm_reset_alarm\";\n");
	printf("	var i48=detect.indexOf(s48)+s48.length+1;\n");
//	printf("	var val_alarm_reset_alarm = detect.substring(i48, i48+1);\n");//0
	printf("	arr_alarm_status[4] = detect.substring(i48, i48+1);\n");//0
	printf("	var s49=\"category_reset_alarm\";\n");
	printf("	var i49=detect.indexOf(s49)+s49.length+1;\n");
//	printf("	var val_category_reset_alarm = detect.substring(i49, i49+1);\n");//0
	printf("	arr_alarm_category[4] = detect.substring(i49, i49+1);\n");//0

	printf("	var s50=\"alarm_osc_alarm\";\n");
	printf("	var i50=detect.indexOf(s50)+s50.length+1;\n");
//	printf("	var val_alarm_osc_alarm = detect.substring(i50, i50+1);\n");//0
	printf("	arr_alarm_status[5] = detect.substring(i50, i50+1);\n");//0
	printf("	var s51=\"category_osc_alarm\";\n");
	printf("	var i51=detect.indexOf(s51)+s51.length+1;\n");
//	printf("	var val_category_osc_alarm = detect.substring(i51, i51+1);\n");//0
	printf("	arr_alarm_category[5] = detect.substring(i51, i51+1);\n");//0

	printf("	var s52=\"alarm_over_input\";\n");
	printf("	var i52=detect.indexOf(s52)+s52.length+1;\n");
//	printf("	var val_alarm_over_input = detect.substring(i52, i52+1);\n");//0
	printf("	arr_alarm_status[6] = detect.substring(i52, i52+1);\n");//0
	printf("	var s53=\"category_over_input\";\n");
	printf("	var i53=detect.indexOf(s53)+s53.length+1;\n");
//	printf("	var val_category_over_input = detect.substring(i53, i53+1);\n");//0
	printf("	arr_alarm_category[6] = detect.substring(i53, i53+1);\n");//0

	printf("	var s54=\"alarm_ul_pll_alarm\";\n");
	printf("	var i54=detect.indexOf(s54)+s54.length+1;\n");
//	printf("	var val_alarm_ul_pll_alarm = detect.substring(i54, i54+1);\n");//0
	printf("	arr_alarm_status[7] = detect.substring(i54, i54+1);\n");//0
	printf("	var s55=\"category_ul_pll_alarm\";\n");
	printf("	var i55=detect.indexOf(s55)+s55.length+1;\n");
//	printf("	var val_category_ul_pll_alarm = detect.substring(i55, i55+1);\n");//0
	printf("	arr_alarm_category[7] = detect.substring(i55, i55+1);\n");//0

	printf("	var s56=\"alarm_dfm_alarm\";\n");
	printf("	var i56=detect.indexOf(s56)+s56.length+1;\n");
//	printf("	var val_alarm_dfm_alarm = detect.substring(i56, i56+1);\n");//0
	printf("	arr_alarm_status[8] = detect.substring(i56, i56+1);\n");//0
	printf("	var s57=\"category_dfm_alarm\";\n");
	printf("	var i57=detect.indexOf(s57)+s57.length+1;\n");
//	printf("	var val_category_dfm_alarm = detect.substring(i57, i57+1);\n");//0
	printf("	arr_alarm_category[8] = detect.substring(i57, i57+1);\n");//0

	printf("	var s58=\"alarm_ul_over_output\";\n");
	printf("	var i58=detect.indexOf(s58)+s58.length+1;\n");
//	printf("	var val_alarm_ul_over_output = detect.substring(i58, i58+1);\n");//0
	printf("	arr_alarm_status[9] = detect.substring(i58, i58+1);\n");//0
	printf("	var s59=\"category_ul_over_output\";\n");
	printf("	var i59=detect.indexOf(s59)+s59.length+1;\n");
//	printf("	var val_category_ul_over_output = detect.substring(i59, i59+1);\n");//0
	printf("	arr_alarm_category[9] = detect.substring(i59, i59+1);\n");//0

	printf("	var s60=\"alarm_dl_over_input\";\n");
	printf("	var i60=detect.indexOf(s60)+s60.length+1;\n");
//	printf("	var val_alarm_dl_over_input = detect.substring(i60, i60+1);\n");//0
	printf("	arr_alarm_status[10] = detect.substring(i60, i60+1);\n");//0
	printf("	var s61=\"category_dl_over_input\";\n");
	printf("	var i61=detect.indexOf(s61)+s61.length+1;\n");
//	printf("	var val_category_dl_over_input = detect.substring(i61, i61+1);\n");//0
	printf("	arr_alarm_category[10] = detect.substring(i61, i61+1);\n");//0

	printf("	var s62=\"alarm_low_rssi\";\n");
	printf("	var i62=detect.indexOf(s62)+s62.length+1;\n");
//	printf("	var val_alarm_low_rssi = detect.substring(i62, i62+1);\n");//0
	printf("	arr_alarm_status[11] = detect.substring(i62, i62+1);\n");//0
	printf("	var s63=\"category_low_rssi\";\n");
	printf("	var i63=detect.indexOf(s63)+s63.length+1;\n");
//	printf("	var val_category_low_rssi = detect.substring(i63, i63+1);\n");//0
	printf("	arr_alarm_category[11] = detect.substring(i63, i63+1);\n");//0

	printf("	var s64=\"alarm_dl_pll_alarm\";\n");
	printf("	var i64=detect.indexOf(s64)+s64.length+1;\n");
//	printf("	var val_alarm_dl_pll_alarm = detect.substring(i64, i64+1);\n");//0
	printf("	arr_alarm_status[12] = detect.substring(i64, i64+1);\n");//0
	printf("	var s65=\"category_dl_pll_alarm\";\n");
	printf("	var i65=detect.indexOf(s65)+s65.length+1;\n");
//	printf("	var val_category_dl_pll_alarm = detect.substring(i65, i65+1);\n");//0
	printf("	arr_alarm_category[12] = detect.substring(i65, i65+1);\n");//0

	printf("	var s66=\"alarm_low_output\";\n");
	printf("	var i66=detect.indexOf(s66)+s66.length+1;\n");
//	printf("	var val_alarm_low_output = detect.substring(i66, i66+1);\n");//0
	printf("	arr_alarm_status[13] = detect.substring(i66, i66+1);\n");//0
	printf("	var s67=\"category_low_output\";\n");
	printf("	var i67=detect.indexOf(s67)+s67.length+1;\n");
//	printf("	var val_category_low_output = detect.substring(i67, i67+1);\n");//0
	printf("	arr_alarm_category[13] = detect.substring(i67, i67+1);\n");//0

	printf("	var s68=\"alarm_dl_over_output\";\n");
	printf("	var i68=detect.indexOf(s68)+s68.length+1;\n");
//	printf("	var val_alarm_dl_over_output = detect.substring(i68, i68+1);\n");//0
	printf("	arr_alarm_status[14] = detect.substring(i68, i68+1);\n");//0
	printf("	var s69=\"category_dl_over_output\";\n");
	printf("	var i69=detect.indexOf(s69)+s69.length+1;\n");
//	printf("	var val_category_dl_over_output = detect.substring(i69, i69+1);\n");//0
	printf("	arr_alarm_category[14] = detect.substring(i69, i69+1);\n");//0

	printf("	var s70=\"alarm_interferer_alarm\";\n");
	printf("	var i70=detect.indexOf(s70)+s70.length+1;\n");
//	printf("	var val_alarm_interferer_alarm = detect.substring(i70, i70+1);\n");//0
	printf("	arr_alarm_status[15] = detect.substring(i70, i70+1);\n");//0
	printf("	var s71=\"category_interferer_alarm\";\n");
	printf("	var i71=detect.indexOf(s71)+s71.length+1;\n");
//	printf("	var val_category_interferer_alarm = detect.substring(i71, i71+1);\n");//0
	printf("	arr_alarm_category[15] = detect.substring(i71, i71+1);\n");//0

	printf("	var s72=\"alarm_vswr\";\n");
	printf("	var i72=detect.indexOf(s72)+s72.length+1;\n");
//	printf("	var val_alarm_vswr = detect.substring(i72, i72+1);\n");//0
	printf("	arr_alarm_status[16] = detect.substring(i72, i72+1);\n");//0
	printf("	var s73=\"category_vswr\";\n");
	printf("	var i73=detect.indexOf(s73)+s73.length+1;\n");
//	printf("	var val_category_vswr = detect.substring(i73, i73+1);\n");//0
	printf("	arr_alarm_category[16] = detect.substring(i73, i73+1);\n");//0

	printf("	var s74=\"alarm_temp\";\n");
	printf("	var i74=detect.indexOf(s74)+s74.length+1;\n");
//	printf("	var val_alarm_temp = detect.substring(i74, i74+1);\n");//0
	printf("	arr_alarm_status[17] = detect.substring(i74, i74+1);\n");//0
	printf("	var s75=\"category_temp\";\n");
	printf("	var i75=detect.indexOf(s75)+s75.length+1;\n");
//	printf("	var val_category_temp = detect.substring(i75, i75+1);\n");//0
	printf("	arr_alarm_category[17] = detect.substring(i75, i75+1);\n");//0

	printf("	var s76=\"alarm_fan\";\n");
	printf("	var i76=detect.indexOf(s76)+s76.length+1;\n");
//	printf("	var val_alarm_fan = detect.substring(i76, i76+1);\n");//0
	printf("	arr_alarm_status[18] = detect.substring(i76, i76+1);\n");//0
	printf("	var s77=\"category_fan\";\n");
	printf("	var i77=detect.indexOf(s77)+s77.length+1;\n");
//	printf("	var val_category_fan = detect.substring(i77, i77+1);\n");//0
	printf("	arr_alarm_category[18] = detect.substring(i77, i77+1);\n");//0
	printf("\n");
 	printf("	document.form0.alarm_status.style.background = (val_alarm_led == 1)? \"#FF0000\": \"#00FF00\";\n");
 //	printf("	document.form0.alarm_status.value = val_alarm_led;\n");
	printf("	document.form0.shutdown_status.style.background = (val_shutdown_led == 1)? \"#FF0000\": \"#00FF00\";\n");
	printf("\n");
	printf("	document.form1.input_led_1.style.background = (1<=val_led_input_power )? \"#00FF00\": \"#000000\";\n");
	printf("	document.form1.input_led_2.style.background = (2<=val_led_input_power )? \"#00FF00\": \"#000000\";\n");
	printf("	document.form1.input_led_3.style.background = (3<=val_led_input_power )? \"#00FF00\": \"#000000\";\n");
	printf("	document.form1.input_led_4.style.background = (4<=val_led_input_power )? \"#00FF00\": \"#000000\";\n");
	printf("	document.form1.input_led_5.style.background = (5<=val_led_input_power )? \"#00FF00\": \"#000000\";\n");
	printf("	document.form1.output_led_1.style.background = (1<=val_led_output_power )? \"#00FF00\": \"#000000\";\n");
	printf("	document.form1.output_led_2.style.background = (2<=val_led_output_power )? \"#00FF00\": \"#000000\";\n");
	printf("	document.form1.output_led_3.style.background = (3<=val_led_output_power )? \"#00FF00\": \"#000000\";\n");
	printf("	document.form1.output_led_4.style.background = (4<=val_led_output_power )? \"#00FF00\": \"#000000\";\n");
	printf("	document.form1.output_led_5.style.background = (5<=val_led_output_power )? \"#00FF00\": \"#000000\";\n");
	printf("\n");
	printf("	document.form1.dl_amp_onoff.value = (0==val_dl_amp_onoff	)? \"OFF\": \"ON\";\n");
	printf("	document.form1.dl_output.value = val_dl_output_power;\n");
	printf("	document.form1.dl_agc_limit.value = val_dl_agc_limit;\n");
	printf("	document.form1.dl_gain.value = val_dl_gain;\n");
	printf("	document.form1.dl_attenuation.value = val_dl_attenuation;\n");
	printf("	document.form1.dl_rssi.value = val_dl_rssi;\n");
	printf("	document.form1.dl_input_low_limit.value = val_dl_input_low_limit;\n");
	printf("	document.form1.dl_low_output_gap.value = val_dl_low_output_gap;\n");
	printf("	document.form1.dl_band1_rssi.value = val_dl_band1_rssi;\n");
	printf("	document.form1.dl_band1_compensation.value = val_dl_band1_compensation;\n");
	printf("	document.form1.dl_band2_rssi.value = val_dl_band2_rssi;\n");
	printf("	document.form1.dl_band2_compensation.value = val_dl_band2_compensation;\n");
	printf("	document.form1.dl_band3_rssi.value = val_dl_band3_rssi;\n");
	printf("	document.form1.dl_band3_compensation.value = val_dl_band3_compensation;\n");
	printf("	document.form1.ul_amp_onoff.value = (0==val_ul_amp_onoff	)? \"OFF\": \"ON\";\n");
	printf("	document.form1.ul_output.value = val_ul_output;\n");
	printf("	document.form1.ul_agc_limit.value = val_ul_agc_limit;\n");
	printf("	document.form1.ul_gain.value = val_ul_gain;\n");
	printf("	document.form1.ul_attenuation.value = val_ul_attenuation;\n");
	printf("	document.form1.amp_version.value = val_ver_amp;\n");
	printf("	document.form1.dfm_version.value = val_ver_dfm;\n");
	printf("	document.form1.temperature.value = val_temperature;\n");
	printf("	document.form1.temperature_limit.value = val_temperature_limit;\n");
	printf("	document.form1.shutdown_onoff.value = (0==val_shutdown_onoff	)? \"OFF\": \"ON\";\n");
	printf("	document.form1.aoc_onoff.value = (0==val_aoc_onoff	)? \"OFF\": \"ON\";\n");
	printf("	document.form1.gainbalance_onoff.value = (0==val_gain_balance_onoff)? \"OFF\": \"ON\";\n");
	printf("	document.form1.agc_onoff.value = (0==val_agc_onoff	)? \"OFF\": \"ON\";\n");
	printf("	document.form1.isolation_value.value = val_isolation_value;\n");
	printf("	document.form1.isolation_limit.value = val_isolation_detect_limit;\n");
	printf("	document.form1.gainbalance_value.value = val_gain_balance_value;\n");
	printf("	document.form1.alarm_delay.value = (0==val_alarm_delay)? 0: 5;\n");
	printf("\n");
	printf("	for(i=0;i<%d;i++) {\n", ALARM_INDEX_MAX);
	// CATEGORY_RSSI(0), CATEGORY_RF_POWER(1), CATEGORY_VSWR(2), CATEGORY_OVER_TEMPERATURE(3), CATEGORY_UNDER_CURRENT(4)
	printf("		if((arr_alarm_category[i] == 0) || (arr_alarm_category[i] == 1) || (arr_alarm_category[i] == 2) || (arr_alarm_category[i] == 3) || (arr_alarm_category[i] == 4)) {\n");
	printf("			if (arr_alarm_status[i]==1) {\n");
	printf("				yellow_cnt[arr_alarm_category[i]]++;\n");
	printf("				alarm_msg_result += str_alarm_color[arr_alarm_status[i]] + \" : \" + str_alarm_category[arr_alarm_category[i]] + \" - \" + str_alarm_name[i]+\"\\n\";\n");
	printf("			} else if (arr_alarm_status[i]==2) {\n");
	printf("				red_cnt[arr_alarm_category[i]]++; \n");
	printf("				alarm_msg_result += str_alarm_color[arr_alarm_status[i]] + \" : \" + str_alarm_category[arr_alarm_category[i]] + \" - \" + str_alarm_name[i]+\"\\n\";\n");
	printf("			}\n");
	/* test == normal
	printf("			else if (arr_alarm_status[i]==0) {\n");
	printf("				yellow_cnt[arr_alarm_category[i]]++; \n");
	printf("				alarm_msg_result += str_alarm_color[arr_alarm_status[i]] + \" : \" + str_alarm_category[arr_alarm_category[i]] + \" - \" + str_alarm_name[i]+\"\\n\";\n");
	printf("			}\n");
	*/
	printf("		}\n");
	printf("	}\n");

	printf("	for(i=0;i<%d;i++) {\n", CATEGORY_INDEX_MAX);
	printf("		if (red_cnt[i]) category_color[i] = 2;\n");
	printf("		else if (yellow_cnt[i]) category_color[i] = 1;\n");
	printf("		else category_color[i] = 0;\n");
	printf("	}\n");
	// alarm clolor
	printf("	document.form1.alarm_rssi.style.background = (category_color[0]==0)? \"#00FF00\": ((category_color[0]==1)? \"#FFFF00\": \"#FF0000\");\n");
	printf("	document.form1.alarm_rf_power.style.background = (category_color[1]==0 )? \"#00FF00\": ((category_color[1]==1 )? \"#FFFF00\": \"#FF0000\");\n");
	printf("	document.form1.alarm_vswr.style.background = (category_color[2]==0 )? \"#00FF00\": ((category_color[2]==1 )? \"#FFFF00\": \"#FF0000\");\n");
	printf("	document.form1.alarm_over_temperature.style.background = (category_color[3]==0 )? \"#00FF00\": ((category_color[3]==1 )? \"#FFFF00\": \"#FF0000\");\n");
	printf("	document.form1.alarm_under_current.style.background = (category_color[4]==0 )? \"#00FF00\": ((category_color[4]==1 )? \"#FFFF00\": \"#FF0000\");\n");
	// alarm message
	printf("	document.form1.alm_msg.value = alarm_msg_result;\n");

	printf("	clearTimeout(time_id);\n");
	printf("	time_id = setTimeout(\"reload()\", 3000);\n");
	printf("}\n");

	printf("//--> \n");
	printf("</script>\n");
	printf("\n");

	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("\n");
	printf("</head>\n");
	printf("\n");
	printf("<body bgcolor=\"#FFFFFF\" text=\"#000000\" onLoad = \"reload()\">\n");

//==============================================================
	html_menu_head(&protocol_status, "RF Status");
//--------------------------------------------------------------------------------

	printf("<form name = \"form1\">\n");
	printf("<table cellspacing=\"0\" border=\"0\" width=\"740\">\n");
	printf("	<tr height=\"22\" style=\"height:16.5pt\">\n");
	printf("		<td height=\"50\" width=\"529\" bgcolor=\"%s\" style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\">\n", service_color[protocol_status.status[amp_id].svc_info]);
	if(protocol_status.status[amp_id].svc_info>7) protocol_status.status[amp_id].svc_info=0;
	if(protocol_status.status[amp_id].svc_info==0)
		sprintf(str, "unknown"); 
	else
		sprintf(str, "%s (model %d)", service_name[protocol_status.status[amp_id].svc_info], protocol_status.status[amp_id].amp_capacity);
	printf("			<p align=\"left\"><font size=\"5\"><b>&nbsp;Status - %s</b></font></br></br>&nbsp;Cascade Code - %s</p>\n", str, protocol_status.status[amp_id].cascade_code);
	printf("		</td>\n");
	printf("		<td bgcolor=\"%s\" style=\"border-width:2; border-top-style:ridge; border-right-style:ridge; border-bottom-style:ridge; border-left-style:hidden;\">\n", service_color[protocol_status.status[amp_id].svc_info] );
	printf("			<p align=\"right\"><input type=\"button\" style=\"width:136;\" name=\"renew\" value=\"Configuration\" onclick=\"location='html.cgi?function=tree_status'\"></p>\n");
	printf("		</td>\n");
	printf("	</tr>\n");
	printf("	<tr>\n");
	memcpy(str_tmp, protocol_status.status[amp_id].serial_no, 16);
	str_tmp[16]=0;
	sprintf(str, "[AMP-%d] S/N:%s", amp_id+1, str_tmp);
	printf("		<td height=\"30\" bgcolor=\"%s\" style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\">\n", service_color[protocol_status.status[amp_id].svc_info]);
	printf("			&nbsp;%s\n", str);
	printf("		</td>\n");
	// add
	if (amp_id==0) strcpy(str, "rfconfig");
	else sprintf(str, "rfconfig_%d", amp_id);
	printf("		<td bgcolor=\"%s\" style=\"border-width:2; border-top-style:hidden; border-right-style:ridge; border-bottom-style:ridge; border-left-style:hidden;\">\n", service_color[protocol_status.status[amp_id].svc_info]);
	printf("			<p align=\"right\"><input type=\"button\" style=\"width:200;\" name=\"rf_config\" value=\"AMP-%d RF Configuration\" onclick=\"location='html.cgi?function=%s'\"></p>\n", amp_id+1, str);
	printf("		</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");

	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\">\n");
	printf("	<tr height=\"30\">\n");
	printf("		<td width=\"26\%\" style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\" align=center>&nbsp;Input Power</td>\n");
	printf("		<td width=\"24\%\" style=\"border-width:2; border-top-style:ridge; border-right-style:ridge; border-bottom-style:ridge; border-left-style:hidden;\">\n");
	printf("			<input type=\"text\" name=\"input_led_1\" style=\"WIDTH:20pt; HEIGHT:15pt; BACKGROUND-COLOR:%s; \" readonly>\n", (1<=protocol_status.status[amp_id].led_input_power)? "lime": "white");
	printf("			<input type=\"text\" name=\"input_led_2\" style=\"WIDTH:20pt; HEIGHT:15pt; BACKGROUND-COLOR:%s; \" readonly>\n", (2<=protocol_status.status[amp_id].led_input_power)? "lime": "white");
	printf("			<input type=\"text\" name=\"input_led_3\" style=\"WIDTH:20pt; HEIGHT:15pt; BACKGROUND-COLOR:lime; \" readonly>\n", (3<=protocol_status.status[amp_id].led_input_power)? "lime": "white");
	printf("			<input type=\"text\" name=\"input_led_4\" style=\"WIDTH:20pt; HEIGHT:15pt; BACKGROUND-COLOR:white; \" readonly>\n", (4<=protocol_status.status[amp_id].led_input_power)? "lime": "white");
	printf("			<input type=\"text\" name=\"input_led_5\" style=\"WIDTH:20pt; HEIGHT:15pt; BACKGROUND-COLOR:white; \" readonly>\n", (5<=protocol_status.status[amp_id].led_input_power)? "lime": "white");
	printf("		</td>\n");
	printf("		<td width=\"26\%\" style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\" align=center>&nbsp;Output Power</td>\n");
	printf("		<td width=\"24\%\" style=\"border-width:2; border-top-style:ridge; border-right-style:ridge;  border-bottom-style:ridge; border-left-style:hidden;\">\n");
	printf("			<input type=\"text\" name=\"output_led_1\" style=\"WIDTH:20pt; HEIGHT:15pt; BACKGROUND-COLOR:lime; \" readonly>\n", (1<=protocol_status.status[amp_id].led_output_power)? "lime": "white");
	printf("			<input type=\"text\" name=\"output_led_2\" style=\"WIDTH:20pt; HEIGHT:15pt; BACKGROUND-COLOR:white; \" readonly>\n", (2<=protocol_status.status[amp_id].led_output_power)? "lime": "white");
	printf("			<input type=\"text\" name=\"output_led_3\" style=\"WIDTH:20pt; HEIGHT:15pt; BACKGROUND-COLOR:white; \" readonly>\n", (3<=protocol_status.status[amp_id].led_output_power)? "lime": "white");
	printf("			<input type=\"text\" name=\"output_led_4\" style=\"WIDTH:20pt; HEIGHT:15pt; BACKGROUND-COLOR:white; \" readonly>\n", (4<=protocol_status.status[amp_id].led_output_power)? "lime": "white");
	printf("			<input type=\"text\" name=\"output_led_5\" style=\"WIDTH:20pt; HEIGHT:15pt; BACKGROUND-COLOR:white; \" readonly>\n", (5<=protocol_status.status[amp_id].led_output_power)? "lime": "white");
	printf("		</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Downlink </b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;DL Output Power</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_output\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dBm</td>\n", protocol_status.status[amp_id].dl_output_power);
	printf("		<td width=\"26\%\" >&nbsp;DL Low RF Power</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_low_output_gap\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dB</td>\n", protocol_status.status[amp_id].dl_low_output_gap);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;DL Gain</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_gain\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dB</td>\n", protocol_status.status[amp_id].dl_gain);
	printf("		<td width=\"26\%\" >&nbsp;DL Attenuation</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_attenuation\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dB</td>\n", protocol_status.status[amp_id].dl_attenuation);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;DL RSSI</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_rssi\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dBm</td>\n", protocol_status.status[amp_id].dl_rssi);
	printf("		<td width=\"26\%\" >&nbsp;DL Low RSSI(-8dB)</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_input_low_limit\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dBm</td>\n", protocol_status.status[amp_id].dl_input_low_limit);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;DL AMP Control</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_amp_onoff\" style=\"WIDTH: 80pt;background:silver\" readonly value=%s></td>\n", (protocol_status.status[amp_id].dl_amp_onoff)? "ON": "OFF");
	printf("		<td width=\"26\%\" >&nbsp;DL ALC Limit</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_agc_limit\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dBm</td>\n", protocol_status.status[amp_id].dl_agc_limit);
//	printf("		<td colspan=\"2\" width=\"50\%\" >&nbsp;</td>\n");
	printf("	</tr>\n");

	char band_open[3];
	char band_name[3][32];
	get_band_open(&protocol_status.status[amp_id], (u8*)band_open, band_name);
	printf("	<tr height=\"25\" %s>\n", (band_open[0])?"":"style=\"display:none;\"");
	printf("		<td width=\"26\%\" >&nbsp;<font size=\"2\">DL Band1 RSSI(%s)</font></td>\n", band_name[0]);
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_band1_rssi\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dBm</td>\n", protocol_status.status[amp_id].dl_band_rssi[0]);
	printf("		<td width=\"26\%\" >&nbsp;<font size=\"2\">Band1 Compensation(%s)</font></td>\n", band_name[0]);
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_band1_compensation\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dB</td>\n", protocol_status.status[amp_id].dl_band_compensation[0]);
	printf("	</tr>\n");

	printf("	<tr height=\"25\" %s>\n", (band_open[1])?"":"style=\"display:none;\"");
	printf("		<td width=\"26\%\" >&nbsp;<font size=\"2\">DL Band2 RSSI(%s)</font></td>\n", band_name[1]);
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_band2_rssi\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dBm</td>\n", protocol_status.status[amp_id].dl_band_rssi[1]);
	printf("		<td width=\"26\%\" >&nbsp;<font size=\"2\">Band2 Compensation(%s)</font></td>\n", band_name[1]);
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_band2_compensation\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dB</td>\n", protocol_status.status[amp_id].dl_band_compensation[1]);
	printf("	</tr>\n");

	printf("	<tr height=\"25\" %s>\n", (band_open[2])?"":"style=\"display:none;\"");
	printf("		<td width=\"26\%\" >&nbsp;<font size=\"2\">DL Band3 RSSI(%s)</font></td>\n", band_name[2]);
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_band3_rssi\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dBm</td>\n", protocol_status.status[amp_id].dl_band_rssi[2]);
	printf("		<td width=\"26\%\" >&nbsp;<font size=\"2\">Band3 Compensation(%s)</font></td>\n", band_name[2]);
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dl_band3_compensation\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dB</td>\n", protocol_status.status[amp_id].dl_band_compensation[2]);
	printf("	</tr>\n");
	printf("</table>\n");

	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Uplink </b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;UL Output Power</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"ul_output\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dBm</td>\n", protocol_status.status[amp_id].ul_output);
	printf("		<td width=\"26\%\" >&nbsp;UL ALC Limit</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"ul_agc_limit\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dBm</td>\n", protocol_status.status[amp_id].ul_agc_limit);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;UL Gain</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"ul_gain\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dB</td>\n", protocol_status.status[amp_id].ul_gain);
	printf("		<td width=\"26\%\" >&nbsp;UL Attenuation</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"ul_attenuation\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dB</td>\n", protocol_status.status[amp_id].ul_attenuation);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;UL AMP Control</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"ul_amp_onoff\" style=\"WIDTH: 80pt;background:silver\" readonly value=%s></td>\n", (protocol_status.status[amp_id].ul_amp_onoff)? "ON": "OFF");
	printf("		<td colspan=\"2\" width=\"50\%\" >&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");

	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Common</b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;AMP Version</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"amp_version\" style=\"WIDTH: 80pt;background:silver\" readonly value=%x.%x></td>\n", protocol_status.status[amp_id].ver_amp>>4, protocol_status.status[amp_id].ver_amp&0x0f);
	printf("		<td width=\"26\%\" >&nbsp;Temperature</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"temperature\" style=\"WIDTH: 80pt;background:silver\" readonly value=%d>&nbsp;'F</td>\n", (int)protocol_status.status[amp_id].temperature);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;DFM Version</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"dfm_version\" style=\"WIDTH: 80pt;background:silver\" readonly value=%x.%x></td>\n", protocol_status.status[amp_id].ver_dfm>>4, protocol_status.status[amp_id].ver_dfm&0x0f);
	printf("		<td width=\"26\%\" >&nbsp;Temperature Upper Limit</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"temperature_limit\" style=\"WIDTH: 80pt;background:silver\" readonly value=%d>&nbsp;'F</td>\n", (int)protocol_status.status[amp_id].temperature_limit);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;Gain Balance Control</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"gainbalance_onoff\" style=\"WIDTH: 80pt;background:silver\" readonly value=%s></td>\n", (protocol_status.status[amp_id].gain_balance_onoff)? "ON": "OFF");
	printf("		<td width=\"26\%\" >&nbsp;Gain Balance Value</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"gainbalance_value\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dB</td>\n", protocol_status.status[amp_id].gain_balance_value);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;Shutdown Control</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"shutdown_onoff\" style=\"WIDTH: 80pt;background:silver\" readonly value=%s></td>\n", (protocol_status.status[amp_id].shutdown_onoff)? "ON": "OFF");
	printf("		<td width=\"26\%\" >&nbsp;Isolation Detect Limit</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"isolation_limit\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dB</td>\n", protocol_status.status[amp_id].isolation_detect_limit);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;ALC Control</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"agc_onoff\" style=\"WIDTH: 80pt;background:silver\" readonly value=%s></td>\n", (protocol_status.status[amp_id].agc_onoff)? "ON": "OFF");
	printf("		<td width=\"26\%\" >&nbsp;Isolation Value</td>\n"); 
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"isolation_value\" style=\"WIDTH: 80pt;background:silver\" readonly value=%6.1f>&nbsp;dB</td>\n", protocol_status.status[amp_id].isolation_value);
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"26\%\" >&nbsp;AOC(Auto Oscillation Control)</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"aoc_onoff\" style=\"WIDTH: 80pt;background:silver\" readonly value=%s></td>\n", (protocol_status.status[amp_id].aoc_onoff)? "ON": "OFF");
#if 0	/* sprint */
	printf("		<td width=\"26\%\" >&nbsp;Alarm Delay</td>\n");
	printf("		<td width=\"24\%\" ><input type=\"text\" name=\"alarm_delay\" style=\"WIDTH: 80pt;background:silver\" readonly value=%d>&nbsp;Minutes</td>\n", (protocol_status.status[amp_id].alarm_delay)? 5: 0);
#else
	printf("		<td colspan=\"2\" width=\"50\%\" >&nbsp;</td>\n");
#endif
	printf("	</tr>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");

	printf("<table name=\"alarm_table\" cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=\"1\">\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"6\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Alarm</b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"2\" width=\"33\%\" >&nbsp;Delay Alarm Reporting Minites</td>\n");
	printf("		<td colspan=\"4\" width=\"67\%\" >\n");
	printf("			<input type=\"text\" name=\"alarm_delay\" style=\"WIDTH: 80pt;background:silver\" readonly value=%d>&nbsp;Minutes\n", (protocol_status.status[amp_id].alarm_delay)? 5: 0);
	printf("		</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"10\%\" bgcolor=\"#CCCCCC\" align=\"center\">Status</td>\n");
	printf("		<td width=\"23\%\" bgcolor=\"#CCCCCC\" align=\"center\">Name</td>\n");
	printf("		<td width=\"10\%\" bgcolor=\"#CCCCCC\" align=\"center\">Status</td>\n");
	printf("		<td width=\"23\%\" bgcolor=\"#CCCCCC\" align=\"center\">Name</td>\n");
	printf("		<td width=\"10\%\" bgcolor=\"#CCCCCC\" align=\"center\">Status</td>\n");
	printf("		<td width=\"24\%\" bgcolor=\"#CCCCCC\" align=\"center\">Name</td>\n");
	printf("	</tr>\n");

	// (ALARM == SNMP) 매핑정보 가져오기
	int j;
	u8 yellow_cnt[CATEGORY_INDEX_MAX], red_cnt[CATEGORY_INDEX_MAX], curr_alarm[CATEGORY_INDEX_MAX];
	bzero(yellow_cnt,   CATEGORY_INDEX_MAX);
	bzero(red_cnt,  CATEGORY_INDEX_MAX);
	bzero(curr_alarm,  CATEGORY_INDEX_MAX);

	for(j=0;j<ALARM_INDEX_MAX;j++) {
		switch(protocol_status.alarm.slot[amp_id].alarm_item[j].category)
		{
			case CATEGORY_RSSI:
			case CATEGORY_RF_POWER:
			case CATEGORY_VSWR:
			case CATEGORY_OVER_TEMPERATURE:
			case CATEGORY_UNDER_CURRENT:
				if (protocol_status.alarm.slot[amp_id].alarm_item[j].status==1) yellow_cnt[protocol_status.alarm.slot[amp_id].alarm_item[j].category]++;
				else if (protocol_status.alarm.slot[amp_id].alarm_item[j].status==2) red_cnt[protocol_status.alarm.slot[amp_id].alarm_item[j].category]++;
				break;
		}
	}
	for(j=0;j<CATEGORY_INDEX_MAX;j++) {
		if (red_cnt[j]) {
			curr_alarm[j] = 2;
		} else if (yellow_cnt[j]) {
			curr_alarm[j] = 1;
		} else {
			curr_alarm[j] = 0;
		}
	}

	printf("	<tr height=\"25\">\n");
	// RSSI
	GetAlarmColorString(str, curr_alarm[CATEGORY_RSSI]);
//	printf("		<td width=\"10\%\" ><div><input type=\"text\" name=\"alarm_rssi\" style=\"width:100\%; height:100\%; background-color: %s\" readonly></div></td>\n", str);
	printf("		<td width=\"10\%\" ><input type=\"text\" name=\"alarm_rssi\" style=\"width:100\%; height:100\%; background-color: %s\" readonly></td>\n", str);
	printf("		<td width=\"23\%\" >&nbsp;RSSI</td>\n");
	// RF Power
	GetAlarmColorString(str, curr_alarm[CATEGORY_RF_POWER]);
//	printf("		<td width=\"10\%\" ><div><input type=\"text\" name=\"alarm_rf_power\" style=\"width:100\%; height:100\%; background-color: %s\" readonly></div></td>\n", str);
	printf("		<td width=\"10\%\" ><input type=\"text\" name=\"alarm_rf_power\" style=\"width:100\%; height:100\%; background-color: %s\" readonly></td>\n", str);
	printf("		<td width=\"23\%\" >&nbsp;RF Power</td>\n");
	// VSWR
	GetAlarmColorString(str, curr_alarm[CATEGORY_VSWR]);
//	printf("		<td width=\"10\%\" ><div><input type=\"text\" name=\"alarm_vswr\" style=\"width:100\%; height:100\%; background-color: %s\" readonly></div></td>\n", str);
	printf("		<td width=\"10\%\" ><input type=\"text\" name=\"alarm_vswr\" style=\"width:100\%; height:100\%; background-color: %s\" readonly></td>\n", str);
	printf("		<td width=\"24\%\" >&nbsp;VSWR</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	// Over Temperature
	GetAlarmColorString(str, curr_alarm[CATEGORY_OVER_TEMPERATURE]);
//	printf("		<td width=\"10\%\" ><div><input type=\"text\" name=\"alarm_over_temperature\" style=\"width:100\%; height:100\%; background-color: %s\" readonly></div></td>\n", str);
	printf("		<td width=\"10\%\" ><input type=\"text\" name=\"alarm_over_temperature\" style=\"width:100\%; height:100\%; background-color: %s\" readonly></td>\n", str);
	printf("		<td width=\"23\%\" >&nbsp;Over Temperature</td>\n");
	// Under Current
	GetAlarmColorString(str, curr_alarm[CATEGORY_UNDER_CURRENT]);
//	printf("		<td width=\"10\%\" ><div><input type=\"text\" name=\"alarm_under_current\" style=\"width:100\%; height:100\%; background-color: %s\" readonly></div></td>\n", str);
	printf("		<td width=\"10\%\" ><input type=\"text\" name=\"alarm_under_current\" style=\"width:100\%; height:100\%; background-color: %s\" readonly></td>\n", str);
	printf("		<td width=\"23\%\" >&nbsp;Under Current</td>\n");
	// blank
	printf("		<td colspan=\"2\" width=\"34\%\">&nbsp;</td>\n");
	printf("	</tr>\n");

	// new current alarm 
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"6\" width=\"100%\">\n");
	//printf("			<textarea name=\"alm_msg\" align=\"left\" style=\"width:725px;height=100px;\">\n");
	//printf("			<textarea name=\"alm_msg\" rows=\"10\" cols=\"98\" align=\"left\">\n");
	printf("			<textarea name=\"alm_msg\" rows=\"5\" align=\"left\" style=\"width:725px;\">\n");
	print_current_alarm_msg(amp_id, &protocol_status.alarm);
	printf("			</textarea>\n");
	printf("		</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");

//band select - lte700
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1 %s>\n", (protocol_status.status[amp_id].svc_info==1)? "": "style=\"display:none\"");
//	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Band Selection</b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_lte700_la\" value=\"1\" %s onclick=check_no_click(this) disabled>&nbsp;Lower A</td>\n", (lte700_band_check[0])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_lte700_lb\" value=\"2\" %s onclick=check_no_click(this) disabled>&nbsp;Lower B</td>\n", (lte700_band_check[1])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_lte700_lc\" value=\"3\" %s onclick=check_no_click(this) disabled>&nbsp;Lower C</td>\n", (lte700_band_check[2])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_lte700_uc\" value=\"4\" %s onclick=check_no_click(this) disabled>&nbsp;Upper C</td>\n", (lte700_band_check[3])? "checked": "");
	printf("	</tr>\n");
	printf("</table>\n");

//band select - Cellular
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1 %s>\n", (protocol_status.status[amp_id].svc_info==2)? "": "style=\"display:none\"");
//	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Band Selection</b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_cell_band_a1\" value=\"1\" %s onclick=check_no_click(this) disabled>&nbsp;A1</td>\n", (cellular_band_check[0])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_cell_band_b1\" value=\"2\" %s onclick=check_no_click(this) disabled>&nbsp;B1</td>\n", (cellular_band_check[1])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_cell_band_a2\" value=\"3\" %s onclick=check_no_click(this) disabled>&nbsp;A2</td>\n", (cellular_band_check[2])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_cell_band_b2\" value=\"4\" %s onclick=check_no_click(this) disabled>&nbsp;B2</td>\n", (cellular_band_check[3])? "checked": "");
	printf("	</tr>\n");
	printf("</table>\n");

#ifdef iDEN800_FUNC_ADD	/* iden800 */
//band select - iDEN
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1 %s>\n", (protocol_status.status[amp_id].svc_info==3)? "": "style=\"display:none\"");
//	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"5\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Band Selection</b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band_6_6m\" value=\"9\" %s onclick=check_no_click(this) disabled>&nbsp;6.6M</td>\n", (iden_band_check[8])? "checked": "");
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band_6_8m\" value=\"8\" %s onclick=check_no_click(this) disabled>&nbsp;6.8M</td>\n", (iden_band_check[7])? "checked": "");
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band_7m\" value=\"7\" %s onclick=check_no_click(this) disabled>&nbsp;7M</td>\n", (iden_band_check[6])? "checked": "");
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band_14_6m\" value=\"6\" %s onclick=check_no_click(this) disabled>&nbsp;14.6M</td>\n", (iden_band_check[5])? "checked": "");
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band_14_8m\" value=\"5\" %s onclick=check_no_click(this) disabled>&nbsp;14.8M</td>\n", (iden_band_check[4])? "checked": "");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band_15m\" value=\"4\" %s onclick=check_no_click(this) disabled>&nbsp;15M</td>\n", (iden_band_check[3])? "checked": "");
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band_17_6m\" value=\"3\" %s onclick=check_no_click(this) disabled>&nbsp;17.6M</td>\n", (iden_band_check[2])? "checked": "");
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band_17_8m\" value=\"2\" %s onclick=check_no_click(this) disabled>&nbsp;17.8M</td>\n", (iden_band_check[1])? "checked": "");
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band_18m\" value=\"1\" %s onclick=check_no_click(this) disabled>&nbsp;18M</td>\n", (iden_band_check[0])? "checked": "");
	printf("		<td width=\"20\%\" align=\"center\">&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");
#endif /* iDEN800_FUNC_ADD */

//band select - PCS
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1 %s>\n", (protocol_status.status[amp_id].svc_info==4)? "": "style=\"display:none\"");
//	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"16\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Band Selection</b></font></td>\n");
	printf("	</tr>\n");
	printf("\n");
	printf("	<tr height=\"4\"><td colspan=\"16\" bgcolor=\"#ffffff\"></td></tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#a0f887\" width=\"135\" align=\"center\" colspan=\"3\"><font size=\"2\"><b>A1</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  align=\"center\" >&nbsp;</td>\n");
	printf("		<td bgcolor=\"#6ff448\" width=\"135\" align=\"center\" colspan=\"3\"><font size=\"2\"><b>A2</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  align=\"center\" >&nbsp;</td>\n");
	printf("		<td bgcolor=\"#2eab0a\" width=\"135\" align=\"center\" colspan=\"3\"><font size=\"2\"><b>A3</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  align=\"center\" >&nbsp;</td>\n");
	printf("		<td bgcolor=\"#a346ff\" width=\"135\" align=\"center\" colspan=\"3\"><font size=\"2\"><b>D</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  align=\"center\" >&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#a0f887\" width=\"45\" ><font size=\"2\">25</font></td>\n");
	printf("		<td bgcolor=\"#a0f887\" width=\"45\" ><font size=\"2\">50</font></td>\n");
	printf("		<td bgcolor=\"#a0f887\" width=\"45\" ><font size=\"2\">75</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><font size=\"2\">100</font></td>\n");
	printf("		<td bgcolor=\"#6ff448\" width=\"45\" ><font size=\"2\">125</font></td>\n");
	printf("		<td bgcolor=\"#6ff448\" width=\"45\" ><font size=\"2\">150</font></td>\n");
	printf("		<td bgcolor=\"#6ff448\" width=\"45\" ><font size=\"2\">175</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><font size=\"2\">200</font></td>\n");
	printf("		<td bgcolor=\"#2eab0a\" width=\"45\" ><font size=\"2\">225</font></td>\n");
	printf("		<td bgcolor=\"#2eab0a\" width=\"45\" ><font size=\"2\">250</font></td>\n");
	printf("		<td bgcolor=\"#2eab0a\" width=\"45\" ><font size=\"2\">275</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><font size=\"2\">300</font></td>\n");
	printf("		<td bgcolor=\"#a346ff\" width=\"45\" ><font size=\"2\">325</font></td>\n");
	printf("		<td bgcolor=\"#a346ff\" width=\"45\" ><font size=\"2\">350</font></td>\n");
	printf("		<td bgcolor=\"#a346ff\" width=\"45\" ><font size=\"2\">375</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><font size=\"2\">400</font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#a0f887\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[0])? "checked": "");
	printf("		<td bgcolor=\"#a0f887\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[1])? "checked": "");
	printf("		<td bgcolor=\"#a0f887\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[2])? "checked": "");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[3])? "checked": "");
	printf("		<td bgcolor=\"#6ff448\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[4])? "checked": "");
	printf("		<td bgcolor=\"#6ff448\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[5])? "checked": "");
	printf("		<td bgcolor=\"#6ff448\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[6])? "checked": "");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[7])? "checked": "");
	printf("		<td bgcolor=\"#2eab0a\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[8])? "checked": "");
	printf("		<td bgcolor=\"#2eab0a\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[9])? "checked": "");
	printf("		<td bgcolor=\"#2eab0a\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[10])? "checked": "");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[11])? "checked": "");
	printf("		<td bgcolor=\"#a346ff\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[12])? "checked": "");
	printf("		<td bgcolor=\"#a346ff\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[13])? "checked": "");
	printf("		<td bgcolor=\"#a346ff\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[14])? "checked": "");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[15])? "checked": "");
	printf("	</tr>\n");
	printf("\n");
	printf("	<tr height=\"4\"><td colspan=\"16\" bgcolor=\"#ffffff\"></td></tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#a3aaeb\" width=\"135\" colspan=\"3\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b>B1</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  >&nbsp;</td>\n");
	printf("		<td bgcolor=\"#747ee0\" width=\"135\" colspan=\"3\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b>B2</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  >&nbsp;</td>\n");
	printf("		<td bgcolor=\"#4755d6\" width=\"135\" colspan=\"3\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b>B3</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  >&nbsp;</td>\n");
	printf("		<td bgcolor=\"#ff80c0\" width=\"135\" colspan=\"3\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b>E</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  >&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#a3aaeb\" width=\"45\" ><font size=\"2\">425</font></td>\n");
	printf("		<td bgcolor=\"#a3aaeb\" width=\"45\" ><font size=\"2\">450</font></td>\n");
	printf("		<td bgcolor=\"#a3aaeb\" width=\"45\" ><font size=\"2\">475</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><font size=\"2\">500</font></td>\n");
	printf("		<td bgcolor=\"#747ee0\" width=\"45\" ><font size=\"2\">525</font></td>\n");
	printf("		<td bgcolor=\"#747ee0\" width=\"45\" ><font size=\"2\">550</font></td>\n");
	printf("		<td bgcolor=\"#747ee0\" width=\"45\" ><font size=\"2\">575</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><font size=\"2\">600</font></td>\n");
	printf("		<td bgcolor=\"#4755d6\" width=\"45\" ><font size=\"2\">625</font></td>\n");
	printf("		<td bgcolor=\"#4755d6\" width=\"45\" ><font size=\"2\">650</font></td>\n");
	printf("		<td bgcolor=\"#4755d6\" width=\"45\" ><font size=\"2\">675</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><font size=\"2\">700</font></td>\n");
	printf("		<td bgcolor=\"#ff80c0\" width=\"45\" ><font size=\"2\">725</font></td>\n");
	printf("		<td bgcolor=\"#ff80c0\" width=\"45\" ><font size=\"2\">740</font></td>\n");
	printf("		<td bgcolor=\"#ff80c0\" width=\"45\" ><font size=\"2\">775</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><font size=\"2\">800</font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#a3aaeb\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[16])? "checked": "");
	printf("		<td bgcolor=\"#a3aaeb\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[17])? "checked": "");
	printf("		<td bgcolor=\"#a3aaeb\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[18])? "checked": "");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[19])? "checked": "");
	printf("		<td bgcolor=\"#747ee0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[20])? "checked": "");
	printf("		<td bgcolor=\"#747ee0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[21])? "checked": "");
	printf("		<td bgcolor=\"#747ee0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[22])? "checked": "");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[23])? "checked": "");
	printf("		<td bgcolor=\"#4755d6\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[24])? "checked": "");
	printf("		<td bgcolor=\"#4755d6\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[25])? "checked": "");
	printf("		<td bgcolor=\"#4755d6\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[26])? "checked": "");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[27])? "checked": "");
	printf("		<td bgcolor=\"#ff80c0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[28])? "checked": "");
	printf("		<td bgcolor=\"#ff80c0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[29])? "checked": "");
	printf("		<td bgcolor=\"#ff80c0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[30])? "checked": "");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[31])? "checked": "");
	printf("	</tr>\n");
	printf("\n");
	printf("	<tr height=\"4\"><td colspan=\"16\" bgcolor=\"#ffffff\"></td></tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"135\" colspan=\"3\"><font size=\"2\">&nbsp;</font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  >&nbsp;</td>\n");
	printf("		<td bgcolor=\"#ffca95\" width=\"225\" colspan=\"5\"><font size=\"2\"><b>C1</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  >&nbsp;</td>\n");
	printf("		<td bgcolor=\"#ff8102\" width=\"225\" colspan=\"5\"><font size=\"2\"><b>C2</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  >&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#80ffff\" width=\"135\" colspan=\"3\"><font size=\"2\"><b>F</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  >&nbsp;</td>\n");
	printf("		<td bgcolor=\"#ffca95\" width=\"135\" colspan=\"3\"><font size=\"2\"><b>C3</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  >&nbsp;</td>\n");
	printf("		<td bgcolor=\"#ffa64d\" width=\"135\" colspan=\"3\"><font size=\"2\"><b>C4</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  >&nbsp;</td>\n");
	printf("		<td bgcolor=\"#ff8102\" width=\"135\" colspan=\"3\"><font size=\"2\"><b>C5</b></font></td>\n");
	printf("		<td bgcolor=\"#ffffff\" width=\"45\"  >&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#80ffff\" width=\"45\" ><font size=\"2\">825</font></td>\n");
	printf("		<td bgcolor=\"#80ffff\" width=\"45\" ><font size=\"2\">850</font></td>\n");
	printf("		<td bgcolor=\"#80ffff\" width=\"45\" ><font size=\"2\">875</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><font size=\"2\">900</font></td>\n");
	printf("		<td bgcolor=\"#ffca95\" width=\"45\" ><font size=\"2\">925</font></td>\n");
	printf("		<td bgcolor=\"#ffca95\" width=\"45\" ><font size=\"2\">950</font></td>\n");
	printf("		<td bgcolor=\"#ffca95\" width=\"45\" ><font size=\"2\">975</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><font size=\"2\">1000</font></td>\n");
	printf("		<td bgcolor=\"#ffa64d\" width=\"45\" ><font size=\"2\">1025</font></td>\n");
	printf("		<td bgcolor=\"#ffa64d\" width=\"45\" ><font size=\"2\">1050</font></td>\n");
	printf("		<td bgcolor=\"#ffa64d\" width=\"45\" ><font size=\"2\">1075</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><font size=\"2\">1100</font></td>\n");
	printf("		<td bgcolor=\"#ff8102\" width=\"45\" ><font size=\"2\">1125</font></td>\n");
	printf("		<td bgcolor=\"#ff8102\" width=\"45\" ><font size=\"2\">1150</font></td>\n");
	printf("		<td bgcolor=\"#ff8102\" width=\"45\" ><font size=\"2\">1175</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><font size=\"2\">1200</font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#80ffff\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[32])? "checked": "");
	printf("		<td bgcolor=\"#80ffff\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[33])? "checked": "");
	printf("		<td bgcolor=\"#80ffff\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[34])? "checked": "");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[35])? "checked": "");
	printf("		<td bgcolor=\"#ffca95\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[36])? "checked": "");
	printf("		<td bgcolor=\"#ffca95\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[37])? "checked": "");
	printf("		<td bgcolor=\"#ffca95\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[38])? "checked": "");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[39])? "checked": "");
	printf("		<td bgcolor=\"#ffa64d\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[40])? "checked": "");
	printf("		<td bgcolor=\"#ffa64d\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[41])? "checked": "");
	printf("		<td bgcolor=\"#ffa64d\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[42])? "checked": "");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[43])? "checked": "");
	printf("		<td bgcolor=\"#ff8102\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[44])? "checked": "");
	printf("		<td bgcolor=\"#ff8102\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[45])? "checked": "");
	printf("		<td bgcolor=\"#ff8102\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[46])? "checked": "");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this)  disabled></td>\n", (pcs_band_check[47])? "checked": "");
	printf("	</tr>\n");
	printf("\n");
	printf("	<tr height=\"4\"><td colspan=\"16\" bgcolor=\"#ffffff\"></td></tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td height=\"25\" bgcolor=\"#ff0080\" width=\"135\" colspan=\"3\"><font size=\"2\"><b>G</b></font></td>\n");
#if 0	/* sprint */
	printf("		<td height=\"25\" align=\"center\" colspan=\"13\">&nbsp;</td>\n");
#else
	printf("		<td height=\"25\" bgcolor=\"#00e372\" colspan=\"4\"><font size=\"2\"><b>LTE</b></font></td>\n");
	printf("		<td height=\"25\" align=\"center\" colspan=\"9\">&nbsp;</td>\n");
#endif

	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td height=\"25\" bgcolor=\"#ff0080\" width=\"45\" ><font size=\"2\">1225</font></td>\n");
	printf("		<td height=\"25\" bgcolor=\"#ff0080\" width=\"45\" ><font size=\"2\">1250</font></td>\n");
	printf("		<td height=\"25\" bgcolor=\"#ff0080\" width=\"45\" ><font size=\"2\">1275</font></td>\n");
#if 0	/* sprint */
	printf("		<td height=\"25\" width=\"45\" colspan=\"13\">&nbsp;</td>\n");
#else
	printf("		<td colspan=\"4\" height=\"25\" bgcolor=\"#00e372\"><font size=\"2\">1250</font></td>\n");
	printf("		<td height=\"25\" width=\"45\" colspan=\"9\">&nbsp;</td>\n");
#endif
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#ff0080\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this) disabled></td>\n", (pcs_band_check[48])? "checked": "");
	printf("		<td bgcolor=\"#ff0080\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this) disabled></td>\n", (pcs_band_check[49])? "checked": "");
	printf("		<td bgcolor=\"#ff0080\" width=\"45\" ><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this) disabled></td>\n", (pcs_band_check[50])? "checked": "");
#if 1	/* sprint */
	printf("		<td colspan=\"4\" bgcolor=\"#00e372\"><input type=\"checkbox\" name=\"check_pcs_band\" %s  onclick=check_no_click(this) disabled></td>\n", (pcs_band_check[51])? "checked": "");
	printf("		<td height=\"25\" width=\"45\" colspan=\"9\">&nbsp;</td>\n");
#endif
	printf("	</tr>\n");
	printf("\n");
	printf("</table>\n");

//band select - aws
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1 %s>\n", (protocol_status.status[amp_id].svc_info==5)? "": "style=\"display:none\"");
//	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"17\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Band Selection</b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td align=\"center\">&nbsp;A1</td>\n");
	printf("		<td align=\"center\">&nbsp;+ </td>\n");
	printf("		<td align=\"center\">&nbsp;A2</td>\n");
	printf("		<td align=\"center\">&nbsp;+ </td>\n");
	printf("		<td align=\"center\">&nbsp;B1</td>\n");
	printf("		<td align=\"center\">&nbsp;+ </td>\n");
	printf("		<td align=\"center\">&nbsp;B2</td>\n");
	printf("		<td align=\"center\">&nbsp;+ </td>\n");
	printf("		<td align=\"center\">&nbsp;C </td>\n");
	printf("		<td align=\"center\">&nbsp;+ </td>\n");
	printf("		<td align=\"center\">&nbsp;D </td>\n");
	printf("		<td align=\"center\">&nbsp;+ </td>\n");
	printf("		<td align=\"center\">&nbsp;E </td>\n");
	printf("		<td align=\"center\">&nbsp;+ </td>\n");
	printf("		<td align=\"center\">&nbsp;F1</td>\n");
	printf("		<td align=\"center\">&nbsp;+ </td>\n");
	printf("		<td align=\"center\">&nbsp;F2</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_a1\"  value= \"0\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[0])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_a1_\" value= \"1\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[1])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_a2\"  value= \"2\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[2])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_a2_\" value= \"3\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[3])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_b1\"  value= \"4\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[4])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_b1_\" value= \"5\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[5])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_b2\"  value= \"6\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[6])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_b2_\" value= \"7\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[7])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_c\"   value= \"8\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[8])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_c_\"  value= \"9\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[9])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_d\"   value=\"10\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[10])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_d_\"  value=\"11\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[11])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_e\"   value=\"12\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[12])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_e_\"  value=\"13\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[13])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_f1\"  value=\"14\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[14])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_f1_\" value=\"15\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[15])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_f2\"  value=\"16\" %s  onclick=check_no_click(this)  disabled></td>\n", (aws_band_check[16])? "checked": "");
	printf("	</tr>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");

	printf("</form>");
//===============================
	printf("</body>\n");
	printf("</html>\n");
}
