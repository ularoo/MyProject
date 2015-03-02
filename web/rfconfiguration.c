#include <stdio.h>
#include "protocol.h"
#include "debug.h"
#include "main.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

extern const char* service_name[];
extern const char* service_color[];

#define	PCS_BAND_MAX		52
void rfconfig(int amp_id)
{
	int i;
	char str[64], str_tmp[64];
#if 0	/* sprint */
	char pcs_band_check[51],
#else
	char pcs_band_check[PCS_BAND_MAX], 	// G Band LTE Ãß°¡
#endif
		lte700_band_check[4],	//lowA,lowB,lowC,lowD
		cellular_band_check[4],	//A1,A2,B1,B2
#ifdef iDEN800_FUNC_ADD	/* iden800 */
		iden_band_check[9], //6.6M,6.8M,7M,14.6M,14.8M,15M,17.6M,17.8M,18M (only one selected)
#endif /* iDEN800_FUNC_ADD */
		aws_band_check[17];	//A1,+,A2,+,B1,+,B2,+,C,+,D,+,E,+,F1,+,F2
	
	struct protocol_web_status protocol_status;
	if(get_status(&protocol_status)<0) return;

	memset(pcs_band_check, 0, sizeof(pcs_band_check));
	for(i=0; i<protocol_status.status[amp_id].pcs_band[1]; i++) 
	{
		if(protocol_status.status[amp_id].pcs_band[0]+i>=PCS_BAND_MAX) continue;
		pcs_band_check[protocol_status.status[amp_id].pcs_band[0]+i]=1;
	}
	for(i=0; i<protocol_status.status[amp_id].pcs_band[3]; i++) 
	{
		if(protocol_status.status[amp_id].pcs_band[2]+i>=PCS_BAND_MAX) continue;
		pcs_band_check[protocol_status.status[amp_id].pcs_band[2]+i]=1;
	}
	for(i=0; i<protocol_status.status[amp_id].pcs_band[5]; i++) 
	{
		if(protocol_status.status[amp_id].pcs_band[4]+i>=PCS_BAND_MAX) continue;
		pcs_band_check[protocol_status.status[amp_id].pcs_band[4]+i]=1;
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

#ifdef iDEN800_FUNC_ADD   /* iden800 */
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

	printf("function status_load(oj)\n");
	printf("{\n");
	printf("	var detect = decodeURIComponent(oj.responseText);\n");
	printf("	var s00=\"alarm_led\";\n");
	printf("	var i00=detect.indexOf(s00)+s00.length+1;\n");
	printf("	var val_alarm_led = detect.substring(i00, i00+1);\n");//i00 to i00; 1 character
	printf("	var s01=\"shutdown_led\";\n");
	printf("	var i01=detect.indexOf(s01)+s01.length+1;\n");
	printf("	var val_shutdown_led = detect.substring(i01, i01+1);\n");//0
 	printf("	document.form0.alarm_status.style.background = (val_alarm_led == 1)? \"#FF0000\": \"#00FF00\";\n");
	printf("	document.form0.shutdown_status.style.background = (val_shutdown_led == 1)? \"#FF0000\": \"#00FF00\";\n");
	printf("	clearTimeout(time_id);\n");
	printf("	time_id = setTimeout(\"reload()\", 3000);\n");
	printf("}\n");
	printf("\n");

	// Submit
	printf("%s", 
"var i = 0;\n"
"var check = new Array();\n"
"var max = 0;\n"
"var width = 0;\n"
"var blocks = 0;\n"
"var first = -1;\n"
"var last = -1;\n"
"var rollback = 0;\n"
"\n"
"function onSubmit(){\n"
"	if(1 == form1.flag_band_aws.value)\n"
"	{\n"
"		i = 0;\n"
"		check[i] = document.form1.check_aws_a1; i++;\n"
"		check[i] = document.form1.check_aws_a1_; i++;\n"
"		check[i] = document.form1.check_aws_a2; i++;\n"
"		check[i] = document.form1.check_aws_a2_; i++;\n"
"		check[i] = document.form1.check_aws_b1; i++;\n"
"		check[i] = document.form1.check_aws_b1_; i++;\n"
"		check[i] = document.form1.check_aws_b2; i++;\n"
"		check[i] = document.form1.check_aws_b2_; i++;\n"
"		check[i] = document.form1.check_aws_c; i++;\n"
"		check[i] = document.form1.check_aws_c_; i++;\n"
"		check[i] = document.form1.check_aws_d; i++;\n"
"		check[i] = document.form1.check_aws_d_; i++;\n"
"		check[i] = document.form1.check_aws_e; i++;\n"
"		check[i] = document.form1.check_aws_e_; i++;\n"
"		check[i] = document.form1.check_aws_f1; i++;\n"
"		check[i] = document.form1.check_aws_f1_; i++;\n"
"		check[i] = document.form1.check_aws_f2; i++;\n"
"		for(i = 0, first = -1, last = -1; i < check.length; i++)\n"
"		{\n"
"			if(check[i].checked)\n"
"			{\n"
"				if(-1 == first)\n"
"					first = i;\n"
"			}\n"
"			else\n"
"			if(0 <= first)\n"
"			{\n"
"				last = i - 1;\n"
"				if(first <= last)\n"
"				{\n"
"					if(1 == (first % 2))\n"
"						check[first].checked = false;\n"
"					if(1 == (last % 2))\n"
"						check[last].checked = false;\n"
"				}\n"
"				first = -1;\n"
"				last = -1;\n"
"			}\n"
"		}\n"
"		if(0 <= first)\n"
"		{\n"
"			last = i - 1;\n"
"			if(first <= last)\n"
"			{\n"
"				if(1 == (first % 2))\n"
"					check[first].checked = false;\n"
"				if(1 == (last % 2))\n"
"					check[last].checked = false;\n"
"			}\n"
"		}\n"
"		document.form1.hidden_band_aws[0].value=0x00;\n"
"		document.form1.hidden_band_aws[1].value=0x00;\n"
"		document.form1.hidden_band_aws[2].value=0x00;\n"
"		if(document.form1.check_aws_a1.checked) document.form1.hidden_band_aws[0].value|=0x01;\n"
"		if(document.form1.check_aws_a1_.checked)document.form1.hidden_band_aws[0].value|=0x02;\n"
"		if(document.form1.check_aws_a2.checked) document.form1.hidden_band_aws[0].value|=0x04;\n"
"		if(document.form1.check_aws_a2_.checked)document.form1.hidden_band_aws[0].value|=0x08;\n"
"		if(document.form1.check_aws_b1.checked) document.form1.hidden_band_aws[0].value|=0x10;\n"
"		if(document.form1.check_aws_b1_.checked)document.form1.hidden_band_aws[0].value|=0x20;\n"
"		if(document.form1.check_aws_b2.checked) document.form1.hidden_band_aws[0].value|=0x40;\n"
"		if(document.form1.check_aws_b2_.checked)document.form1.hidden_band_aws[0].value|=0x80;\n"
"		if(document.form1.check_aws_c.checked)  document.form1.hidden_band_aws[1].value|=0x01;\n"
"		if(document.form1.check_aws_c_.checked) document.form1.hidden_band_aws[1].value|=0x02;\n"
"		if(document.form1.check_aws_d.checked)  document.form1.hidden_band_aws[1].value|=0x04;\n"
"		if(document.form1.check_aws_d_.checked) document.form1.hidden_band_aws[1].value|=0x08;\n"
"		if(document.form1.check_aws_e.checked) document.form1.hidden_band_aws[1].value|=0x10;\n"
"		if(document.form1.check_aws_e_.checked)document.form1.hidden_band_aws[1].value|=0x20;\n"
"		if(document.form1.check_aws_f1.checked) document.form1.hidden_band_aws[1].value|=0x40;\n"
"		if(document.form1.check_aws_f1_.checked)document.form1.hidden_band_aws[1].value|=0x80;\n"
"		if(document.form1.check_aws_f2.checked) document.form1.hidden_band_aws[2].value|=0x01;\n"
"	}\n"
"	else\n"
"	if(1 == form1.flag_band_lte700.value)\n"
"	{\n"
"		document.form1.hidden_band_lte.value=0x00;\n"
"		if(document.form1.check_lte700_la.checked) document.form1.hidden_band_lte.value|=0x01;\n"
"		if(document.form1.check_lte700_lb.checked) document.form1.hidden_band_lte.value|=0x02;\n"
"		if(document.form1.check_lte700_lc.checked) document.form1.hidden_band_lte.value|=0x04;\n"
"		if(document.form1.check_lte700_uc.checked) document.form1.hidden_band_lte.value|=0x08;\n"
"	}\n"
"}\n"
"\n");
	printf("\n");

	// CheckObj
	printf("function CheckCheckObj(CheckObj)\n");
	printf("{\n");
	printf("	CheckObj.checked=\"true\";\n");
	printf("	CheckObj.value=\"1\";\n");
//	printf("	form1.flag_amp_id.value=\"%d\";\n", amp_id);
//	printf("alert(\"CheckCheckObj()\" );\n");
	printf("}\n");
	printf("\n");

	//LTE Band
	printf("function CheckLte700Band(val)\n");
	printf("{\n");
	printf("	CheckCheckObj(form1.flag_band_lte700);\n");
	printf("%s", 
"	rollback = 0;\n"
"	i = 0;\n"
"	check[i] = document.form1.check_lte700_la; i++;\n"
"	check[i] = document.form1.check_lte700_lb; i++;\n"
"	check[i] = document.form1.check_lte700_lc; i++;\n"
"	check[i] = document.form1.check_lte700_uc; i++;\n"
"	for(i = 0, width = 0, blocks = 0; i < check.length - 1; i++)\n"
"	{\n"
"		if(check[i].checked)\n"
"			width++;\n"
"		else\n"
"		{\n"
"			if(0 < width)\n"
"				blocks++;\n"
"			width = 0;\n"
"		}\n"
"	}\n"
"	if(0 < width)\n"
"		blocks++;\n"
"	if(check[i].checked)\n"
"		blocks++;\n"
"	if(2 < blocks)\n"
"	{\n"
"		alert(\"too many blocks, maximum 2 blocks avaliable\");\n"
"		rollback = 1;\n"
"	}\n"
"	if(rollback)\n"
"		check[val].checked = !check[val].checked;\n"
	);
	printf("}\n");
	printf("\n");

	//Cellular Band
	printf("function CheckCellularBand(val){\n");
	printf("	CheckCheckObj(form1.flag_band_cellular);\n");
	printf("	document.form1.hidden_band_cellular.value=0x00;\n");
	printf("	if(document.form1.check_cell_band_a1.checked) document.form1.hidden_band_cellular.value|=0x01;\n");
	printf("	if(document.form1.check_cell_band_b1.checked) document.form1.hidden_band_cellular.value|=0x02;\n");
	printf("	if(document.form1.check_cell_band_a2.checked) document.form1.hidden_band_cellular.value|=0x04;\n");
	printf("	if(document.form1.check_cell_band_b2.checked) document.form1.hidden_band_cellular.value|=0x08;\n");
	printf("}\n");
	printf("\n");

	//iDEN Band
#ifdef iDEN800_FUNC_ADD	/* iden800 */
	printf("function CheckiDENBand(val){\n");
	printf("	CheckCheckObj(form1.flag_band_iden);\n");
	printf("	document.form1.hidden_band_iden.value=0x00;\n");
	printf("	if(document.form1.check_iden_band[0].checked) document.form1.hidden_band_iden.value=0x09;\n");	//6.6M
	printf("	if(document.form1.check_iden_band[1].checked) document.form1.hidden_band_iden.value=0x08;\n");	//6.8M
	printf("	if(document.form1.check_iden_band[2].checked) document.form1.hidden_band_iden.value=0x07;\n");	//7M
	printf("	if(document.form1.check_iden_band[3].checked) document.form1.hidden_band_iden.value=0x06;\n");	//14.6M
	printf("	if(document.form1.check_iden_band[4].checked) document.form1.hidden_band_iden.value=0x05;\n");	//14.8M
	printf("	if(document.form1.check_iden_band[5].checked) document.form1.hidden_band_iden.value=0x04;\n");	//15M
	printf("	if(document.form1.check_iden_band[6].checked) document.form1.hidden_band_iden.value=0x03;\n");	//17.6M
	printf("	if(document.form1.check_iden_band[7].checked) document.form1.hidden_band_iden.value=0x02;\n");	//17.6M
	printf("	if(document.form1.check_iden_band[8].checked) document.form1.hidden_band_iden.value=0x01;\n");	//18M
	printf("}\n");
	printf("\n");
#endif /* iDEN800_FUNC_ADD */

	//AWS Band
	printf("function CheckAwsBand(value){\n");
	printf("	CheckCheckObj(form1.flag_band_aws);\n");
	printf("%s", 
"	rollback = 0;\n"
"	i = 0;\n"
"	check[i] = document.form1.check_aws_a1; i++;\n"
"	check[i] = document.form1.check_aws_a1_; i++;\n"
"	check[i] = document.form1.check_aws_a2; i++;\n"
"	check[i] = document.form1.check_aws_a2_; i++;\n"
"	check[i] = document.form1.check_aws_b1; i++;\n"
"	check[i] = document.form1.check_aws_b1_; i++;\n"
"	check[i] = document.form1.check_aws_b2; i++;\n"
"	check[i] = document.form1.check_aws_b2_; i++;\n"
"	check[i] = document.form1.check_aws_c; i++;\n"
"	check[i] = document.form1.check_aws_c_; i++;\n"
"	check[i] = document.form1.check_aws_d; i++;\n"
"	check[i] = document.form1.check_aws_d_; i++;\n"
"	check[i] = document.form1.check_aws_e; i++;\n"
"	check[i] = document.form1.check_aws_e_; i++;\n"
"	check[i] = document.form1.check_aws_f1; i++;\n"
"	check[i] = document.form1.check_aws_f1_; i++;\n"
"	check[i] = document.form1.check_aws_f2; i++;\n"
"	for(i = 0, width = 0, max = 0, blocks = 0; i < check.length; i++)\n"
"	{\n"
"		if(check[i].checked)\n"
"		{\n"
"			if(0 == (i % 2))\n"
"				width += 5;\n"
"		}\n"
"		else\n"
"		{\n"
"			if(0 < width)\n"
"			{\n"
"				max += width;\n"
"				blocks++;\n"
"			}\n"
"			width = 0;\n"
"		}\n"
"	}\n"
"	if(0 < width)\n"
"	{\n"
"		max += width;\n"
"		blocks++;\n"
"	}\n"
"	if(15 < max)\n"
"	{\n"
"		alert(\"too wide bandwidth, maximun 15MHz avaliable\");\n"
"		rollback = 1;\n"
"	}\n"
"	if(2 < blocks)\n"
"	{\n"
"		alert(\"too many blocks, maximum 2 blocks avaliable\");\n"
"		rollback = 1;\n"
"	}\n"
"	if(rollback)\n"
"		check[value].checked = !check[value].checked;\n"
	);
	printf("}\n");
	printf("\n");

	//PCS Band
	printf("function CheckPcsBand(val, divide){\n");
	printf("	var i, j;\n");
	printf("	ary_band_from = new Array(3);\n");
	printf("	ary_band_to = new Array(3);\n");
	printf("	ary_band_cnt = new Array(3);\n");
#if 1	/* sprint */
	printf("	if (divide == 1) {\n");
	// checkbox direct selected
	printf("		if (SoloOverCountCheck(val) == 1) return 1;\n");
	// G Band & LTE Band checkbox direct selected
	printf("		if(val==51) {\n");
	printf("			if((document.form1.check_pcs_band[48].checked) || (document.form1.check_pcs_band[49].checked) || (document.form1.check_pcs_band[50].checked)) {\n");
	printf("				document.form1.check_pcs_band[val].checked ^=1;\n");
	printf("				alert(\"First, please unckeck the G Band\");\n");
	printf("				return 1;\n");
	printf("			}\n");
	printf("		} else if ((val==48) || (val==49) || (val==50)) {\n");
	printf("			if(document.form1.check_pcs_band[51].checked) {\n");
	printf("				document.form1.check_pcs_band[val].checked ^=1;\n");
	printf("				alert(\"First, please unckeck the LTE Band\");\n");
	printf("				return 1;\n");
	printf("			}\n");
	printf("		}\n");
	printf("	}\n");
	printf("	k=0;\n");
	printf("	for(i=0;i<%d;i++) {\n", PCS_BAND_MAX);
	printf("		if(document.form1.check_pcs_band[i].checked) {\n");
	printf("			ary_band_from[k]=i;\n");
	printf("			if (i==%d) {\n", PCS_BAND_MAX-1);
	printf("				ary_band_cnt[k++]=1;\n");
	printf("			} else {\n");
	printf("				for(j=i-(-1);j<%d;j++) {\n", PCS_BAND_MAX);
	printf("					if(!document.form1.check_pcs_band[j].checked) {\n");
	printf("						ary_band_cnt[k++]=(j-i);\n");
	printf("						i = j;\n");
	printf("						break;\n");
	printf("					}\n");
	printf("				}\n");
	printf("			}\n");
	printf("		}\n");
	printf("	}\n");
	printf("	for(i=0;i<3;i++) {\n");
	printf("		document.form1.hidden_band_pcs_start[i].value=ary_band_from[i];\n");
	printf("		document.form1.hidden_band_pcs_count[i].value=ary_band_cnt[i];\n");
	printf("	}\n");
	printf("	document.form1.flag_band_pcs.value=1;\n");
	printf("	return 0;\n");
	printf("}\n");
	printf("\n");
#endif

	printf("function ButtonPcsBand_Clear(){\n");
	printf("	var i;\n");
	printf("	for(i=0;i<%d;i++)\n", PCS_BAND_MAX);
	printf("		document.form1.check_pcs_band[i].checked = 0;\n");
	printf("}\n");

#if 1	/* sprint */
	printf("function SoloOverCountCheck(begin_idx){\n");
	printf("	var i=0,lc=0,rc=0,lgc=0,rgc=0,ft_flag=0;\n");
	printf("	var total=0\n");
	/* max check 11 over */
	printf("	if(document.form1.check_pcs_band[begin_idx].checked == 1) {\n");
	printf("		for(i=begin_idx-1;i>=0;i--) {\n");
	printf("			if(document.form1.check_pcs_band[i].checked) lc++;\n");
	printf("			else break;\n");
	printf("		}\n"); 
	printf("		for(i=begin_idx-(-1);i<=%d;i++) {\n", PCS_BAND_MAX-1);
	printf("			if(document.form1.check_pcs_band[i].checked) rc++;\n");
	printf("			else break;\n");
	printf("		}\n"); 
	printf("		total=lc+rc+1;\n");
	//printf("		if((lc+rc+1)>11) {\n");
	printf("		if((lc+rc+1)>15) {\n");
	printf("			document.form1.check_pcs_band[begin_idx].checked = 0;\n");
	printf("			alert(\"too wide bandwidth, maximun 18.75MHz avaliable\");\n");
	printf("			return 1;\n"); 
	printf("		}\n"); 
	/* max check group 3 over */
	printf("		for(i=begin_idx-1;i>=0;i--) {\n");
	printf("			if(ft_flag) {\n");
	printf("				if(document.form1.check_pcs_band[i].checked) {\n");
	printf("					lgc++;\n");
	printf("					ft_flag=0;\n");
	printf("				} else continue;\n");
	printf("			} else {\n");
	printf("				if(document.form1.check_pcs_band[i].checked) continue;\n");
	printf("				else ft_flag=1;\n");
	printf("			}\n");
	printf("		}\n"); 
	printf("		ft_flag=0;\n");
	printf("		for(i=begin_idx-(-1);i<=%d;i++) {\n", PCS_BAND_MAX-1);
	printf("			if(ft_flag) {\n");
	printf("				if(document.form1.check_pcs_band[i].checked) {\n");
	printf("					rgc++;\n");
	printf("					ft_flag=0;\n");
	printf("				} else continue;\n");
	printf("			} else {\n");
	printf("				if(document.form1.check_pcs_band[i].checked) continue;\n");
	printf("				else ft_flag=1;\n");
	printf("			}\n");
	printf("		}\n"); 
	printf("		total=(lgc+rgc+1);\n");
	printf("		if((lgc+rgc+1)>3) {\n");
	printf("			document.form1.check_pcs_band[begin_idx].checked = 0;\n");
	printf("			alert(\"too many blocks, maximum 3 blocks avaliable\");\n");
	printf("			return 1;\n"); 
	printf("		}\n"); 
	/* max uncheck group 3 over */
	printf("	} else {\n");
	printf("		for(i=begin_idx-1;i>=0;i--) {\n");
	printf("			if(ft_flag) {\n");
	printf("				if(!document.form1.check_pcs_band[i].checked) {\n");
	printf("					lgc++;\n");
	printf("					ft_flag=0;\n");
	printf("				} else continue;\n");
	printf("			} else {\n");
	printf("				if(!document.form1.check_pcs_band[i].checked) continue;\n");
	printf("				else ft_flag=1;\n");
	printf("			}\n");
	printf("		}\n"); 
	printf("		ft_flag=0;\n");
	printf("		for(i=begin_idx-(-1);i<=%d;i++) {\n", PCS_BAND_MAX-1);
	printf("			if(ft_flag) {\n");
	printf("				if(!document.form1.check_pcs_band[i].checked) {\n");
	printf("					rgc++;\n");
	printf("					ft_flag=0;\n");
	printf("				} else continue;\n");
	printf("			} else {\n");
	printf("				if(!document.form1.check_pcs_band[i].checked) continue;\n");
	printf("				else ft_flag=1;\n");
	printf("			}\n");
	printf("		}\n"); 
	printf("		total=(lgc+rgc+1);\n");
	printf("		if((lgc+rgc+1)>3) {\n");
	printf("			document.form1.check_pcs_band[begin_idx].checked = 1;\n");
	printf("			alert(\"too many blocks, maximum 3 blocks avaliable\");\n");
	printf("			return 1;\n"); 
	printf("		}\n"); 
	printf("	}\n");
	printf("	return 0;\n");
	printf("}\n");

	printf("function GroupOverCountCheck(begin_idx, end_idx){\n");
	printf("	var i=0,lc=0,rc=0,lgc=0,rgc=0, ft_flag=0;\n");
	printf("	var total=0\n");
	/* max check 11 over */
	printf("	for(i=begin_idx-1;i>=0;i--) {\n");
	printf("		if(document.form1.check_pcs_band[i].checked) lc++;\n");
	printf("		else break;\n");
	printf("	}\n"); 
	printf("	for(i=end_idx-(-1);i<=%d;i++) {\n", PCS_BAND_MAX-1);
	printf("		if(document.form1.check_pcs_band[i].checked) rc++;\n");
	printf("		else break;\n");
	printf("	}\n"); 
	printf("	total=(lc+rc+(end_idx-begin_idx+1));\n");
	//printf("	if((lc+rc+(end_idx-begin_idx+1))>11) {\n");
	printf("	if((lc+rc+(end_idx-begin_idx+1))>15) {\n");
	printf("		alert(\"too wide bandwidth, maximun 18.75MHz avaliable\");\n");
	printf("		return 1;\n"); 
	printf("	}\n"); 
	/* max check group 3 over */
	printf("	for(i=begin_idx-1;i>=0;i--) {\n");
	printf("		if(ft_flag) {\n");
	printf("			if(document.form1.check_pcs_band[i].checked) {\n");
	printf("				lgc++;\n");
	printf("				ft_flag=0;\n");
	printf("			} else continue;\n");
	printf("		} else {\n");
	printf("			if(document.form1.check_pcs_band[i].checked) continue;\n");
	printf("			else ft_flag=1;\n");
	printf("		}\n");
	printf("	}\n"); 
	printf("	ft_flag=0;\n");
	printf("	for(i=end_idx-(-1);i<=%d;i++) {\n", PCS_BAND_MAX-1);
	printf("		if(ft_flag) {\n");
	printf("			if(document.form1.check_pcs_band[i].checked) {\n");
	printf("				rgc++;\n");
	printf("				ft_flag=0;\n");
	printf("			} else continue;\n");
	printf("		} else {\n");
	printf("			if(document.form1.check_pcs_band[i].checked) continue;\n");
	printf("			else ft_flag=1;\n");
	printf("		}\n");
	printf("	}\n"); 
	printf("	total=(lgc+rgc+1);\n");
	printf("	if((lgc+rgc+1)>3) {\n");
	printf("		alert(\"too many blocks, maximum 3 blocks avaliable\");\n");
	printf("		return 1;\n"); 
	printf("	}\n"); 
	/* G Band & LTE Band duplicate check */
	printf("	if(begin_idx==51) {\n");		// LTE band
	printf("		if((document.form1.check_pcs_band[48].checked) || (document.form1.check_pcs_band[49].checked) || (document.form1.check_pcs_band[50].checked)) {\n");
	printf("			document.form1.check_pcs_band[51].checked = 0;\n");
	printf("			alert(\"First, please unckeck the G Band\");\n");
	printf("			return 1;\n"); 
	printf("		}\n");
	printf("	} else if (begin_idx==48) {\n");	// G band
	printf("		if(document.form1.check_pcs_band[51].checked) {\n");
	printf("			for(i=begin_idx;i<=end_idx;i++) {\n");
	printf("				document.form1.check_pcs_band[i].checked = 0;\n");
	printf("			}\n");
	printf("			alert(\"First, please unckeck the LTE Band\");\n");
	printf("			return 1;\n"); 
	printf("		}\n");
	printf("	}\n");
	printf("	return 0;\n");
	printf("}\n");
#endif

	printf("function ButtonPcsBand_A(){\n");
	printf("	var i;\n");
	printf("	if(GroupOverCountCheck(0,10) == 1) return 0;\n");
	printf("	for(i=0;i<11;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(0,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_A1(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(0,2) == 1) return 0;\n");
	printf("	for(i=0;i<3;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(0,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_A2(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(4,6) == 1) return 0;\n");
	printf("	for(i=4;i<7;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(4,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_A3(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(8,10) == 1) return 0;\n");
	printf("	for(i=8;i<11;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(8,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_D(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(12,14) == 1) return 0;\n");
	printf("	for(i=12;i<15;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(12,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_B(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(16,26) == 1) return 0;\n");
	printf("	for(i=16;i<27;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(16,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_B1(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(16,18) == 1) return 0;\n");
	printf("	for(i=16;i<19;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(16,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_B2(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(20,22) == 1) return 0;\n");
	printf("	for(i=20;i<23;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(20,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_B3(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(24,26) == 1) return 0;\n");
	printf("	for(i=24;i<27;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(24,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_E(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(28,30) == 1) return 0;\n");
	printf("	for(i=28;i<31;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(28,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_F(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(32,34) == 1) return 0;\n");
	printf("	for(i=32;i<35;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(32,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_C(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(36,46) == 1) return 0;\n");
	printf("	for(i=36;i<47;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(36,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_C1(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(36,40) == 1) return 0;\n");
	printf("	for(i=36;i<41;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(36,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_C2(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(42,46) == 1) return 0;\n");
	printf("	for(i=42;i<47;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(42,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_C3(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(36,38) == 1) return 0;\n");
	printf("	for(i=36;i<39;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(36,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_C4(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(40,42) == 1) return 0;\n");
	printf("	for(i=40;i<43;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(40,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_C5(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(44,46) == 1) return 0;\n");
	printf("	for(i=44;i<47;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(44,2);\n");
	printf("}\n");
	printf("function ButtonPcsBand_G(){\n");
	printf("	var i;\n");
	printf("	if (GroupOverCountCheck(48,50) == 1) return 0;\n");
	printf("	for(i=48;i<51;i++) document.form1.check_pcs_band[i].checked = 1;\n");
	printf("	CheckPcsBand(48,2);\n");
	printf("}\n");
#if 1	/* sprint */
	printf("function ButtonPcsBand_LTE(){\n");
	printf("	if (GroupOverCountCheck(51,51) == 1) return 0;\n");
	printf("	document.form1.check_pcs_band[51].checked = 1;\n");
	printf("	CheckPcsBand(51,2);\n");
	printf("}\n");
#endif

	printf("//--> \n");
	printf("</script>\n");
	printf("\n");

	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("\n");
	printf("</head>\n");
	printf("\n");
	printf("<body bgcolor=\"#FFFFFF\" text=\"#000000\" onLoad = \"reload()\">\n");

//==============================================================
	html_menu_head(&protocol_status, "RF Configuration");
//--------------------------------------------------------------------------------

	printf("<form name = \"form1\" method=\"POST\" action=\"html.cgi\">\n");
	printf("<table cellspacing=\"0\" border=\"0\" width=\"740\">\n");
	printf("	<tr height=\"22\" style=\"height:16.5pt\">\n");
	printf("		<td height=\"50\" width=\"529\" bgcolor=\"%s\" style=\"border-width:2; border-top-style:ridge; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\">\n", service_color[protocol_status.status[amp_id].svc_info]);
	if(protocol_status.status[amp_id].svc_info>7) protocol_status.status[amp_id].svc_info=0;
	if(protocol_status.status[amp_id].svc_info==0)
		sprintf(str, "unknown"); 
	else
		sprintf(str, "%s (model %d)", service_name[protocol_status.status[amp_id].svc_info], protocol_status.status[amp_id].amp_capacity);
	printf("			<p align=\"left\"><font size=\"5\"><b>&nbsp;Configuration - %s</b></font></p>\n", str);
	printf("		</td>\n");
	printf("		<td bgcolor=\"%s\" style=\"border-width:2; border-top-style:ridge; border-right-style:ridge; border-bottom-style:ridge; border-left-style:hidden;\">\n", service_color[protocol_status.status[amp_id].svc_info]);
	printf("			<p align=\"right\">\n");
	printf("				<input type=\"button\" style=\"width:136; name=\"renew\" value=\"Configuration\" onclick=\"location='html.cgi?function=tree_rfconfig'\"></br>\n");
	// add
	if (amp_id==0) strcpy(str, "status");
	else sprintf(str, "status_%d", amp_id);
	printf("				<input type=\"button\" style=\"width:136; name=\"rf_status\" value=\"AMP-%d Status\" onclick=\"location='html.cgi?function=%s'\">\n", amp_id+1, str);
	printf("			</p>\n");
	printf("		</td>\n");
	printf("	</tr>\n");
	printf("	<tr>\n");
	memcpy(str_tmp, protocol_status.status[amp_id].serial_no, 16);
	str_tmp[16]=0;
	sprintf(str, "[AMP-%d] S/N:%s", amp_id+1, str_tmp);
	printf("		<td height=\"30\" bgcolor=\"%s\" style=\"border-width:2; border-top-style:hidden; border-right-style:hidden; border-bottom-style:ridge; border-left-style:ridge;\">\n", service_color[protocol_status.status[amp_id].svc_info]);
	printf("			&nbsp;%s\n", str);
	printf("		</td>\n");

	printf("		<td bgcolor=\"%s\" style=\"border-width:2; border-top-style:hidden; border-right-style:ridge; border-bottom-style:ridge; border-left-style:hidden;\">\n", service_color[protocol_status.status[amp_id].svc_info]);
	printf("			<p align=\"right\"><button type=\"submit\" style=\"width:136;\" onclick=\"onSubmit(); this.form.submit(); this.disabled=true;\">Apply</button></p>\n");
	printf("		</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");

	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");
	printf("	<input name=\"flag_amp_id\" type=\"checkbox\" checked value=\"%d\" style=\"display:none\">\n", amp_id);
	printf("	<input name=\"flag_dl_amp_onoff\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_dl_atten\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_dl_agc_limit\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_dl_band1_comp\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_dl_input_low_limit\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_dl_band2_comp\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_dl_low_output_gap\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_dl_band3_comp\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_ul_amp_onoff\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_ul_atten\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_ul_agc_limit\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
//	printf("	<input name=\"flag_ul_gain\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_agc_onoff\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_temp_limit\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_gain_balance_onoff\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_isolation_det_limit\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_shutdown_onoff\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_gain_balance_value\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_aoc_onoff\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
#if 1	/* sprint */
	printf("	<input name=\"flag_cascade_code\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
#endif
	printf("	<input name=\"flag_alarm_delay\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_band_lte700\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_band_pcs\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_band_cellular\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"flag_band_aws\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
#ifdef iDEN800_FUNC_ADD	/* iden800 */
	printf("	<input name=\"flag_band_iden\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
#endif /* iDEN800_FUNC_ADD */
	printf("\n");
	printf("	<input name=\"hidden_band_pcs_start\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"hidden_band_pcs_start\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"hidden_band_pcs_start\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"hidden_band_pcs_count\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"hidden_band_pcs_count\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"hidden_band_pcs_count\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");

	printf("	<input name=\"hidden_band_lte\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"hidden_band_cellular\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"hidden_band_aws\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"hidden_band_aws\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
	printf("	<input name=\"hidden_band_aws\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
#ifdef iDEN800_FUNC_ADD	/* iden800 */
	printf("	<input name=\"hidden_band_iden\" type=\"checkbox\" checked value=\"0\" style=\"display:none\">\n");
#endif /* iDEN800_FUNC_ADD */
	printf("\n");

	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Downlink </b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30%\" >&nbsp;DL Attenuation</td>\n");
	printf("		<td width=\"20%\" >\n");
	printf("			<select name=\"combo_dl_atten\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_dl_atten)>\n");
for(i=0;i<61;i++){
	printf("				<option value=\"%d\" %s>%.1f</option>\n", i, ((float)i*0.5==protocol_status.status[amp_id].dl_attenuation)?"selected":"", (float)i*0.5);
}
	printf("			</select>&nbsp;dB</td>\n");
	printf("		<td width=\"33%\" >&nbsp;DL ALC Limit</td>\n");
	printf("		<td width=\"17%\" >\n");
	printf("			<select name=\"combo_dl_agc_limit\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_dl_agc_limit)>\n");
for(i=0;i<(protocol_status.status[amp_id].amp_capacity*2)+1;i++){
	printf("				<option value=\"%d\" %s>%.1f</option>\n", i, ((float)i*0.5==protocol_status.status[amp_id].dl_agc_limit)?"selected":"", (float)i*0.5);
}
	printf("			</select>&nbsp;dBm</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30%\" >&nbsp;DL AMP Control</td>\n");
	printf("		<td width=\"20%\" >\n");
	printf("			<select name=\"combo_dl_amp_onoff\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_dl_amp_onoff)>\n");
	printf("				<option value=\"0\" %s>OFF</option>\n", (protocol_status.status[amp_id].dl_amp_onoff)? "": "selected");
	printf("				<option value=\"1\" %s>ON</option>\n", (protocol_status.status[amp_id].dl_amp_onoff)? "selected": "");
	printf("			</select></td>\n");
	printf("		<td width=\"33%\" >&nbsp;DL Low RSSI(-8dB)</td>\n");
	printf("		<td width=\"17%\" >\n");
	printf("			<select name=\"combo_dl_input_low_limit\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_dl_input_low_limit)>\n");
for(i=0;i<73;i++){
	printf("				<option value=\"%d\" %s>%.1f</option>\n", i-93*2, ( ((float)i-93*2)==protocol_status.status[amp_id].dl_input_low_limit*2)?"selected":"", (float)i*0.5-93);
}
	printf("			</select>&nbsp;dBm</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30%\" >&nbsp;DL Low RF Power</td>\n");
	printf("		<td width=\"20%\" >\n");
	printf("			<select name=\"combo_dl_low_output_gap\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_dl_low_output_gap)>\n");
for(i=2;i<11;i++){
	printf("				<option value=\"%d\" %s>%d</option>\n", i, ((float)i==protocol_status.status[amp_id].dl_low_output_gap)?"selected":"", i );
}
	printf("			</select>&nbsp;dB</td>\n");

	char band_open[3];
	char band_name[3][32];
	get_band_open(&protocol_status.status[amp_id], (u8*)band_open, band_name);
/* band 1 */
if(band_open[0]) {
	printf("		<td width=\"33%\" >&nbsp;<font size=\"2\">Band1 Compensation(%s)</font></td>\n", band_name[0]);
	printf("		<td width=\"17%\" >\n");
} else {
	printf("		<td colspan=\"2\" width=\"50%\" >&nbsp;\n");
}
	printf("			<select name=\"combo_dl_band1_comp\" style=\"WIDTH: 50pt;%s\" onChange=CheckCheckObj(form1.flag_dl_band1_comp)>\n", (band_open[0])?"":"display:none;");
for(i=-20;i<7;i++){
	printf("				<option value=\"%d\" %s>%.1f</option>\n", i, ((float)i*0.5==protocol_status.status[amp_id].dl_band_compensation[0])?"selected":"", (float)i*0.5);
}
	printf("			</select>&nbsp;%s</td>\n", (band_open[0])? "dB": "");
	printf("	</tr>\n");
/* band 2 */
	printf("	<tr height=\"25\" %s>\n", (band_open[1])?"":"style=\"display:none\"");
	printf("		<td colspan=\"2\" width=\"50%\">&nbsp;\n");
if(band_open[1]) {
	printf("		<td width=\"33%\" >&nbsp;<font size=\"2\">Band2 Compensation(%s)</font></td>\n", band_name[1]);
	printf("		<td width=\"17%\" >\n");
} else {
	printf("		<td colspan=\"2\" width=\"50%\">&nbsp;\n");
}
	printf("			<select name=\"combo_dl_band2_comp\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_dl_band2_comp)>\n");
for(i=-20;i<7;i++){
	printf("				<option value=\"%d\" %s>%.1f</option>\n", i, ((float)i*0.5==protocol_status.status[amp_id].dl_band_compensation[1])?"selected":"", (float)i*0.5);
}
	printf("			</select>&nbsp;%s</td>\n", (band_open[1])? "dB": "");
	printf("	</tr>\n");
/* band 3 */
	printf("	<tr height=\"25\" %s>\n", (band_open[2])?"":"style=\"display:none\"");
	printf("		<td colspan=\"2\" width=\"50%\">&nbsp;\n");
if(band_open[2]) {
	printf("		<td width=\"33%\" >&nbsp;<font size=\"2\">Band3 Compensation(%s)</font></td>\n", band_name[2]);
	printf("		<td width=\"17%\" >\n");
} else {
	printf("		<td colspan=\"2\" width=\"50%\">&nbsp;\n");
}
	printf("			<select name=\"combo_dl_band3_comp\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_dl_band3_comp)>\n");
for(i=-20;i<7;i++){
	printf("				<option value=\"%d\" %s>%.1f</option>\n", i, ((float)i*0.5==protocol_status.status[amp_id].dl_band_compensation[2])?"selected":"", (float)i*0.5);
}
	printf("			</select>&nbsp;%s</td>\n", (band_open[2])? "dB": "");
	printf("	</tr>\n");
	printf("</table>\n");

	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Uplink </b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30%\" >&nbsp;UL Attenuation</td>\n");
	printf("		<td width=\"20%\" >\n");
	printf("			<select name=\"combo_ul_atten\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_ul_atten)>\n");
for(i=0;i<61;i++){
	printf("				<option value=\"%d\" %s>%.1f</option>\n", i, ((float)i*0.5==protocol_status.status[amp_id].ul_attenuation)?"selected":"", (float)i*0.5);
}
	printf("			</select>&nbsp;dB</td>\n");
	printf("		<td width=\"33%\" >&nbsp;UL ALC Limit</td>\n");
	printf("		<td width=\"17%\" >\n");
	printf("			<select name=\"combo_ul_agc_limit\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_ul_agc_limit)>\n");
for(i=0;i<(protocol_status.status[amp_id].amp_capacity*2)+1;i++){
	printf("				<option value=\"%d\" %s>%.1f</option>\n", i, ((float)i*0.5==protocol_status.status[amp_id].ul_agc_limit)?"selected":"", (float)i*0.5);
}
#if 0
{
	char str[128];
	sprintf(str, "dl_agc_limit[%d]=%f, ul_agc_limit[%d]=%f", amp_id, protocol_status.status[amp_id].dl_agc_limit, amp_id, protocol_status.status[amp_id].ul_agc_limit);
	debuglog(str);
}
#endif
	printf("			</select>&nbsp;dBm</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30%\" >&nbsp;UL AMP Control</td>\n");
	printf("		<td width=\"20%\" >\n");
	printf("			<select name=\"combo_ul_amp_onoff\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_ul_amp_onoff)>\n");
	printf("				<option value=\"0\" %s>OFF</option>\n", (protocol_status.status[amp_id].ul_amp_onoff)? "": "selected");
	printf("				<option value=\"1\" %s>ON</option>\n", (protocol_status.status[amp_id].ul_amp_onoff)? "selected": "");
	printf("			</select></td>\n");
	printf("		<td colspan=\"2\" width=\"50%\" >&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("</table>\n");
	printf("<table width=\"740\" height=\"10\" bgcolor=\"transparent\" border=\"0\"><tr><td> </td></tr></table>\n");
	printf("\n");

	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Common</b></font></td>\n");
	printf("	</tr>\n");
#if 1	/* sprint */
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30%\" >&nbsp;Cascade Code</td>\n");
	printf("		<td colspan=\"3\" width=70% >\n");
	printf("			<input type=\"text\" name=\"txt_cascade_code\" style=\"WIDTH:400; \" maxlength=\"50\" value=\"%s\" onChange=CheckCheckObj(form1.flag_cascade_code)></td>\n", protocol_status.status[amp_id].cascade_code);	//onChange(text = onPropertyChange)
	printf("	</tr>\n");
#endif
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30%\" >&nbsp;Delay Alarm Reporting</td>\n");
	printf("		<td width=\"20%\" >\n");
	printf("			<select name=\"combo_alarm_delay\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_alarm_delay)>\n");
	printf("				<option value=\"0\" %s>0</option>\n", (protocol_status.status[amp_id].alarm_delay)? "": "selected");
	printf("				<option value=\"5\" %s>5</option>\n", (protocol_status.status[amp_id].alarm_delay)? "selected": "");
	printf("			</select>&nbsp;Minutes</td>\n");
	printf("		<td colspan=\"2\" width=\"50%\" >&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30%\" >&nbsp;AMP Temperature Upper Limit</td>\n");
	printf("		<td width=\"20%\" >\n");
	printf("			<select name=\"combo_temp_limit\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_temp_limit)>\n");
for(i=14;i<=210;i++){
	printf("				<option value=\"%d\" %s>%d</option>\n", i, (i==(int)protocol_status.status[amp_id].temperature_limit)?"selected":"", i);
}
	printf("			</select>&nbsp;'F</td>\n");
	printf("		<td width=\"33%\" >&nbsp;Shutdown Control</td>\n");
	printf("		<td width=\"17%\" >\n");
	printf("			<select name=\"combo_shutdown_onoff\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_shutdown_onoff)>\n");
	printf("				<option value=\"0\" %s>OFF</option>\n", (protocol_status.status[amp_id].shutdown_onoff)? "": "selected");
	printf("				<option value=\"1\" %s>ON</option>\n", (protocol_status.status[amp_id].shutdown_onoff)? "selected": "");
	printf("			</select></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30%\" >&nbsp;Gain Balance Value</td>\n");
	printf("		<td width=\"20%\" >\n");
	printf("			<select name=\"combo_gain_balance_value\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_gain_balance_value)>\n");
for(i=0;i<=30;i++){
	printf("				<option value=\"%d\" %s>%.1f</option>\n", i, ((float)i*0.5==protocol_status.status[amp_id].gain_balance_value)?"selected":"", (float)i*0.5);
}
	printf("			</select>&nbsp;dB</td>\n");
	printf("		<td width=\"33%\" >&nbsp;Gain Balance Control</td>\n");
	printf("		<td width=\"17%\" >\n");
	printf("			<select name=\"combo_gain_balance_onoff\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_gain_balance_onoff)>\n");
	printf("				<option value=\"0\" %s>OFF</option>\n", (protocol_status.status[amp_id].gain_balance_onoff)? "": "selected");
	printf("				<option value=\"1\" %s>ON</option>\n", (protocol_status.status[amp_id].gain_balance_onoff)? "selected": "");
	printf("			</select></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"30%\" >&nbsp;AOC(Auto Oscillation Control)</td>\n");
	printf("		<td width=\"20%\" >\n");
	printf("			<select name=\"combo_aoc_onoff\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_aoc_onoff)>\n");
	printf("				<option value=\"0\" %s>OFF</option>\n", (protocol_status.status[amp_id].aoc_onoff)? "": "selected");
	printf("				<option value=\"1\" %s>ON</option>\n", (protocol_status.status[amp_id].aoc_onoff)? "selected": "");
	printf("			</select></td>\n");
#if 0	// alc on/off debug
	printf("		<td width=\"33%\" >&nbsp;ALC Control</td>\n");
	printf("		<td width=\"17%\" >\n");
	printf("			<select name=\"combo_agc_onoff\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_agc_onoff)>\n");
	printf("				<option value=\"0\" %s>OFF</option>\n", (protocol_status.status[amp_id].agc_onoff)? "": "selected");
	printf("				<option value=\"1\" %s>ON</option>\n", (protocol_status.status[amp_id].agc_onoff)? "selected": "");
	printf("			</select></td>\n");
#else
	printf("		<td colspan=\"2\" width=\"50%\" >&nbsp;</td>\n");
#endif
	printf("	</tr>\n");

#if 0	/* sprint */
	printf("	<tr height=\"25\">\n");
	printf("		<td width=26% >&nbsp;Isolation Detect Limit</td>\n");
	printf("		<td width=24% >\n");
	printf("			<select name=\"combo_isolation_det_limit\" style=\"WIDTH: 50pt;\" onChange=CheckCheckObj(form1.flag_isolation_det_limit)>\n");
for(i=95;i<=115;i++){
	printf("				<option value=\"%d\" %s>%d</option>\n", i, (i==(int)protocol_status.status[amp_id].isolation_detect_limit)?"selected":"", i);
}
#if 0
{
	char str[128];
	sprintf(str, "amp_id=%d, isolation_detect_limit=%f", amp_id, protocol_status.status[amp_id].isolation_detect_limit);
	debuglog(str);
}
#endif
	printf("			</select>&nbsp;dB</td>\n");
	printf("	</tr>\n");
#endif

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
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_lte700_la\" value=\"0\" %s onclick=\"CheckLte700Band(value);\">&nbsp;Lower A</td>\n", (lte700_band_check[0])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_lte700_lb\" value=\"1\" %s onclick=\"CheckLte700Band(value);\">&nbsp;Lower B</td>\n", (lte700_band_check[1])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_lte700_lc\" value=\"2\" %s onclick=\"CheckLte700Band(value);\">&nbsp;Lower C</td>\n", (lte700_band_check[2])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_lte700_uc\" value=\"3\" %s onclick=\"CheckLte700Band(value);\">&nbsp;Upper C</td>\n", (lte700_band_check[3])? "checked": "");
	printf("	</tr>\n");
	printf("</table>\n");

//band select - Cellular
	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1 %s>\n", (protocol_status.status[amp_id].svc_info==2)? "": "style=\"display:none\"");
//	printf("<table cellpadding=\"0\" cellspacing=\"0\" width=\"740\" border=1>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td colspan=\"4\" bgcolor=\"#CCCCCC\" align=\"center\"><font size=\"2\"><b>&nbsp; Band Selection</b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_cell_band_a1\" value=\"0\" %s onclick=\"CheckCellularBand(value);\">&nbsp;A1</td>\n", (cellular_band_check[0])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_cell_band_b1\" value=\"1\" %s onclick=\"CheckCellularBand(value);\">&nbsp;B1</td>\n", (cellular_band_check[1])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_cell_band_a2\" value=\"2\" %s onclick=\"CheckCellularBand(value);\">&nbsp;A2</td>\n", (cellular_band_check[2])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_cell_band_b2\" value=\"3\" %s onclick=\"CheckCellularBand(value);\">&nbsp;B2</td>\n", (cellular_band_check[3])? "checked": "");
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
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band\" value=\"9\" %s onclick=\"CheckiDENBand(value);\">&nbsp;6.6M</td>\n", (iden_band_check[8])? "checked": "");	// 6.6M
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band\" value=\"8\" %s onclick=\"CheckiDENBand(value);\">&nbsp;6.8M</td>\n", (iden_band_check[7])? "checked": "");	// 6.8M
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band\" value=\"7\" %s onclick=\"CheckiDENBand(value);\">&nbsp;7M</td>\n", (iden_band_check[6])? "checked": "");	// 7M
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band\" value=\"6\" %s onclick=\"CheckiDENBand(value);\">&nbsp;14.6M</td>\n", (iden_band_check[5])? "checked": "");	// 14.6M
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band\" value=\"5\" %s onclick=\"CheckiDENBand(value);\">&nbsp;14.8M</td>\n", (iden_band_check[4])? "checked": "");	// 14.8M
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band\" value=\"4\" %s onclick=\"CheckiDENBand(value);\">&nbsp;15M</td>\n", (iden_band_check[3])? "checked": "");	// 15M
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band\" value=\"3\" %s onclick=\"CheckiDENBand(value);\">&nbsp;17.6M</td>\n", (iden_band_check[2])? "checked": "");	// 17.6M
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band\" value=\"2\" %s onclick=\"CheckiDENBand(value);\">&nbsp;17.8M</td>\n", (iden_band_check[1])? "checked": "");	// 17.8M
	printf("		<td width=\"20\%\" align=\"center\"><input type=\"radio\" name=\"check_iden_band\" value=\"1\" %s onclick=\"CheckiDENBand(value);\">&nbsp;18M</td>\n", (iden_band_check[0])? "checked": "");	// 18M
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
	printf("	<tr height=\"4\"><td colspan=\"16\" bgcolor=\"#ffffff\"></td></tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td colspan=\"16\">\n");
	printf("			<button type=\"button\" style=\"background-color=#ffffbb; width:100%;height=100%\" onclick=\"ButtonPcsBand_Clear()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">Clear All Band</font></button></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"4\"><td colspan=\"16\" bgcolor=\"#ffffff\"></td></tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td colspan=\"11\">\n");
	printf("			<button type=\"button\" style=\"background-color=#6ff448;width:100%;height=100%\" onclick=\"ButtonPcsBand_A()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\"size=\"2\">A Band</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#a346ff;width:100%;height=100%\" onclick=\"ButtonPcsBand_D()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">D Band</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#a0f887;width:100%;height=100%\" onclick=\"ButtonPcsBand_A1()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">A1</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#6ff448;width:100%;height=100%\" onclick=\"ButtonPcsBand_A2()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">A2</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#2eab0a;width:100%;height=100%\" onclick=\"ButtonPcsBand_A3()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">A3</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#a346ff;width:100%;height=100%\" onclick=\"ButtonPcsBand_D()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">D</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#a0f887\" width=\"45\" ><font size=\"2\">25 </font></td>\n");
	printf("		<td bgcolor=\"#a0f887\" width=\"45\" ><font size=\"2\">50 </font></td>\n");
	printf("		<td bgcolor=\"#a0f887\" width=\"45\" ><font size=\"2\">75 </font></td>\n");
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
	printf("		<td bgcolor=\"#a0f887\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"0\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[0])? "checked": "");//[0],25
	printf("		<td bgcolor=\"#a0f887\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"1\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[1])? "checked": "");//[1],50
	printf("		<td bgcolor=\"#a0f887\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"2\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[2])? "checked": "");//[2],75
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"3\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[3])? "checked": "");//[3],100
	printf("		<td bgcolor=\"#6ff448\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"4\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[4])? "checked": "");//[4],125
	printf("		<td bgcolor=\"#6ff448\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"5\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[5])? "checked": "");//[5],150
	printf("		<td bgcolor=\"#6ff448\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"6\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[6])? "checked": "");//[6],175
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"7\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[7])? "checked": "");//[7],200
	printf("		<td bgcolor=\"#2eab0a\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"8\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[8])? "checked": "");//[8],225
	printf("		<td bgcolor=\"#2eab0a\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"9\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[9])? "checked": "");//[9],250
	printf("		<td bgcolor=\"#2eab0a\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"10\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[10])? "checked": "");//[10],275
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"11\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[11])? "checked": "");//[11],300
	printf("		<td bgcolor=\"#a346ff\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"12\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[12])? "checked": "");//[12],325
	printf("		<td bgcolor=\"#a346ff\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"13\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[13])? "checked": "");//[13],350
	printf("		<td bgcolor=\"#a346ff\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"14\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[14])? "checked": "");//[14],375
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"15\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[15])? "checked": "");//[15],400
	printf("	</tr>\n");
	printf("	<tr height=\"4\"><td colspan=\"16\" bgcolor=\"#ffffff\"></td></tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td colspan=\"11\">\n");
	printf("			<button type=\"button\" style=\"background-color=#747ee0;width:100%;height=100%\" onclick=\"ButtonPcsBand_B()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">B Band</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#ff80c0;width:100%;height=100%\" onclick=\"ButtonPcsBand_E()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">E Band</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#a3aaeb;width:100%;height=100%\" onclick=\"ButtonPcsBand_B1()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">B1</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#747ee0;width:100%;height=100%\" onclick=\"ButtonPcsBand_B2()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">B2</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#4755d6;width:100%;height=100%\" onclick=\"ButtonPcsBand_B3()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">B3</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#ff80c0;width:100%;height=100%\" onclick=\"ButtonPcsBand_E()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">E</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\">\n");
	printf("		<td bgcolor=\"#a3aaeb\" width=\"45\" align=\"center\" ><font size=\"2\">425</font></td>\n");
	printf("		<td bgcolor=\"#a3aaeb\" width=\"45\" align=\"center\" ><font size=\"2\">450</font></td>\n");
	printf("		<td bgcolor=\"#a3aaeb\" width=\"45\" align=\"center\" ><font size=\"2\">475</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" align=\"center\" ><font size=\"2\">500</font></td>\n");
	printf("		<td bgcolor=\"#747ee0\" width=\"45\" align=\"center\" ><font size=\"2\">525</font></td>\n");
	printf("		<td bgcolor=\"#747ee0\" width=\"45\" align=\"center\" ><font size=\"2\">550</font></td>\n");
	printf("		<td bgcolor=\"#747ee0\" width=\"45\" align=\"center\" ><font size=\"2\">575</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" align=\"center\" ><font size=\"2\">600</font></td>\n");
	printf("		<td bgcolor=\"#4755d6\" width=\"45\" align=\"center\" ><font size=\"2\">625</font></td>\n");
	printf("		<td bgcolor=\"#4755d6\" width=\"45\" align=\"center\" ><font size=\"2\">650</font></td>\n");
	printf("		<td bgcolor=\"#4755d6\" width=\"45\" align=\"center\" ><font size=\"2\">675</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" align=\"center\" ><font size=\"2\">700</font></td>\n");
	printf("		<td bgcolor=\"#ff80c0\" width=\"45\" align=\"center\" ><font size=\"2\">725</font></td>\n");
	printf("		<td bgcolor=\"#ff80c0\" width=\"45\" align=\"center\" ><font size=\"2\">750</font></td>\n");
	printf("		<td bgcolor=\"#ff80c0\" width=\"45\" align=\"center\" ><font size=\"2\">775</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\" align=\"center\" ><font size=\"2\">800</font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#a3aaeb\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"16\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[16])? "checked": "");//[16],425
	printf("		<td bgcolor=\"#a3aaeb\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"17\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[17])? "checked": "");//[17],450
	printf("		<td bgcolor=\"#a3aaeb\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"18\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[18])? "checked": "");//[18],475
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"19\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[19])? "checked": "");//[19],500
	printf("		<td bgcolor=\"#747ee0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"20\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[20])? "checked": "");//[20],525
	printf("		<td bgcolor=\"#747ee0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"21\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[21])? "checked": "");//[21],550
	printf("		<td bgcolor=\"#747ee0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"22\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[22])? "checked": "");//[22],575
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"23\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[23])? "checked": "");//[23],600
	printf("		<td bgcolor=\"#4755d6\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"24\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[24])? "checked": "");//[24],625
	printf("		<td bgcolor=\"#4755d6\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"25\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[25])? "checked": "");//[25],650
	printf("		<td bgcolor=\"#4755d6\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"26\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[26])? "checked": "");//[26],675
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"27\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[27])? "checked": "");//[27],700
	printf("		<td bgcolor=\"#ff80c0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"28\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[28])? "checked": "");//[28],725
	printf("		<td bgcolor=\"#ff80c0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"29\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[29])? "checked": "");//[29],750
	printf("		<td bgcolor=\"#ff80c0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"30\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[30])? "checked": "");//[30],775
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"31\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[31])? "checked": "");//[31],800
	printf("	</tr>\n");
	printf("	<tr height=\"4\"><td colspan=\"16\" bgcolor=\"#ffffff\"></td></tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td colspan=\"3\">&nbsp;</td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"11\">\n");
	printf("			<button type=\"button\" style=\"background-color=#ffac59;width:100%;height=100%\" onclick=\"ButtonPcsBand_C()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">C Band</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#80ffff;width:100%;height=100%\" onclick=\"ButtonPcsBand_F()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">F Band</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"5\">\n");
	printf("			<button type=\"button\" style=\"background-color=#ffca95;width:100%;height=100%\" onclick=\"ButtonPcsBand_C1()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">C1</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"5\">\n");
	printf("			<button type=\"button\" style=\"background-color=#ff8102;width:100%;height=100%;\" onclick=\"ButtonPcsBand_C2()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">C2</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#80ffff;width:100%;height=100%\" onclick=\"ButtonPcsBand_F()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">F</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#ffca95;width:100%;height=100%\" onclick=\"ButtonPcsBand_C3()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">C3</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#ffa64d;width:100%;height=100%\" onclick=\"ButtonPcsBand_C4()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">C4</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#ff8102;width:100%;height=100%;\" onclick=\"ButtonPcsBand_C5()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">C5</font></button></td>\n");
	printf("		<td>&nbsp;</td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#80ffff\" width=\"45\"><font size=\"2\">825 </font></td>\n");
	printf("		<td bgcolor=\"#80ffff\" width=\"45\"><font size=\"2\">850 </font></td>\n");
	printf("		<td bgcolor=\"#80ffff\" width=\"45\"><font size=\"2\">875 </font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\"><font size=\"2\">900 </font></td>\n");
	printf("		<td bgcolor=\"#ffca95\" width=\"45\"><font size=\"2\">925 </font></td>\n");
	printf("		<td bgcolor=\"#ffca95\" width=\"45\"><font size=\"2\">950 </font></td>\n");
	printf("		<td bgcolor=\"#ffca95\" width=\"45\"><font size=\"2\">975 </font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\"><font size=\"2\">1000</font></td>\n");
	printf("		<td bgcolor=\"#ffa64d\" width=\"45\"><font size=\"2\">1025</font></td>\n");
	printf("		<td bgcolor=\"#ffa64d\" width=\"45\"><font size=\"2\">1050</font></td>\n");
	printf("		<td bgcolor=\"#ffa64d\" width=\"45\"><font size=\"2\">1075</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\"><font size=\"2\">1100</font></td>\n");
	printf("		<td bgcolor=\"#ff8102\" width=\"45\"><font size=\"2\">1125</font></td>\n");
	printf("		<td bgcolor=\"#ff8102\" width=\"45\"><font size=\"2\">1150</font></td>\n");
	printf("		<td bgcolor=\"#ff8102\" width=\"45\"><font size=\"2\">1175</font></td>\n");
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\"><font size=\"2\">1200</font></td>\n");
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#80ffff\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"32\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[32])? "checked": "");//[32],825
	printf("		<td bgcolor=\"#80ffff\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"33\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[33])? "checked": "");//[33],850
	printf("		<td bgcolor=\"#80ffff\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"34\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[34])? "checked": "");//[34],875
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"35\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[35])? "checked": "");//[35],900
	printf("		<td bgcolor=\"#ffca95\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"36\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[36])? "checked": "");//[36],925
	printf("		<td bgcolor=\"#ffca95\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"37\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[37])? "checked": "");//[37],950
	printf("		<td bgcolor=\"#ffca95\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"38\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[38])? "checked": "");//[38],975
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"39\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[39])? "checked": "");//[39],1000
	printf("		<td bgcolor=\"#ffa64d\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"40\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[40])? "checked": "");//[40],1025
	printf("		<td bgcolor=\"#ffa64d\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"41\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[41])? "checked": "");//[41],1050
	printf("		<td bgcolor=\"#ffa64d\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"42\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[42])? "checked": "");//[42],1075
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"43\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[43])? "checked": "");//[43],1100
	printf("		<td bgcolor=\"#ff8102\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"44\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[44])? "checked": "");//[44],1125
	printf("		<td bgcolor=\"#ff8102\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"45\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[45])? "checked": "");//[45],1150
	printf("		<td bgcolor=\"#ff8102\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"46\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[46])? "checked": "");//[46],1175
	printf("		<td bgcolor=\"#e0e0e0\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"47\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[47])? "checked": "");//[47],1200
	printf("	</tr>\n");
	printf("	<tr height=\"4\"><td colspan=\"16\" bgcolor=\"#ffffff\"></td></tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#ff0080;width:100%;height=100%\" onclick=\"ButtonPcsBand_G()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">G Band</font></button></td>\n");
#if 0	/* sprint */
	printf("		<td width=\"45\" colspan=\"13\">&nbsp;</td>\n");
#else
	printf("		<td colspan=\"4\">\n");
	printf("			<button type=\"button\" style=\"background-color=#00e372;width:100%;height=100%\" onclick=\"ButtonPcsBand_LTE()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">LTE Band</font></button></td>\n");
	printf("		<td colspan=\"9\">&nbsp;</td>\n");
#endif
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td colspan=\"3\">\n");
	printf("			<button type=\"button\" style=\"background-color=#ff0080;width:100%;height=100%\" onclick=\"ButtonPcsBand_G()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">G</font></button></td>\n");
#if 0	/* sprint */
	printf("		<td colspan=\"13\">&nbsp;</td>\n");
#else
	printf("		<td colspan=\"4\">\n");
	printf("			<button type=\"button\" style=\"background-color=#00e372;width:100%;height=100%\" onclick=\"ButtonPcsBand_LTE()\">\n");
	printf("			<font face=\"Arial, Helvetica, sans-serif\" size=\"2\">LTE</font></button></td>\n");
	printf("		<td colspan=\"9\">&nbsp;</td>\n");
#endif
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#ff0080\" width=\"45\"><font size=\"2\">1225</font></td>\n");
	printf("		<td bgcolor=\"#ff0080\" width=\"45\"><font size=\"2\">1250</font></td>\n");
	printf("		<td bgcolor=\"#ff0080\" width=\"45\"><font size=\"2\">1275</font></td>\n");
#if 0	/* sprint */
	printf("		<td colspan=\"13\">&nbsp;</td>\n");
#else
	printf("		<td colspan=\"4\" bgcolor=\"#00e372\"><font size=\"2\">1250</font></td>\n");
	printf("		<td colspan=\"9\">&nbsp;</td>\n");
#endif
	printf("	</tr>\n");
	printf("	<tr height=\"25\" align=\"center\">\n");
	printf("		<td bgcolor=\"#ff0080\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"48\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[48])? "checked": "");//[48],1225
	printf("		<td bgcolor=\"#ff0080\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"49\" %s onclick=\"CheckPcsBand(value,1);\"></td>\n", (pcs_band_check[49])? "checked": "");//[49],1250
	printf("		<td bgcolor=\"#ff0080\" width=\"45\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"50\" %s onclick=\"CheckPcsBand(value,1);\" ></td>\n", (pcs_band_check[50])? "checked": "");//[50],1275
#if 0	/* sprint */
	printf("		<td colspan=\"13\">&nbsp;</td>\n");
#else
	printf("		<td colspan=\"4\" bgcolor=\"#00e372\">\n");
	printf("			<input type=\"checkbox\" name=\"check_pcs_band\" value=\"51\" %s onclick=\"CheckPcsBand(value,1);\" ></td>\n", (pcs_band_check[51])? "checked": "");//[51],1250
	printf("		<td colspan=\"9\">&nbsp;</td>\n");
#endif
	printf("	</tr>\n");
	printf("</table>\n");

//band select - AWS
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
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_a1\"  value= \"0\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[0])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_a1_\" value= \"1\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[1])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_a2\"  value= \"2\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[2])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_a2_\" value= \"3\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[3])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_b1\"  value= \"4\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[4])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_b1_\" value= \"5\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[5])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_b2\"  value= \"6\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[6])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_b2_\" value= \"7\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[7])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_c\"   value= \"8\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[8])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_c_\"  value= \"9\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[9])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_d\"   value=\"10\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[10])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_d_\"  value=\"11\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[11])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_e\"   value=\"12\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[12])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_e_\"  value=\"13\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[13])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_f1\"  value=\"14\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[14])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_f1_\" value=\"15\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[15])? "checked": "");
	printf("		<td align=\"center\"><input type=\"checkbox\" name=\"check_aws_f2\"  value=\"16\" %s onclick=\"CheckAwsBand(value);\"></td>\n", (aws_band_check[16])? "checked": "");
	printf("	</tr>\n");
	printf("</table>\n");

	printf("</form>\n");
//===============================
	printf("</body>\n");
	printf("</html>\n");
}
