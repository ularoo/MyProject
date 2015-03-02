#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include "main.h"
#include "protocol.h"
#include "debug.h"
#include "multipart.h"
#include "crcmodel.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#include "cgiutil.h"
#include <sched.h>

struct cookie cookie[16];
//extern struct post post[10];
extern struct get get[10];
struct user_buffer user_buffer[30];
extern char** environ;

extern const char* service_name[];

struct alarm_buffer
{
	char name[128];
};

struct temp_buffer
{
	char name[128];
};

#define   CRC_16_SEED		0x0000
#define   CRC_TAB_SIZE		256
#define CRC_16_POLYNOMIAL 	0x1021
const unsigned short Crc16_Table[CRC_TAB_SIZE] =
{
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

unsigned short crc(unsigned char* start, unsigned char* end)
{
	unsigned short data; 
	unsigned short crc_16;
	unsigned char* buf_ptr;
	int len;

	buf_ptr = start;
	len = end - start;

	len *= 8;
	for(crc_16 = CRC_16_SEED; len >= 8; len -= 8, buf_ptr++)
	{
		crc_16 = Crc16_Table[(crc_16 >> 8) ^ *buf_ptr]^(crc_16 << 8);
	}

	if(len != 0)
	{
		data = ((unsigned short)(*buf_ptr))<<(8);
		while(len--!=0){
			if( ((crc_16^data)&((unsigned short)1<<15)) != 0)
			{
				crc_16 <<= 1;
				crc_16 ^= CRC_16_POLYNOMIAL;
			}
			else 
				crc_16 <<= 1;
			data <<=1;
		}
	}
	return (crc_16);
}


int debuglog(char *str)
{
#ifdef ENABLE_LOG_DEBUG_MESSAGE
	FILE* file = NULL;
	struct tm *t;
	time_t curr_time;
	char datetime[64]="";

	time(&curr_time);
	t=localtime(&curr_time);
	sprintf(datetime,"%d/%02d/%02d-%02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	file = fopen("/var/tmp/debuglog.txt", "a");
	if(file)
	{
		fprintf(file, "%s: %s\n", datetime, str);
		fclose(file);
	}
#endif
	return 0;
}

int upgrade()
{
	struct protocol_web_status protocolstatus;
	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;
	unsigned char frame[sizeof(protocolstatus)*2];
	int frame_size;

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

	//    bzero(&serveraddr, sizeof(serveraddr));
	bzero(&clientaddr, sizeof(clientaddr));
	clientaddr.sun_family = AF_UNIX;
	strcpy(clientaddr.sun_path, "/tmp/socket_web");
	remove("/tmp/socket_web");

	if(bind(sockfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr)))
	{
		message_dialog("socket error");
		exit(0);
	}
	memset(&protocolstatus, 0, sizeof(protocolstatus));
	protocolstatus.type = PROTOCOL_DOWNLOAD;
	protocol_buffer_to_frame((void*)&protocolstatus, 1,frame, &frame_size);
	sched_yield();

	logs_write(LOG_SYSTEM_DOWNLOAD_SET);

	sendto(sockfd, frame, 3,0,(struct sockaddr*)&serveraddr, client);
	close(sockfd);

	return 0;
}
#if 1	/* fpga download */
int upgrade_fpga(char slot, char svc)
{
	//struct protocol_web_status protocolstatus;
	struct protocol_fpga_download protocol_fpga_download;
	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;
	unsigned char frame[sizeof(protocol_fpga_download)*2];
	int frame_size;

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

	memset(&protocol_fpga_download, 0, sizeof(protocol_fpga_download));

	protocol_fpga_download.type=PROTOCOL_DOWNLOAD_FPGA;

	protocol_fpga_download.slot = slot;
	protocol_fpga_download.service = svc;

	protocol_fpga_download.crc = crc((void*)&protocol_fpga_download.type, (void*)&protocol_fpga_download.crc);
	protocol_buffer_to_frame((void*)&protocol_fpga_download, sizeof(protocol_fpga_download), frame, &frame_size);
	sched_yield();

	logs_write(LOG_SYSTEM_DOWNLOAD_SET);

	sendto(sockfd, frame, frame_size, 0, (struct sockaddr*)&serveraddr, client);
	close(sockfd);

	return 0;
}
#endif

#if 1
void upload(void)
{
	FILE* log = NULL;
	struct input input;
	struct multipart multipart;
	struct content_type type;
	char* value;

	char buffer[256];
	char* filename = buffer;
	int ie_check = 0;      // Internet Explorer version check

	input_initial(&input);
	multipart_initial(&multipart);
	content_type_initial(&type);

	if((value = getenv("REQUEST_METHOD")) && !strcmp("POST", value))
	{
		if((value = getenv("CONTENT_TYPE")))
		{
			input.buffer_size = strlen("Content-Type: ") + strlen(value);
			input.buffer = malloc(input.buffer_size + 1);
			sprintf((char*)input.buffer, "Content-Type: %s", value);
			if(0 < content_type_parse(NULL, &type, &input))
			boundary = content_type_value(&type, "boundary");
		}
	}

	if(boundary)
	{
		input.file = NULL;
		input_final(&input);
		input_initial(&input);
		input.file = stdin;

		if(0 < multipart_parse(NULL, &multipart, &input))
		{
			int i;
			for(i = 0; i < multipart.form_data_size; i++)
			{
				struct form_data* form_data = multipart.form_data + i;
				if(!strcmp("file", form_data_value(form_data, "name")))
				{
					FILE* file = NULL;
					char path[256];
					int nwrite;
					int n;
					sprintf(filename, "%s", form_data_value(form_data, "filename"));
					for(filename += strlen(filename); filename >= buffer; filename--)
					{
						if(*filename == '\\')
						{
							ie_check = 1;
							filename++;
							break;
						}
					}

					if(!ie_check)					
					{
						filename =  buffer;
					}

//					system("/bin/rm /var/ftp/* > /dev/null 2>&1");
//					sprintf(path, "/var/ftp/%s", filename);
					system("/bin/rm /dev/shm/* > /dev/null 2>&1");
					sprintf(path, "/dev/shm/%s", filename);
					if((file = fopen(path, "w")))
					{
						for(nwrite = 0; nwrite < form_data->content_size; nwrite += n)
							if(0 >= (n = fwrite(form_data->content + nwrite, 1, form_data->content_size - nwrite, file)))
								break;
						if(file)
							fclose(file);
					}
				}
			}
			sleep(1);
		}
	}
	goto RETURN;

RETURN:
	content_type_final(&type);
	multipart_final(&multipart);
	input.file = NULL;
	input_final(&input);
	if(log) fclose(log);

#if 0
	if(!strncasecmp(filename, "smc_v", 5)) 
#else
	if((!strncasecmp(filename, "smc_sprint_v", 12)) || (!strncasecmp(filename, "dfm_", 4)))
#endif
		dummy(DOWNLOAD_PAGE);
	else if(!strncasecmp(filename, "transfer", 8)) 
		dummy(TRANSFER_UPLOAD_PAGE);
	else 
		dummy(WRONG_FILE);

	return;
}
#else
int upload(void)
{
    FILE* log = NULL;
    struct input input;
    struct multipart multipart;
    struct content_type type;
    //char** p;
    char* value;

    input_initial(&input);
    multipart_initial(&multipart);
    content_type_initial(&type);

    if((value = getenv("REQUEST_METHOD")) && !strcmp("POST", value))
        if((value = getenv("CONTENT_TYPE")))
        {
            input.buffer_size = strlen("Content-Type: ") + strlen(value);
            input.buffer = malloc(input.buffer_size + 1);
            sprintf((char*)input.buffer, "Content-Type: %s", value);
            if(0 < content_type_parse(NULL, &type, &input))
                boundary = content_type_value(&type, "boundary");
        }
    if(boundary)
    {
        input.file = NULL;
        input_final(&input);
        input_initial(&input);
        input.file = stdin;

        if(0 < multipart_parse(NULL, &multipart, &input))
        {
            int i;
            for(i = 0; i < multipart.form_data_size; i++)
            {
                struct form_data* form_data = multipart.form_data + i;
                if(!strcmp("file", form_data_value(form_data, "name")))
                {
                    FILE* file = NULL;
                    char path[256];
                    int nwrite;
                    int n;
                    char buffer[256];
                    char* filename = buffer;
                    sprintf(filename, "%s", form_data_value(form_data, "filename"));
                    for(filename += strlen(filename); filename >= buffer; filename--)
                        if(*filename == '\\')
                        {
                            filename++;
                            break;
                        }
                    system("/bin/rm /dev/shm/* > /dev/null 2>&1");
                    sprintf(path, "/dev/shm/%s", filename);
                    if((file = fopen(path, "w")))
                    {
                        for(nwrite = 0; nwrite < form_data->content_size; nwrite += n)
                            if(0 >= (n = fwrite(form_data->content + nwrite, 1, form_data->content_size - nwrite, file)))
                                break;
                        if(file)
                            fclose(file);
                    }
                }
            }
            sleep(1);
        }
    }
    
    goto RETURN;
RETURN:
    content_type_final(&type);
    multipart_final(&multipart);
    input.file = NULL;
    input_final(&input);
    if(log)
        fclose(log);
    return 1;
}

#endif



void button_login_login(char* value1, char* value2)
{
	FILE* fp=NULL;
	char* line = NULL;
	char buf[32]="";
	size_t len = 0;
	ssize_t read;

	sprintf(buf,"%s=%s",value1,value2);

	fp=fopen("/usr/local/etc/manager","r");
//===============================================================================
//===============================================================================
// Tracking Issue #129 Setup Wizard
/*
	if(fp)
	{
		while((read = getline(&line, &len, fp)) != -1)
		{
			if(strlen(buf) == (read-3))
			{
				if(strncmp(buf,line,read-3)==0)
				{
					setcookie(value1);
					list();
					logs_write(LOG_LOGIN_CHECK);
					break;
				}
			}
		}
		if((strlen(buf) != (read-3)) && (strncmp(buf,line,read-3) != 0))
		{
			message_dialog("The username and/or password you entered is incorrect. Please try again");
		}
	}
*/
	if(fp)
	{
		while((read = getline(&line, &len, fp)) != -1)
		{
			if(strlen(buf) == (read-3))
			{
				if(strncmp(buf,line,read-3)==0)
				{
					setcookie(value1);
//===============================================================================
//===============================================================================
// Tracking Issue #150 Setup Wizard
/*
					install_wizard_select();
*/
					setup_wizard_step3();
//===============================================================================
//===============================================================================
					logs_write(LOG_LOGIN_CHECK);
					break;
				}
			}
		}
		if((strlen(buf) != (read-3)) && (strncmp(buf,line,read-3) != 0))
		{
			message_dialog("The username and/or password you entered is incorrect. Please try again");
		}
	}
	else login();
//===============================================================================
//===============================================================================

	if(line) free(line);
	if(fp) fclose(fp);
	system("/bin/rm /var/ftp/* > /dev/null 2>&1");
	system("/bin/rm /var/tmp/esn_number > /dev/null 2>&1");
	system("/bin/rm /var/tmp/spc_number > /dev/null 2>&1");
	system("/bin/rm /var/ftp/min_number > /dev/null 2>&1");
}

void message_dialog(char* message)
{
	printf("%s", "Content-type: text/html\r\n\r\n");
	printf("%s", "<html>\n");
	printf("%s", "<head>\n");
	printf("%s", "<title>Sprint SmartCell</title>\n");
	printf("%s", "\n");
	printf("%s", "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("%s", "\n");
	printf("%s", "</head>\n");
	printf("%s", "\n");
	printf("%s", "<body bgcolor=\"#FFFFFF\" text=\"#000000\">\n");
	printf("%s", message);
	printf("%s", "</body>\n");
	printf("%s", "</html>\n");
}


void setcookie(char* value1)
{
	char random_value[36] ="abcdefghijklmnopqrstuvwxyz0123456789";
	char return_setcookie[32]="";
	int n=0,i=0;
	char buf[9]="";
	FILE* fp=NULL;

	return_setcookie[0] = '\0';

	srand(time(NULL));
	for(i=0; i<8; i++)
	{
		n=rand()%36;
		buf[i]=random_value[n];
	}

	sprintf(return_setcookie,"%s=%s",value1,buf);

	fp=fopen("/var/tmp/session","w");
	if(fp)
	{
		fprintf(fp,"%s\n",return_setcookie);
		fclose(fp);
	}

	printf("Set-Cookie:%s;",return_setcookie);
}

int authentication()
{
	FILE* session=NULL;
	FILE* manager=NULL;
	size_t len=0;
	char* line=NULL; /* must be freed */
	ssize_t read;
	int i;
	char* pCookie = NULL;
	int return_value=0;
	char* compare=malloc(1);
	int a=0;
	compare[0] = '\0';	

	if(getenv("HTTP_COOKIE"))
	{
		if(!(pCookie=strdup(getenv("HTTP_COOKIE"))))
		{ 
			message_dialog((char*)__FUNCTION__); 
			exit(-1); 
		}

		session=fopen("/var/tmp/session","r");
		manager=fopen("/usr/local/etc/manager","r");
		if(session)
		{
			while((read=getline(&line,&len,session)) != -1)
			{
				for(i=0; i<16; i++)
				{
					getword(cookie[i].value,pCookie,';');
					if(strncmp(cookie[i].value,line,read-1)==0)
					{
						return_value=1;
						if(!(compare=(char*)realloc(compare, strlen(cookie[i].value) + 1)))
						{ 
							message_dialog((char*)__FUNCTION__); 
							exit(-1); 
						}
						strcpy(compare,cookie[i].value);
						break;
					}
				}
			}
		}
		memset(cookie[0].value,'\0',sizeof(cookie[0].value));
		getword(cookie[0].value,compare,'=');

		if(manager)
		{
			while((read=getline(&line,&len,manager)) != -1)	
			{
				getword(cookie[0].name,line,'=');

				if(strcmp(cookie[0].name,cookie[0].value)==0)
				{
					getword(cookie[1].name,line,':');
					a=line[0]-'0';
					if(a==0)
					{
						a=100;
					}
					else if(a==1)
					{
						a=200;
					}
					return_value+=a;
					break;
				}
			}
		}
	}

	if(line) free(line);
	if(session) fclose(session);
	if(manager) fclose(manager);
	if(pCookie) free(pCookie);	
	if(compare) free(compare);
	return return_value;
}

int button_register_usermanagement(char* user, char* password, char* passconfirm, char* authority)
{
	FILE* manager=NULL;
	ssize_t read;
	size_t len=0;
	char* line=NULL;
	int return_value=0;
	manager=fopen("/usr/local/etc/manager","a+");

	if(!manager)
	{
		message_dialog("File Error,restart ");
		exit(0);
	}

	if(manager)
	{
		while((read=getline(&line,&len,manager))!= -1)
		{
			getword(get[0].name,line,'=');
			if(strcmp(get[0].name,user)==0)
			{
				return_value=1;
				break;	
			}
		}

		if(strcmp(password,passconfirm)!=0)
			return_value+=1;

		if((strlen(user)<5) || (strlen(password)<5))
			return_value+=1;

		if(return_value==0)
		{
			fprintf(manager,"%s=%s:%s\n",user,password,authority);
		}
	}
	if(manager) fclose(manager);

	logs_write(LOG_USER_INSERT);

	if(line) free(line);
	return return_value;
}

void getuser()
{
	FILE* manager=NULL;
	ssize_t read;
	size_t len=0;
	char* line=NULL;

	manager=fopen("/usr/local/etc/manager","r");
	if(manager)
	{
		while((read=getline(&line,&len,manager)) != -1)
		{
			getword(get[0].name,line,'=');
			printf("<option>%s</option>",get[0].name);
		}
	}
	if(manager) fclose(manager);
	if(line) free(line);
}

void showcookie()
{
	printf("%s","function showcookie() \n");
	printf("%s","{ \n");
	printf("%s","alert(document.cookie); \n");
	printf("%s","} \n");
}

void button_delete_usermanagement(char* name)
{
	FILE* manager=NULL;
	ssize_t read;
	size_t len=0;
	char* line=NULL;
	char buf[32]="";
	int i;
	int n=0;
	char* tmp=NULL;
	int sockfd;
    int client;
    struct sockaddr_un serveraddr;
    struct sockaddr_un clientaddr;
	struct protocol_usermanagement protocol_usermanagement;
	unsigned char frame[sizeof(protocol_usermanagement)*2];
	int frame_size;
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

//    bzero(&serveraddr, sizeof(serveraddr));
	bzero(&clientaddr, sizeof(clientaddr));
    clientaddr.sun_family = AF_UNIX;
    strcpy(clientaddr.sun_path, "/tmp/socket_web");
    remove("/tmp/socket_web");

    if(bind(sockfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr)))
    {
        message_dialog("socket error");
        exit(0);
    }
	memset(&protocol_usermanagement, 0, sizeof(protocol_usermanagement));
	protocol_usermanagement.Type=PROTOCOL_USERMANAGEMENT;
	protocol_usermanagement.UserAccounts=1;

	manager=fopen("/usr/local/etc/manager","r");

	if(manager)
	{
		while((read=getline(&line,&len,manager)) != -1)
		{
			tmp=strdup(line);
			getword(buf,tmp,'=');
			free(tmp);
			if(strcmp(name,buf)!=0)
				sprintf(user_buffer[n++].name, "%s",line);
		}

		if(manager) fclose(manager);

		manager=fopen("/usr/local/etc/manager","w");
		if(manager)
		{
			for(i=0; i<n; i++)
				fprintf(manager,"%s",user_buffer[i].name);
		}
	}
	if(manager) fclose(manager);

	protocol_buffer_to_frame((void*)&protocol_usermanagement,2,frame, &frame_size);
	sched_yield();
	sendto(sockfd, frame, 4,0,(struct sockaddr*)&serveraddr, client);

	close(sockfd);
	logs_write(LOG_USER_DELETE);

	if(line) free(line);
	usermanagement();
}


void button_apply_rfconfiguration(char* post_value)
{
	char buf[256]="";
	int sockfd;
	int client;

	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;
	struct protocol_web_control protocol_control;
	unsigned char frame[sizeof(protocol_control)*2];
	int frame_size;
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

	memset(&protocol_control, 0, sizeof(protocol_control));
	protocol_control.type = PROTOCOL_CONTROL;

	getword(buf,post_value,'=');	protocol_control.amp_id	= atoi(buf);

	getword(buf,post_value,'=');	protocol_control.flag.dl_amp_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_attenuation = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_agc_limit = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_band_comp_0 = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_input_low_limit = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_band_comp_1 = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_low_output_gap = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_band_comp_2 = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.ul_amp_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.ul_attenuation = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.ul_agc_limit = (atoi(buf))? 1: 0;
//	getword(buf,post_value,'=');	protocol_control.flag.ul_gain = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.agc_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.temperature_limit = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.gain_balance_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.isolation_detect_limit = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.shutdown_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.gain_balance_value = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.aoc_onoff = (atoi(buf))? 1: 0;
#if 1	/* sprint */
	getword(buf,post_value,'=');	protocol_control.flag.cascade_code = (atoi(buf))? 1: 0;
#endif
	getword(buf,post_value,'=');	protocol_control.flag.alarm_delay = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.lte700_band = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.pcs_band = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.cellular_band = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.aws_band = (atoi(buf))? 1: 0;
#ifdef iDEN800_FUNC_ADD /* iden800 */
	getword(buf,post_value,'=');	protocol_control.flag.iden800_band = (atoi(buf))? 1: 0;
#endif /* iDEN800_FUNC_ADD */

	getword(buf,post_value,'=');	protocol_control.pcs_band[0] = (atoi(buf));//band1 from
	getword(buf,post_value,'=');	protocol_control.pcs_band[2] = (atoi(buf));//band2 from
	getword(buf,post_value,'=');	protocol_control.pcs_band[4] = (atoi(buf));//band3 from
	getword(buf,post_value,'=');	protocol_control.pcs_band[1] = (atoi(buf));//band1 cnt
	getword(buf,post_value,'=');	protocol_control.pcs_band[3] = (atoi(buf));//band2 cnt
	getword(buf,post_value,'=');	protocol_control.pcs_band[5] = (atoi(buf));//band3 cnt
	getword(buf,post_value,'=');	protocol_control.lte700_band = (atoi(buf));//hidden_band_lte
	getword(buf,post_value,'=');	protocol_control.cellular_band =(atoi(buf)); //hidden_band_cellular
	getword(buf,post_value,'=');	protocol_control.aws_band[3] = (atoi(buf)); //hidden_band_aws[0]
	getword(buf,post_value,'=');	protocol_control.aws_band[2] = (atoi(buf)); //hidden_band_aws[1]
	getword(buf,post_value,'=');	protocol_control.aws_band[1] = (atoi(buf)); //hidden_band_aws[2]
#ifdef iDEN800_FUNC_ADD /* iden800 */
	getword(buf,post_value,'=');	protocol_control.iden800_band = (atoi(buf));//hidden_band_iden
#endif /* iDEN800_FUNC_ADD */

#if 0	/* sprint */
	getword(buf,post_value,'=');	protocol_control.dl_amp_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.dl_attenuation = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.dl_agc_limit = atof(buf)/2.;
	getword(buf,post_value,'=');	protocol_control.dl_band_comp[0] = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.dl_input_low_limit = atof(buf)/2.;
	getword(buf,post_value,'=');	protocol_control.dl_band_comp[1] = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.dl_low_output_gap = atof(buf);
	getword(buf,post_value,'=');	protocol_control.dl_band_comp[2] = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.ul_amp_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.ul_attenuation = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.ul_agc_limit = atof(buf)/2;
//	getword(buf,post_value,'=');	protocol_control.ul_gain = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.agc_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.temperature_limit = atof(buf);
	getword(buf,post_value,'=');	protocol_control.gain_balance_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.isolation_detect_limit = atof(buf);
	getword(buf,post_value,'=');	protocol_control.shutdown_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.gain_balance_value = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.aoc_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.alarm_delay = atoi(buf);
#else
	getword(buf,post_value,'=');	protocol_control.dl_attenuation = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.dl_agc_limit = atof(buf)/2.;
	getword(buf,post_value,'=');	protocol_control.dl_amp_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.dl_input_low_limit = atof(buf)/2.;
	getword(buf,post_value,'=');	protocol_control.dl_low_output_gap = atof(buf);
	getword(buf,post_value,'=');	protocol_control.dl_band_comp[0] = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.dl_band_comp[1] = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.dl_band_comp[2] = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.ul_attenuation = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.ul_agc_limit = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.ul_amp_onoff = atoi(buf);
#if 1	/* sprint */
	getword(buf,post_value,'=');	sprintf(protocol_control.cascade_code, "%s", buf);
#endif
	getword(buf,post_value,'=');	protocol_control.alarm_delay = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.temperature_limit = atof(buf);
	getword(buf,post_value,'=');	protocol_control.shutdown_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.gain_balance_value = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.gain_balance_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.aoc_onoff = atoi(buf);
	//getword(buf,post_value,'=');	protocol_control.agc_onoff = atoi(buf);
	//getword(buf,post_value,'=');	protocol_control.isolation_detect_limit = atof(buf);
#endif

	protocol_control.crc = crc((void*)&protocol_control.type, (void*)&protocol_control.crc);
	protocol_buffer_to_frame((void*)&protocol_control,sizeof(protocol_control),frame, &frame_size);
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client);
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);

	logs_write(LOG_RF_SET);
	if(n>0) rfconfig(protocol_control.amp_id);
//	tree(0);
	close(sockfd);
}

//===============================================================================
//===============================================================================
// Tracking Issue #129 Setup Wizard	
void button_apply_setup_wizard_rfconfiguration(char* post_value, int ant_set_ctrl)
{
	char buf[256]="";
	int sockfd;
	int client;

	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;
	struct protocol_web_control protocol_control;
	unsigned char frame[sizeof(protocol_control)*2];
	int frame_size;
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

	memset(&protocol_control, 0, sizeof(protocol_control));
	protocol_control.type = PROTOCOL_CONTROL;

//===============================================================================
//===============================================================================
// Tracking Issue #129 Setup Wizard
	getword(buf,post_value,'='); // flag_setup_wizard_rf_configuration
//===============================================================================
//===============================================================================

	getword(buf,post_value,'=');	protocol_control.amp_id	= atoi(buf);

	getword(buf,post_value,'=');	protocol_control.flag.dl_amp_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_attenuation = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_agc_limit = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_band_comp_0 = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_input_low_limit = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_band_comp_1 = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_low_output_gap = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.dl_band_comp_2 = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.ul_amp_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.ul_attenuation = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.ul_agc_limit = (atoi(buf))? 1: 0;
//	getword(buf,post_value,'=');	protocol_control.flag.ul_gain = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.agc_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.temperature_limit = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.gain_balance_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.isolation_detect_limit = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.shutdown_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.gain_balance_value = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.aoc_onoff = (atoi(buf))? 1: 0;
#if 1	/* sprint */
	getword(buf,post_value,'=');	protocol_control.flag.cascade_code = (atoi(buf))? 1: 0;
#endif
	getword(buf,post_value,'=');	protocol_control.flag.alarm_delay = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.lte700_band = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.pcs_band = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.cellular_band = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_control.flag.aws_band = (atoi(buf))? 1: 0;
#ifdef iDEN800_FUNC_ADD /* iden800 */
	getword(buf,post_value,'=');	protocol_control.flag.iden800_band = (atoi(buf))? 1: 0;
#endif /* iDEN800_FUNC_ADD */

	getword(buf,post_value,'=');	protocol_control.pcs_band[0] = (atoi(buf));//band1 from
	getword(buf,post_value,'=');	protocol_control.pcs_band[2] = (atoi(buf));//band2 from
	getword(buf,post_value,'=');	protocol_control.pcs_band[4] = (atoi(buf));//band3 from
	getword(buf,post_value,'=');	protocol_control.pcs_band[1] = (atoi(buf));//band1 cnt
	getword(buf,post_value,'=');	protocol_control.pcs_band[3] = (atoi(buf));//band2 cnt
	getword(buf,post_value,'=');	protocol_control.pcs_band[5] = (atoi(buf));//band3 cnt
	getword(buf,post_value,'=');	protocol_control.lte700_band = (atoi(buf));//hidden_band_lte
	getword(buf,post_value,'=');	protocol_control.cellular_band =(atoi(buf)); //hidden_band_cellular
	getword(buf,post_value,'=');	protocol_control.aws_band[3] = (atoi(buf)); //hidden_band_aws[0]
	getword(buf,post_value,'=');	protocol_control.aws_band[2] = (atoi(buf)); //hidden_band_aws[1]
	getword(buf,post_value,'=');	protocol_control.aws_band[1] = (atoi(buf)); //hidden_band_aws[2]
#ifdef iDEN800_FUNC_ADD /* iden800 */
	getword(buf,post_value,'=');	protocol_control.iden800_band = (atoi(buf));//hidden_band_iden
#endif /* iDEN800_FUNC_ADD */

#if 0	/* sprint */
	getword(buf,post_value,'=');	protocol_control.dl_amp_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.dl_attenuation = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.dl_agc_limit = atof(buf)/2.;
	getword(buf,post_value,'=');	protocol_control.dl_band_comp[0] = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.dl_input_low_limit = atof(buf)/2.;
	getword(buf,post_value,'=');	protocol_control.dl_band_comp[1] = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.dl_low_output_gap = atof(buf);
	getword(buf,post_value,'=');	protocol_control.dl_band_comp[2] = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.ul_amp_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.ul_attenuation = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.ul_agc_limit = atof(buf)/2;
//	getword(buf,post_value,'=');	protocol_control.ul_gain = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.agc_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.temperature_limit = atof(buf);
	getword(buf,post_value,'=');	protocol_control.gain_balance_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.isolation_detect_limit = atof(buf);
	getword(buf,post_value,'=');	protocol_control.shutdown_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.gain_balance_value = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.aoc_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.alarm_delay = atoi(buf);
#else
	getword(buf,post_value,'=');	protocol_control.dl_attenuation = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.dl_agc_limit = atof(buf)/2.;
	getword(buf,post_value,'=');	protocol_control.dl_amp_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.dl_input_low_limit = atof(buf)/2.;
	getword(buf,post_value,'=');	protocol_control.dl_low_output_gap = atof(buf);
	getword(buf,post_value,'=');	protocol_control.dl_band_comp[0] = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.dl_band_comp[1] = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.dl_band_comp[2] = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.ul_attenuation = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.ul_agc_limit = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.ul_amp_onoff = atoi(buf);
#if 1	/* sprint */
	getword(buf,post_value,'=');	sprintf(protocol_control.cascade_code, "%s", buf);
#endif
	getword(buf,post_value,'=');	protocol_control.alarm_delay = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.temperature_limit = atof(buf);
	getword(buf,post_value,'=');	protocol_control.shutdown_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.gain_balance_value = atof(buf)/2;
	getword(buf,post_value,'=');	protocol_control.gain_balance_onoff = atoi(buf);
	getword(buf,post_value,'=');	protocol_control.aoc_onoff = atoi(buf);
	//getword(buf,post_value,'=');	protocol_control.agc_onoff = atoi(buf);
	//getword(buf,post_value,'=');	protocol_control.isolation_detect_limit = atof(buf);
#endif

// First :: Band Select Control :: TX Packet
	protocol_control.crc = crc((void*)&protocol_control.type, (void*)&protocol_control.crc);
	protocol_buffer_to_frame((void*)&protocol_control,sizeof(protocol_control),frame, &frame_size);
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client);
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);
// Sleep Before Second Control :: TX Packet
	sleep(2);

// Setup Wizard :: Antenna Setting Control	
// Setup Wizard :: Antenna Setting Control	
	switch(ant_set_ctrl)
	{
		case 0: // SKIP
			break;
		case 1: // Antenna Only(AOC Setting)
			{
				// DL/UL ALC Limit = Standard Output Power(33dBm)
				// DL/UL HPA ON
				// Gain Balance ON
				// AOC ON
				// ALC ON
				// Shutdown Control On		
				protocol_control.flag.dl_agc_limit = 1; // Checked(Control):1
				protocol_control.flag.ul_agc_limit = 1; // Checked(Control):1
				protocol_control.flag.dl_amp_onoff = 1; // Checked(Control):1
				protocol_control.flag.ul_amp_onoff = 1; // Checked(Control):1
				protocol_control.flag.gain_balance_onoff = 1; // Checked(Control):1
				protocol_control.flag.aoc_onoff = 1; // Checked(Control):1
				//====================================
				// Request By GST
				// 1. Force control - ALC On
				// 2. Delete WEB UI
				protocol_control.flag.agc_onoff = 1; // Checked(Control):1
				//====================================
				protocol_control.flag.shutdown_onoff = 1; // Checked(Control):1

				protocol_control.dl_agc_limit = 33.0; // Step 0.5dBm (*2, /2)
				protocol_control.ul_agc_limit = 33.0;  // Step 0.5dBm (*2, /2)
				protocol_control.dl_amp_onoff = 1; // OFF:0, ON:1
				protocol_control.ul_amp_onoff = 1; // OFF:0, ON:1
				protocol_control.gain_balance_onoff = 1; // OFF:0, ON:1
				//====================================
				// Request By GST
				// 1. Force control - ALC On
				// 2. Delete WEB UI
				protocol_control.agc_onoff = 1; // OFF:0, ON:1
				//====================================
				protocol_control.aoc_onoff = 1; // OFF:0, ON:1
				protocol_control.shutdown_onoff = 1; // OFF:0, ON:1
			}
			break;
		case 2: // DAS(ALC downlink limit 7dBm Setting)
			{
				// DL ALC Limit = 7dBm
				// UL ALC Limit = Standard Output Power(33dBm)
				// DL/UL HPA ON
				// Gain Balance OFF
				// ALC ON
				// Shutdown Control On
				// Alarm Delay 5 minutes
				// Downlink Attenuation = 0 (Tracking Issue #153)
				// Uplink Attenuation = 0 (Tracking Issue #153)
				protocol_control.flag.dl_agc_limit = 1; // Checked(Control):1
				protocol_control.flag.ul_agc_limit = 1; // Checked(Control):1
				protocol_control.flag.dl_amp_onoff = 1; // Checked(Control):1
				protocol_control.flag.ul_amp_onoff = 1; // Checked(Control):1
				protocol_control.flag.gain_balance_onoff = 1; // Checked(Control):1
				//====================================
				// Request By GST
				// 1. Force control - ALC On
				// 2. Delete WEB UI
				protocol_control.flag.agc_onoff = 1; // Checked(Control):1
				//====================================
				protocol_control.flag.shutdown_onoff = 1; // Checked(Control):1
				protocol_control.flag.alarm_delay = 1; // Checked(Control):1
				protocol_control.flag.dl_attenuation = 1; // Checked(Control):1
				protocol_control.flag.ul_attenuation = 1; // Checked(Control):1

				protocol_control.dl_agc_limit = 7.0; // Step 0.5dBm (*2, /2)
				protocol_control.ul_agc_limit = 33.0;  // Step 0.5dBm (*2, /2)
				protocol_control.dl_amp_onoff = 1; // OFF:0, ON:1
				protocol_control.ul_amp_onoff = 1; // OFF:0, ON:1
				protocol_control.gain_balance_onoff = 0; // OFF:0, ON:1
				//====================================
				// Request By GST
				// 1. Force control - ALC On
				// 2. Delete WEB UI
				protocol_control.agc_onoff = 1; // OFF:0, ON:1
				//====================================
				protocol_control.shutdown_onoff = 1; // OFF:0, ON:1
				protocol_control.alarm_delay = 5; // 5 minutes
				protocol_control.dl_attenuation = 0.0; // 0dB
				protocol_control.ul_attenuation = 0.0; // 0dB

			}
			break;
		case 3: // DAS(ALC downlink limit Maximum Setting)
			{
				// DL ALC Limit = Standard Output Power(33dBm)
				// UL ALC Limit = Standard Output Power(33dBm)
				// DL/UL HPA ON
				// Gain Balance OFF
				// ALC ON
				// Shutdown Control On
				// Alarm Delay 5 minutes
				// Downlink Attenuation = 0 (Tracking Issue #153)
				// Uplink Attenuation = 0 (Tracking Issue #153)
				protocol_control.flag.dl_agc_limit = 1; // Checked(Control):1
				protocol_control.flag.ul_agc_limit = 1; // Checked(Control):1
				protocol_control.flag.dl_amp_onoff = 1; // Checked(Control):1
				protocol_control.flag.ul_amp_onoff = 1; // Checked(Control):1
				protocol_control.flag.gain_balance_onoff = 1; // Checked(Control):1
				//====================================
				// Request By GST
				// 1. Force control - ALC On
				// 2. Delete WEB UI
				protocol_control.flag.agc_onoff = 1; // Checked(Control):1
				//====================================
				protocol_control.flag.shutdown_onoff = 1; // Checked(Control):1
				protocol_control.flag.alarm_delay = 1; // Checked(Control):1
				protocol_control.flag.dl_attenuation = 1; // Checked(Control):1
				protocol_control.flag.ul_attenuation = 1; // Checked(Control):1
				
				protocol_control.dl_agc_limit = 33.0; // Step 0.5dBm (*2, /2)
				protocol_control.ul_agc_limit = 33.0;  // Step 0.5dBm (*2, /2)
				protocol_control.dl_amp_onoff = 1; // OFF:0, ON:1
				protocol_control.ul_amp_onoff = 1; // OFF:0, ON:1
				protocol_control.gain_balance_onoff = 0; // OFF:0, ON:1
				//====================================
				// Request By GST
				// 1. Force control - ALC On
				// 2. Delete WEB UI
				protocol_control.agc_onoff = 1; // OFF:0, ON:1
				//====================================
				protocol_control.shutdown_onoff = 1; // OFF:0, ON:1
				protocol_control.alarm_delay = 5; // 5 minutes
				protocol_control.dl_attenuation = 0.0; // 0dB
				protocol_control.ul_attenuation = 0.0; // 0dB				
			}
			break;
		case 4: // Not Use(HPA OFF)
			{
				// DL/UL HPA OFF
				protocol_control.flag.dl_amp_onoff = 1; // Checked(Control):1
				protocol_control.flag.ul_amp_onoff = 1; // Checked(Control):1

				protocol_control.dl_amp_onoff = 0; // OFF:0, ON:1
				protocol_control.ul_amp_onoff = 0; // OFF:0, ON:1				
			}
			break;
		default: // Skip
			break;			
	}

// Second :: Antenna Setting Control :: TX Packet
	protocol_control.crc = crc((void*)&protocol_control.type, (void*)&protocol_control.crc);
	protocol_buffer_to_frame((void*)&protocol_control,sizeof(protocol_control),frame, &frame_size);
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client);
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);

	logs_write(LOG_RF_SET);
	if(n>0) setup_wizard_step4(1); //is_config=0:status, 1:config	
//	tree(0);
	close(sockfd);
}
//===============================================================================
//===============================================================================

void wizard1_bt_apply_click(char* post_value)
{
#if 0
	char buf[64]="";
	char current_time_buf[16]="";
	char current_time_buf_year[16]="";
	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;

	struct protocol_communication protocol_communication;
	unsigned char frame[sizeof(protocol_communication)*2];
	int frame_size;
	char protocol_buffer[sizeof(struct protocol_communication)*2];
	int buffer_size;
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

	memset(&protocol_communication, 0, sizeof(protocol_communication));
	protocol_communication.type=PROTOCOL_COMMUNICATIONS_READ;
	protocol_buffer_to_frame((void*)&protocol_communication,1,frame, &frame_size);
	sched_yield();
	sendto(sockfd, frame, 3,0,(struct sockaddr*)&serveraddr, client);
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);
	protocol_frame_to_buffer(frame, n, (unsigned char*)protocol_buffer, &buffer_size);
	memmove(&protocol_communication,protocol_buffer,buffer_size);

	protocol_communication.type=PROTOCOL_COMMUNICATIONS_WRITE2;

	getword(buf,post_value,'=');
	sprintf(protocol_communication.engr_no,"%s",buf);

	getword(buf,post_value,'=');
	sprintf(protocol_communication.site_name,"%s",buf);

	strcat(current_time_buf,current_time_buf_year);

	sprintf(protocol_communication.currenttime,"%s",current_time_buf);

	protocol_communication.crc = crc((void*)&protocol_communication.Type, (void*)&protocol_communication.crc);
	protocol_buffer_to_frame((void*)&protocol_communication,sizeof(protocol_communication),frame, &frame_size);
	sched_yield();
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client);

//	logs_write(LOG_COMMUNICATION_SET);
	close(sockfd);
	wizard2();
#endif
}


void wizard2_bt_apply_click(char* post_value)
{
#if 0
	char buf[64]="";
	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;
	struct protocol_web_control protocol_control;
	unsigned char frame[sizeof(protocol_control)*2];
	int frame_size;
	int n, i, j;
	unsigned int bit_val;
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

	memset(&protocol_control, 0, sizeof(protocol_control));
	protocol_control.type = PROTOCOL_CONTROL;
	protocol_control.flag.dl_system_att		= 0;
	protocol_control.flag.ul_system_att		= 0;
	protocol_control.flag.dl_alc_limit			= 0;
	protocol_control.flag.ul_alc_limit			= 0;
	protocol_control.flag.alc_onoff			= 0;
	protocol_control.flag.shutdown_onoff		= 0;
	protocol_control.flag.dl_hpa_onoff			= 0;
	protocol_control.flag.ul_hpa_onoff			= 0;
	protocol_control.flag.alosc_onoff			= 0;
	protocol_control.flag.isolation_detect_level	= 0;
	protocol_control.flag.gain_balance_onoff	= 0;
	protocol_control.flag.gain_balance_offset	= 0;
	protocol_control.flag.dl_rssi_limit			= 0;
	protocol_control.flag.dl_low_output_limit	= 0;
	protocol_control.flag.temperature_limit		= 0;
	protocol_control.flag.alarm_delay_min		= 0;
	protocol_control.flag.band_select			= 1;
	protocol_control.flag.antenna_setting		= 1;

	bit_val=0;
	for(i=0;i<3;i++)
	{
		getword(buf,post_value,'=');
		j = atoi(buf);
		if(j!=0) bit_val |= (1<<(j-1));
	}
	protocol_control.band_select[0] = bit_val & 0xff;
	protocol_control.band_select[1] = (bit_val>>8) & 0xff;
	protocol_control.band_select[2] = (bit_val>>16) & 0xff;

	getword(buf,post_value,'=');
	protocol_control.antenna_setting = atoi(buf);

	protocol_control.crc = crc((void*)&protocol_control.type, (void*)&protocol_control.crc);
	protocol_buffer_to_frame((void*)&protocol_control,sizeof(protocol_control),frame, &frame_size);
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client);

	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);
//	logs_write(LOG_RF_SET);
//	rfconfig();
	close(sockfd);
//	wizard2();
#endif
}

#if 0	/* sprint */
void wireless_activation_wave(char* post_value)
{
	char buf[32]="";
	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;
	struct protocol_modem_activate protocol_activate;
	unsigned char frame[sizeof(protocol_activate)*2];
	int frame_size;
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
	memset(&protocol_activate, 0, sizeof(protocol_activate));
	protocol_activate.type = PROTOCOL_MODEM_ACTIVATE;

	getword(buf,post_value,'=');

	getword(buf,post_value,'=');
	sprintf(protocol_activate.Code,"%s",buf);

	getword(buf,post_value,'=');
	sprintf(protocol_activate.MIN,"%s",buf);

	getword(buf,post_value,'=');
	protocol_activate.activate_mode=(unsigned char)atoi(buf);
	
	protocol_activate.crc = crc((void*)&protocol_activate.type, (void*)&protocol_activate.crc);
	protocol_buffer_to_frame((void*)&protocol_activate,sizeof(protocol_activate),frame, &frame_size);
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client);
      
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);
	
	if(n > 0)
		modem_activate_wave();
	close(sockfd);

}


void wireless_activation_tel(char* post_value)
{
	char buf[32]="";
	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;
	struct protocol_modem_activate protocol_activate;
	unsigned char frame[sizeof(protocol_activate)*2];
	int frame_size;
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
	memset(&protocol_activate, 0, sizeof(protocol_activate));
	protocol_activate.type = PROTOCOL_MODEM_ACTIVATE;

	getword(buf,post_value,'=');

	getword(buf,post_value,'=');

	getword(buf,post_value,'=');
	sprintf(protocol_activate.apn,"%s",buf);

	getword(buf,post_value,'=');
	sprintf(protocol_activate.usr,"%s",buf);
	
	getword(buf,post_value,'=');
	sprintf(protocol_activate.pass,"%s",buf);

	
	protocol_activate.crc = crc((void*)&protocol_activate.type, (void*)&protocol_activate.crc);
	protocol_buffer_to_frame((void*)&protocol_activate,sizeof(protocol_activate),frame, &frame_size);
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client);
      
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);
	
	if(n > 0)
		modem_activate_tel();
	close(sockfd);

}
#endif
/*
void ups_configuration(char* post_value)
{
	char buf[32]="";
	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;
	struct protocol_ups protocol_ups_configuration;
	unsigned char frame[sizeof(protocol_ups_configuration)*2];
	int frame_size;
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
	memset(&protocol_ups_configuration, 0, sizeof(protocol_ups_configuration));
	protocol_ups_configuration.type = PROTOCOL_UPS_WRITE;

	getword(buf,post_value,'=');
	protocol_ups_configuration.ups_onoff_mode = (unsigned char)atoi(buf);

	getword(buf,post_value,'=');
	protocol_ups_configuration.ups_lowpower_mode = (unsigned char)atoi(buf);
	
	protocol_ups_configuration.crc = crc((void*)&protocol_ups_configuration.type, (void*)&protocol_ups_configuration.crc);
	protocol_buffer_to_frame((void*)&protocol_ups_configuration,sizeof(protocol_ups_configuration),frame, &frame_size);
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client);
      
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);

	if(n > 0)
		communications();
	close(sockfd);
}
*/
void button_apply_alarmconfiguration(char* post_value)
{
	char buf[128]="";
	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;

	struct protocol_web_alarm protocol_web_alarm;
	unsigned char frame[sizeof(protocol_web_alarm)*2];
	int frame_size;
	int n, i, amp_id;

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

	protocol_web_alarm.type=PROTOCOL_ALARMCONFIGURATION_WRITE;

	getword(buf,post_value,'=');	protocol_web_alarm.contol_amp_id = amp_id = (unsigned char)atoi(buf);	//[0] check_amp_id_alarm
	getword(buf,post_value,'=');	protocol_web_alarm.slot[amp_id].report_alarm = (unsigned char)atoi(buf);	//[1] combo_report

	for(i=0;i<ALARM_INDEX_MAX;i++)
	{
		getword(buf,post_value,'=');	//txt_status_i
		getword(buf,post_value,'=');	protocol_web_alarm.slot[amp_id].alarm_item[i].active=(unsigned char)atoi(buf);//combo_active_i
		getword(buf,post_value,'=');	protocol_web_alarm.slot[amp_id].alarm_item[i].category=(unsigned char)atoi(buf); //combo_category_i
	}

	protocol_web_alarm.crc = crc((void*)&protocol_web_alarm.type, (void*)&protocol_web_alarm.crc);
	protocol_buffer_to_frame((void*)&protocol_web_alarm, sizeof(protocol_web_alarm), frame, &frame_size);
//	sched_yield();
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client);
#if 0
{
	char str[128];
	sprintf(str, "amp_id=%d, %d", amp_id, protocol_web_alarm.contol_amp_id);
	debuglog(str);
}
#endif

	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);
	if(n>0) alarm_configuration(amp_id);
	logs_write(LOG_ALARMCONFIGURATION_SET);

	close(sockfd);
}


void button_apply_fake_alarmconfiguration(char* post_value)
{
	char buf[128]="";
	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;

	struct protocol_web_alarm protocol_web_alarm;
	unsigned char frame[sizeof(protocol_web_alarm)*2];
	int frame_size;
	int n, i, amp_id;

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

	protocol_web_alarm.type=PROTOCOL_FAKE_CONFIGURATION_WRITE;

	getword(buf,post_value,'=');	protocol_web_alarm.contol_amp_id = amp_id = (unsigned char)atoi(buf);	//[0] check_amp_id_alarm
	getword(buf,post_value,'=');	protocol_web_alarm.slot[amp_id].fake_mode = (unsigned char)atoi(buf);	//[1] combo_fake_mode

	for(i=0;i<ALARM_INDEX_MAX;i++)
	{
		getword(buf,post_value,'=');	//txt_status_i
		getword(buf,post_value,'=');	protocol_web_alarm.slot[amp_id].alarm_item[i].fake_status=(unsigned char)atoi(buf);//combo_active_i
	}

	protocol_web_alarm.crc = crc((void*)&protocol_web_alarm.type, (void*)&protocol_web_alarm.crc);
	protocol_buffer_to_frame((void*)&protocol_web_alarm, sizeof(protocol_web_alarm), frame, &frame_size);
//	sched_yield();
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client);
#if 0
{
	char str[128];
	sprintf(str, "amp_id=%d, %d", amp_id, protocol_web_alarm.contol_amp_id);
	debuglog(str);
}
#endif

	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);
//	if(n>0) fake_alarm_configuration(amp_id);
	waiting_page(1, "Fake alarm configuration", "submitting", NULL, PAGE_fake_alarm_1+amp_id);
	logs_write(LOG_ALARMCONFIGURATION_SET);

	close(sockfd);
}


void button_apply_communications(char* post_values)
{
	//char buf[64]="";
//	char current_time_buf[16]="";
//	char current_time_buf_year[16]="";

	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;

	struct protocol_communication protocol_communication;
	unsigned char frame[sizeof(protocol_communication)*2];
	int frame_size;

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
	protocol_communication.type=PROTOCOL_COMMUNICATIONS_WRITE;
/*
	getword(buf,post_value,'=');	protocol_communication.flag.lan_dhcp_client_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.lan_ip = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.lan_dhcp_server_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.lan_netmask = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.lan_gateway = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.wan_modem_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.wan_ip = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.wan_dhcp_client_onoff = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.wan_netmask = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.wan_gateway = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.snmp_read_community = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.snmp_manager_ip = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.snmp_write_community = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.snmp_general_port = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.snmp_trap_community = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.snmp_trap_port = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.set_year = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.set_month = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.set_day = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.set_hour = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.set_minute = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.heartbeat_interval = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.system_id = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.coordinate = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.location = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.contact_info = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.ntp_ip = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.ntp_period = (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	protocol_communication.flag.time_zone = (atoi(buf))? 1: 0;

	getword(buf,post_value,'=');	protocol_communication.lan_dhcp_client_onoff= (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	sprintf(protocol_communication.lan_ip, "%s", buf);
	getword(buf,post_value,'=');	protocol_communication.lan_dhcp_server_onoff= (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	sprintf(protocol_communication.lan_netmask, "%s", buf);
	getword(buf,post_value,'=');	sprintf(protocol_communication.lan_gateway, "%s", buf);
	getword(buf,post_value,'=');	protocol_communication.wan_modem_onoff= (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	sprintf(protocol_communication.wan_ip, "%s", buf);
	getword(buf,post_value,'=');	protocol_communication.wan_dhcp_client_onoff= (atoi(buf))? 1: 0;
	getword(buf,post_value,'=');	sprintf(protocol_communication.wan_netmask, "%s", buf);
	getword(buf,post_value,'=');	sprintf(protocol_communication.wan_gateway, "%s", buf);
	getword(buf,post_value,'=');	sprintf(protocol_communication.snmp_read_community, "%s", buf);
	getword(buf,post_value,'=');	sprintf(protocol_communication.snmp_manager_ip, "%s", buf);
	getword(buf,post_value,'=');	sprintf(protocol_communication.snmp_write_community, "%s", buf);
	getword(buf,post_value,'=');	protocol_communication.snmp_general_port= atoi(buf);
	getword(buf,post_value,'=');	sprintf(protocol_communication.snmp_trap_community, "%s", buf);
	getword(buf,post_value,'=');	protocol_communication.snmp_trap_port= atoi(buf);
	getword(buf,post_value,'=');	protocol_communication.heartbeat_interval= atoi(buf);
	getword(buf,post_value,'=');	sprintf(protocol_communication.system_id, "%s", buf);
	getword(buf,post_value,'=');	sprintf(protocol_communication.coordinate, "%s", buf);
	getword(buf,post_value,'=');	sprintf(protocol_communication.location, "%s", buf);
	getword(buf,post_value,'=');	sprintf(protocol_communication.contact_info, "%s", buf);
	getword(buf,post_value,'=');	//current_year
	getword(buf,post_value,'=');	protocol_communication.set_year= atoi(buf);
	getword(buf,post_value,'=');	//current_time
	getword(buf,post_value,'=');	protocol_communication.set_month= atoi(buf);
	getword(buf,post_value,'=');	protocol_communication.set_day= atoi(buf);
	getword(buf,post_value,'=');	protocol_communication.time_zone= atoi(buf);		//timezone
	getword(buf,post_value,'=');	protocol_communication.set_hour= atoi(buf);
	getword(buf,post_value,'=');	protocol_communication.set_minute= atoi(buf);
	getword(buf,post_value,'=');	sprintf(protocol_communication.ntp_ip, "%s", buf);	//ntp ip
	getword(buf,post_value,'=');	protocol_communication.ntp_period= atoi(buf);		//ntp period
*/
#define CMB(name) \
do{ \
	char* check; \
	char* value; \
	if(!(check = post_value("check_combo_"#name))) \
		GOTO_ERROR; \
	if(!(value = post_value("combo_"#name))) \
		GOTO_ERROR; \
	if((protocol_communication.flag.name = (atoi(check)) ? 1: 0)) \
		protocol_communication.name = atoi(value); \
} while(0)
#define TXT(name) \
do{ \
	char* check; \
	char* value; \
	if(!(check = post_value("check_txt_"#name))) \
		GOTO_ERROR; \
	if(!(value = post_value("txt_"#name))) \
		GOTO_ERROR; \
	if((protocol_communication.flag.name = (atoi(check)) ? 1: 0)) \
		snprintf(protocol_communication.name, sizeof(protocol_communication.name), "%s", value); \
} while(0)
#define NMB(name) \
do{ \
	char* check; \
	char* value; \
	if(!(check = post_value("check_txt_"#name))) \
		GOTO_ERROR; \
	if(!(value = post_value("txt_"#name))) \
		GOTO_ERROR; \
	if((protocol_communication.flag.name = (atoi(check)) ? 1: 0)) \
		protocol_communication.name = atoi(value); \
} while(0)
	CMB(lan_dhcp_client_onoff);
	TXT(lan_ip);
	CMB(lan_dhcp_server_onoff);
	TXT(lan_netmask);
	TXT(lan_gateway);
	CMB(wan_modem_onoff);
	TXT(wan_ip);
	CMB(wan_dhcp_client_onoff);
	TXT(wan_netmask);
	TXT(wan_gateway);
	TXT(snmp_read_community);
	TXT(snmp_manager_ip);
	TXT(snmp_write_community);
	NMB(snmp_general_port);
	TXT(snmp_trap_community);
	NMB(snmp_trap_port);
	NMB(set_year);
	CMB(set_month);
	NMB(set_day);
	NMB(set_hour);
	NMB(set_minute);
	NMB(heartbeat_interval);
#if 0	/* sprint */
	TXT(system_id);
	TXT(coordinate);
	TXT(location);
	TXT(contact_info);
#else
	TXT(latitude);
	TXT(longitude);
#endif
	TXT(ntp_ip);
	CMB(ntp_period);
	CMB(time_zone);
/* C^2 Rev3 */
	CMB(snmp_version);
	//TXT(engine_id);
	TXT(read_user);
	TXT(write_user);
	CMB(auth);
	CMB(priv);
	TXT(authpassphrase);
	TXT(privpassphrase);
	//TXT(trap_engine_id);
	TXT(trap_user);
	CMB(trap_auth);
	CMB(trap_priv);
	TXT(trap_authpassphrase);
	TXT(trap_privpassphrase);
#undef CMB
#undef TXT
#undef NMB
#define CMB(name) \
do{ \
	char* value; \
	if(!(value = post_value("combo_"#name))) \
		GOTO_ERROR; \
	protocol_communication.name = atoi(value); \
} while(0)
#define NMB(name) \
do{ \
	char* value; \
	if(!(value = post_value("txt_"#name))) \
		GOTO_ERROR; \
	protocol_communication.name = atoi(value); \
} while(0)
	if(protocol_communication.flag.set_year 
	|| protocol_communication.flag.set_month 
	|| protocol_communication.flag.set_day 
	|| protocol_communication.flag.set_hour 
	|| protocol_communication.flag.set_minute)
	{
		NMB(set_year);
		CMB(set_month);
		NMB(set_day);
		NMB(set_hour);
		NMB(set_minute);
	}
#undef CMB
#undef NMB
	logs_write(LOG_COMMUNICATION_SET);

	protocol_communication.crc = crc((void*)&protocol_communication.type, (void*)&protocol_communication.crc);
	protocol_buffer_to_frame((void*)&protocol_communication,sizeof(protocol_communication),frame, &frame_size);
	sched_yield();
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client);
RETURN:
	if(0 <= sockfd)
		close(sockfd);
	return;
ERROR:
	goto RETURN;
}


#if 0	/* sprint */
void button_apply_modem(void)
{
//	char buf[64]="";

	int sockfd;
	int client;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;

	struct protocol_communication protocol_communication;
	unsigned char frame[sizeof(protocol_communication)*2];
	int frame_size;

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
	protocol_communication.type=PROTOCOL_MODEM_ACTIVATE;

	logs_write(LOG_MODEM_SET);

	protocol_communication.crc = crc((void*)&protocol_communication.type, (void*)&protocol_communication.crc);
	protocol_buffer_to_frame((void*)&protocol_communication,sizeof(protocol_communication),frame, &frame_size);
	sched_yield();
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client);
	close(sockfd);
}
#endif

void logs_write(unsigned char type)
{
	FILE* logs=NULL;
	FILE* session=fopen("/var/tmp/session","r");
	char buf[32]="";
	char datetime[128]="";
	struct tm* t;
	time_t curtime;
	ssize_t read;
	size_t len=0 ;
	char* line=NULL;
	int i=0;
	int j=0;

	if(session)
	{
		while((read=getline(&line,&len,session))!=-1)
		{
			getword(buf,line,'=');
			break;
		}
	}
	if(session) fclose(session);

	time(&curtime);
	t=localtime(&curtime);
//	snprintf(datetime,sizeof(datetime),"%02d/%02d/%d - %02d:%02d:%02d",t->tm_mon+1,t->tm_mday,t->tm_year+1900,t->tm_hour,t->tm_min,t->tm_sec);
	snprintf(datetime,sizeof(datetime),"%d/%02d/%02d - %02d:%02d:%02d",t->tm_year+1900, t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
    for(i=0; i<30; i++)
        memset(&user_buffer[i],0,sizeof(user_buffer[i]));

    logs=fopen("/usr/local/repeater/logs.txt","r");
    if(logs)
    {
		for(i = 0; i < 30 && 0 < (read=getline(&line,&len,logs)); i++)
            sprintf(user_buffer[i].name,"%s",line);
    }
    if(logs) fclose(logs);
	sched_yield();
	logs=fopen("/usr/local/repeater/logs.txt","w");

	if(logs)
	{
		switch(type)
		{
		case LOG_LOGIN_CHECK:
			fprintf(logs,"%s=%s=Login=Login\n",datetime,buf);
			break;
		case LOG_STATUS_CHECK:
			fprintf(logs,"%s=%s=Status=Checked\n",datetime,buf);
			break;
		case LOG_RF_CHECK:
			fprintf(logs,"%s=%s=RF configuration=Checked\n",datetime,buf);
			break;
		case LOG_RF_SET:
			fprintf(logs,"%s=%s=RF configuration=Set\n",datetime,buf);
			break;
		case LOG_ALARM_CHECK:
			fprintf(logs,"%s=%s=Alarm configuration=Checked\n",datetime,buf);
			break;
		case LOG_ALARM_SET:
			fprintf(logs,"%s=%s=Alarm configuration=Set\n",datetime,buf);
			break;
		case LOG_COMMUNICATION_CHECK:
			fprintf(logs,"%s=%s=Communications=Checked\n",datetime,buf);
			break;
		case LOG_COMMUNICATION_SET:
			fprintf(logs,"%s=%s=Communications=Set\n",datetime,buf);
			break;
		case LOG_MODEM_SET:
			fprintf(logs,"%s=%s=Modem activation=Set\n",datetime,buf);
			break;
		case LOG_USER_ACCESS:
			fprintf(logs,"%s=%s=User management=Accessed\n",datetime,buf);
			break;
		case LOG_USER_INSERT:
			fprintf(logs,"%s=%s=User management=Accessible user ID has been added\n",datetime,buf);
			break;
		case LOG_USER_DELETE:
			fprintf(logs,"%s=%s=User management=Accessible user ID has been deleted\n",datetime,buf);
			break;
		case LOG_LOGS_CHECK:
			fprintf(logs,"%s=%s=Logs=Checked\n",datetime,buf);
			break;
		case LOG_TROUBLE_CHECK:
			fprintf(logs,"%s=%s=Troubleshooting=Checked\n",datetime,buf);
			break;
		case LOG_LOGOUT_CHECK:
			fprintf(logs,"%s=%s=Logout=Logout\n",datetime,buf);
			break;
		case LOG_SUBSYSTEM_MAILING_CHECK:
			fprintf(logs,"%s=%s=Subsystem mailing=Checked\n",datetime,buf);
			break;
		case LOG_SYSTEM_DOWNLOAD_CHECK:
			fprintf(logs,"%s=%s=System download=Checked\n",datetime,buf);
			break;
		case LOG_SYSTEMRESET_CHECK:
			fprintf(logs,"%s=%s=System reset=Checked\n",datetime,buf);
			break;
		case LOG_ALARMCONFIGURATION_CHECK:
			fprintf(logs,"%s=%s=Alarm configuration=Checked\n",datetime,buf);
			break;
		case LOG_ALARMCONFIGURATION_SET:
			fprintf(logs,"%s=%s=Alarm configuration=Set\n",datetime,buf);
			break;
		case LOG_SUBSYSTEM_MAILING_SET:
			fprintf(logs,"%s=%s=Subsystem mailing=Set\n",datetime,buf);
			break;
		case LOG_SYSTEM_DOWNLOAD_SET:
			fprintf(logs,"%s=%s=System download=Set\n",datetime,buf);
			break;
		case LOG_ALARM_LOGS_CHECK:
			fprintf(logs,"%s=%s=Alarm logs=Checked\n",datetime,buf);
			break;
		case LOG_OSC_LOGS_CHECK:
			fprintf(logs,"%s=%s=Oscillation logs=Checked\n",datetime,buf);
			break;
		case LOG_ALARM_LOGS_CLEAR_SET:
			fprintf(logs,"%s=%s=Alarm logs Clear=Set\n",datetime,buf);
			break;
		case LOG_LOGS_CLEAR_SET:
			fprintf(logs,"%s=%s=Logs clear=Set\n",datetime,buf);
			break;
		case LOG_OSC_LOGS_CLEAR_CHECK:
			fprintf(logs,"%s=%s=Osc logs clear=Set\n",datetime,buf);
			break;

		}
	}
	if(logs) fclose(logs);

	sync();
   	sched_yield();

	logs=fopen("/usr/local/repeater/logs.txt","a");
	if(logs)
	{
		if(i<29)
		{
			for(j=0; j<i; j++)
				fprintf(logs,"%s",user_buffer[j].name);
		}
		else
		{
			for(j=0; j<29; j++)
				fprintf(logs,"%s",user_buffer[j].name);
		}
	}
	if(logs) fclose(logs);

	sync();
   	sched_yield();

	if(line) free(line);
}


void get_log()
{
	FILE* logs=NULL;
	ssize_t read;
	size_t len=0;
	char* line=NULL;
	char buf[128]="";
	int line_no=0;

	logs=fopen("/usr/local/repeater/logs.txt","r");
	if(logs)
	{
		while((read=getline(&line,&len,logs)) > 0)
		{
			printf("<tr bgcolor=\"#FFFFFF\" height=\"20\"> \n");
			printf("<td ><font face=\"Arial, Helvetica, sans-serif\" size=\"2\">&nbsp;&nbsp; %d</font></td>\n", ++line_no);
			getword(buf,line,'=');	
			printf("<td ><font face=\"Arial, Helvetica, sans-serif\" size=\"2\">&nbsp;&nbsp; %s</font></td>\n", buf);
			getword(buf,line,'=');
			printf("<td ><font face=\"Arial, Helvetica, sans-serif\" size=\"2\">&nbsp;&nbsp; %s</font></td>\n", buf);
			getword(buf,line,'=');
			printf("<td ><font face=\"Arial, Helvetica, sans-serif\" size=\"2\">&nbsp;&nbsp; %s</font></td>\n", buf);
			printf("<td ><font face=\"Arial, Helvetica, sans-serif\" size=\"2\">&nbsp;&nbsp; %s</font></td>\n", line);
			printf("</tr>\n");
		}
	}
	if(logs) fclose(logs);
	if(line) free(line);
}

void get_alarm_log(int alarm_page)
{
	FILE* logs=NULL;
	ssize_t read;
	size_t len=0;
	char* line=NULL;
	int num = 0;
	int i = 0;
	int j = 0;
	int k=0;
	int alarm_num = 0;
	char name[128]="";
	char module[128]="";
	char status[128]="";
	char active[128]="";
	char snmpmap[128]="";
	char datatime[128]="";
	struct alarm_buffer alarm_buffer[200];
	struct temp_buffer temp_buffer[200];
	char font_color[4][10] = {"#000000", "#000000", "#000000", "#000000"};

	strcpy(font_color[alarm_page], "#640064");

	logs=fopen("/usr/local/repeater/alarm_log.txt","r");
//----------------------------------------------------------------------

	if(logs)
	{
		while((read=getline(&line,&len,logs)) > 0)
		{	
			sprintf(alarm_buffer[i++].name,"%s",line);
		}

		k= alarm_num = i;
		if		(k > 30 && alarm_page == PAGE_1) k = 30;
		else if	(k > 60 && alarm_page == PAGE_2) k = 60;
		else if	(k > 90 && alarm_page == PAGE_3) k = 90;

		num = num + (30 * alarm_page);

		for(j=(30 * alarm_page); j < k; j++)
		{
			sprintf(temp_buffer[j].name,"%s",alarm_buffer[--i -(30 * alarm_page)].name);
			getword(module, temp_buffer[j].name,	'=' );
			getword(name, temp_buffer[j].name,	'=' );
			getword(status, temp_buffer[j].name, '=' );
			getword(active, temp_buffer[j].name, '=' );
			getword(snmpmap, temp_buffer[j].name,	'=' );
			getword(datatime, temp_buffer[j].name, '\n' );

			if(strcmp(name, "\n")!=0)
			{
				printf("  <tr bgcolor=\"#FFFFFF\">\n");
				printf("    <td height=\"16\"><div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">%d</font></div></td>\n", ++num);
				printf("    <td height=\"16\"><div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">&nbsp;&nbsp;%s</font></td>\n", datatime);
				printf("    <td height=\"16\" bgcolor=\"%s\"><div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"></font></div></td>\n", (strcmp(status,"0")? "#FF0000": "#00FF00"));
				printf("    <td height=\"16\"><div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">&nbsp;&nbsp;%s</font></td>\n", module);
				printf("    <td height=\"16\"><div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">&nbsp;&nbsp;%s</font></td>\n", name);
				printf("  </tr>\n");
			}
		}
	}
	printf("</table>\n");

	printf("<table width=\"740\" bgcolor=\"transparent\" border=\"0\" ><tr hight=\"10\"><td></td></tr><tr><td align=\"center\">\n");

	if(alarm_num > 90){
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=alarm_log\"><font color=\"%s\" size=\"3\">1</font></b></a></font>\n", font_color[0]);
		printf("&nbsp;&nbsp;   \n");
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=alarm_log2\"><font color=\"%s\" size=\"3\">2</font></b></a></font>\n", font_color[1]);
		printf("&nbsp;&nbsp;   \n");
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=alarm_log3\"><font color=\"%s\" size=\"3\">3</font></b></a></font>\n", font_color[2]);
		printf("&nbsp;&nbsp;   \n");
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=alarm_log4\"><font color=\"%s\" size=\"3\">4</font></b></a></font>\n", font_color[3]);
	} else if (alarm_num > 60){
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=alarm_log\"><font color=\"%s\" size=\"3\">1</font></b></a></font>\n", font_color[0]);
		printf("&nbsp;&nbsp;   \n");
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=alarm_log2\"><font color=\"%s\" size=\"3\">2</font></b></a></font>\n", font_color[1]);
		printf("&nbsp;&nbsp;   \n");
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=alarm_log3\"><font color=\"%s\" size=\"3\">3</font></b></a></font>\n", font_color[2]);
	}else if(alarm_num > 30){
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=alarm_log\"><font color=\"%s\" size=\"3\">1</font></b></a></font>\n", font_color[0]);
		printf("&nbsp;&nbsp;   \n");
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=alarm_log2\"><font color=\"%s\" size=\"3\">2</font></b></a></font>\n", font_color[1]);
	}
	printf("</td></tr></table>\n");
    if(logs) fclose(logs);
    if(line) free(line);
}

#if 0	/* sprint */
void get_osc_log(int alarm_page)
{
	FILE* logs=NULL;
	ssize_t read;
	size_t len=0;
	char* line=NULL;
	int num = 0;
	int i = 0;
	int j = 0;
	int k=0;
	int log_num = 0;
	char module[128]="";
	char atten[128]="";
	char datatime[128]="";
	struct alarm_buffer alarm_buffer[200];
	struct temp_buffer temp_buffer[200];

	logs=fopen("/usr/local/repeater/osc_log.txt","r");
//----------------------------------------------------------------------

	if(logs)
	{
		while((read=getline(&line,&len,logs)) > 0)
		{	
			sprintf(alarm_buffer[i++].name,"%s",line);
		}

		k= log_num = i;
		if		(k > 30 && alarm_page == PAGE_1) k = 30;
		else if	(k > 60 && alarm_page == PAGE_2) k = 60;
		else if	(k > 90 && alarm_page == PAGE_3) k = 90;

		num = num + (30 * alarm_page);

		for(j=(30 * alarm_page); j < k; j++)
		{
			sprintf(temp_buffer[j].name,"%s",alarm_buffer[--i -(30 * alarm_page)].name);
			getword(module, temp_buffer[j].name,	'=' );
			getword(atten, temp_buffer[j].name,	'=' );
			getword(datatime, temp_buffer[j].name, '\n' );

			if(strcmp(datatime, "\n")!=0)
			{
				printf("  <tr bgcolor=\"#FFFFFF\">\n");
				printf("    <td height=\"16\"><div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">%d</font></div></td>\n", ++num);
				printf("    <td height=\"16\"><div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">&nbsp;&nbsp;%s</font></td>\n", module);
				printf("    <td height=\"16\"><div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">&nbsp;&nbsp;%s</font></td>\n", atten);
				printf("    <td height=\"16\"><div align=\"left\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">&nbsp;&nbsp;%s</font></td>\n", datatime);
				printf("  </tr>\n");
			}
		}
	}
	printf("</table>\n");

	printf("<table width=\"740\" bgcolor=\"transparent\" border=\"0\" ><tr hight=\"10\"><td></td></tr><tr><td align=\"center\">\n");
	if(log_num > 90){
		printf("  <b><a href=\"html.cgi?function=logs_osc\"><font size=\"3\">1</font></b></a>\n");
		printf("&nbsp;&nbsp;   \n");
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=logs_osc_2\"><font color=\"#000000\" size=\"3\">2</font></b></a></font>\n");
		printf("&nbsp;&nbsp;   \n");
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=logs_osc_3\"><font color=\"#000000\" size=\"3\">3</font></b></a></font>\n");
		printf("&nbsp;&nbsp;   \n");
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=logs_osc_4\"><font color=\"#000000\" size=\"3\">4</font></b></a></font>\n");
	} else if (log_num > 60){
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=logs_osc\"><font color=\"#000000\" size=\"3\">1</font></b></a></font>\n");
		printf("&nbsp;&nbsp;   \n");
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=logs_osc_2\"><font color=\"#000000\" size=\"3\">2</font></b></a></font>\n");
		printf("&nbsp;&nbsp;   \n");
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=logs_osc_3\"><font color=\"#000000\" size=\"3\">3</font></b></a></font>\n");
	}else if(log_num > 30){
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=logs_osc\"><font color=\"#000000\" size=\"3\">1</font></b></a></font>\n");
		printf("&nbsp;&nbsp;   \n");
		printf("  <font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=logs_osc_2\"><font color=\"#000000\" size=\"3\">2</font></b></a></font>\n");
	}
	printf("</td></tr></table>\n");
    if(logs) fclose(logs);
    if(line) free(line);
}
#endif

void create_ajax_code()
{
	printf("function chkAjaBrowser()\n");
	printf("{\n");
	printf("	var a,ua = navigator.userAgent;\n");
	printf("	this.bw= {\n");
	printf("		safari    : ((a=ua.split('AppleWebKit/')[1])? a.split('(')[0]: 0)>=124 ,\n");
	printf("		konqueror : ((a=ua.split('Konqueror/')[1])? a.split(';')[0]: 0)>=3.3 ,\n");
	printf("		mozes     : ((a=ua.split('Gecko/')[1])? a.split(\" \")[0]: 0) >= 20011128 ,\n");
	printf("		opera     : (!!window.opera) && ((typeof XMLHttpRequest)=='function') ,\n");
	printf("		msie      : (!!window.ActiveXObject)? (!!createHttpRequest()): false\n");
	printf("	}\n");
	printf("	return (this.bw.safari||this.bw.konqueror||this.bw.mozes||this.bw.opera||this.bw.msie)\n");
	printf("}\n");
	printf("\n");

	printf("function createHttpRequest()\n");
	printf("{\n");
	printf("	if(window.ActiveXObject){\n");
	printf("		try {\n");
	printf("			return new ActiveXObject(\"Msxml2.XMLHTTP\") ;\n");
	printf("		} catch (e) {\n");
	printf("			try {\n");
	printf("				return new ActiveXObject(\"Microsoft.XMLHTTP\") ;\n");
	printf("			} catch (e2) {\n");
	printf("				return null ;\n");
	printf("			}\n");
	printf("		}\n");
	printf("	} else if(window.XMLHttpRequest){\n");
	printf("		return new XMLHttpRequest() ;\n");
	printf("	} else {\n");
	printf("		return null ;\n");
	printf("	}\n");
	printf("}\n");
	printf("\n");

	printf("function sendRequest(callback,data,method,url,async,sload,user,password)\n");
	printf("{\n");
	printf("	var oj = createHttpRequest();\n");
	printf("	if( oj == null ) return null;\n");
	printf("\n");
	printf("	var sload = (!!sendRequest.arguments[5])?sload:false;\n");
	printf("	if(sload || method.toUpperCase()=='GET')url += \"?\";\n");
	printf("	if(sload)url=url+\"t=\"+(new Date()).getTime();\n");
	printf("\n");
	printf("	var bwoj 		= new chkAjaBrowser();\n");
	printf("	var opera		= bwoj.bw.opera;\n");
	printf("	var safari		= bwoj.bw.safari;\n");
	printf("	var konqueror	= bwoj.bw.konqueror;\n");
	printf("	var mozes		= bwoj.bw.mozes ;\n");
	printf("\n");
	printf("	if(opera || safari || mozes){\n");
	printf("		oj.onload = function () { callback(oj); }\n");
	printf("	} else {\n");
	printf("		oj.onreadystatechange =function ()\n");
	printf("		{\n");
	printf("			if ( oj.readyState == 4 ){\n");
	printf("				callback(oj);\n");
	printf("			}\n");
	printf("		}\n");
	printf("	}\n");
	printf("\n");
	printf("	data = uriEncode(data)\n");
	printf("	if(method.toUpperCase() == 'GET') {\n");
	printf("		url += data\n");
	printf("	}\n");
	printf("\n");
	printf("	oj.open(method,url,async,user,password);\n");
	printf("	setEncHeader(oj)\n");
	printf("	oj.send(data);\n");
	printf("\n");
	printf("	function setEncHeader(oj){\n");
	printf("		var contentTypeUrlenc = 'application/x-www-form-urlencoded; charset=UTF-8';\n");
	printf("		if(!window.opera){\n");
	printf("			oj.setRequestHeader('Content-Type',contentTypeUrlenc);\n");
	printf("		} else {\n");
	printf("			if((typeof oj.setRequestHeader) == 'function')\n");
	printf("				oj.setRequestHeader('Content-Type',contentTypeUrlenc);\n");
	printf("		}\n");
	printf("		return oj\n");
	printf("	}\n");
	printf("\n");
	printf("	function uriEncode(data){\n");
	printf("		if(data!=\"\"){\n");
	printf("			var encdata = '';\n");
	printf("			var datas = data.split('&');\n");
	printf("			for(i=1;i<datas.length;i++)\n");
	printf("			{\n");
	printf("				var dataq = datas[i].split('=');\n");
	printf("				encdata += '&'+encodeURIComponent(dataq[0])+'='+encodeURIComponent(dataq[1]);\n");
	printf("			}\n");
	printf("		} else {\n");
	printf("			encdata = \"\";\n");
	printf("		}\n");
	printf("		return encdata;\n");
	printf("	}\n");
	printf("	return oj\n");
	printf("}\n");
	printf("\n");
}


void html_menu_head_left(struct protocol_web_status *p_status)
{
	printf("<table width=\"260\" height=\"355\" border=\"1\" align=\"left\">\n");
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=# onMouseUp=\"logout()\"> Logout</a></b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=tree_status\"> RF Status</a></b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=tree_rfconfig\"> RF Configuration</a></b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=alarm_config\"> Alarm Configuration</a></b></font></td>\n");
	printf("	</tr>\n");
#if 0 /* sprint */
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=fake_alarm_config\"> Fake Alarm Configuration</a></b></font></td>\n");
	printf("	</tr>\n");
#endif
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=communications\"> Communication Configuration</a></b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=usermanagement\"> User Management</a></b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=alarm_log\"> Alarm Log</a></b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=logs\"> Log</a></b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=troubleshooting\"> Troubleshooting</a></b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=system_download\"> Software Upgrade</a></b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=reset\"> System Reset</a></b></font></td>\n");
	printf("	</tr>\n");
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=factory_default\"> Factory Default Setting</a></b></font></td>\n");
	printf("	</tr>\n");
#if 0	/* sprint */
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=logs_osc\"> Oscillation Log</a></b></font></td>\n");
	printf("	</tr>\n");
#endif
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=transfer_except\"> Configuration Transfer</a></b></font></td>\n");
	printf("	</tr>\n");
#if 0	/* sprint */
	printf("	<tr bgcolor=\"#CCCCCC\">\n");
	printf("		<td width=\"8%\" height=\"12\" align=\"center\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b> #</b></font></td>\n");
	printf("		<td width=\"92%\" height=\"12\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\"><b><a href=\"html.cgi?function=modem_activate\"> Modem Activation</a></b></font></td>\n");
	printf("	</tr>\n");
#endif
	printf("</table>\n");
}

void html_menu_head_gate(struct protocol_web_status *p_status, unsigned char center, unsigned char led, char *page_title)
{
	char cascade_code_str[51];
	char cascade_svc_str[16];
	int i;

    printf("<form name = \"form0\">\n");
if(center){
	printf("	<table width=\"480\" border=\"1\" height=\"355\"align=\"center\">\n");
}else{
	printf("	<table width=\"480\" border=\"1\" height=\"355\">\n");
}
	printf("		<tr bgcolor=\"#CCCCCC\">\n");
	printf("			<td width=\"44%\" rowspan=\"2\" nowrap>\n");
	printf("				<div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b><font size=\"6\">SmartCell</br>System</div>\n");
	printf("				<font size=\"3\"></p>\n");
if(led){
	printf("					Alarm <input type=\"text\" name=\"alarm_status\" style=\"WIDTH: 20pt; HEIGHT: 12pt; BACKGROUND-COLOR: #000000; border:0\" readonly>\n");
}
	printf("					&nbsp;&nbsp;\n");
if(led){
	printf("					Shutdown <input type=\"text\" name=\"shutdown_status\" style=\"WIDTH: 20pt; HEIGHT: 12pt; BACKGROUND-COLOR: #000000; border:0\" readonly>\n");
}
	printf("				</b></font></font></td>\n");
	printf("			<td height=\"50\" bgcolor=\"#CCCCCC\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\">Software Version : </br>&nbsp;&nbsp;\n");
	printf("				<input type=\"text\" name=\"txt_swversion\" style=\"WIDTH:120pt; HEIGHT:17pt; background:silver\" readonly value=\"%01d.%01d.%01d%01d\"></font></td>\n",p_status->ver_rcu>>4, p_status->ver_rcu&0xf, p_status->ver_rcu_minor>>4, p_status->ver_rcu_minor&0xf);
	printf("		</tr>\n");
	printf("		<tr>\n");
	printf("			<td height=\"50\" bgcolor=\"#CCCCCC\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\">Serial Number : </br>&nbsp;&nbsp;\n");
	printf("				<input type=\"text\" name=\"txt_sn\" style=\"WIDTH:120pt; HEIGHT:17pt; background:silver\" readonly value=\"%s\" ></font></td>\n",p_status->serial_no);
	printf("		</tr>\n");
	printf("		<tr bgcolor=\"#CCCCCC\">\n");
	printf("			<td rowspan=\"2\"><div align=\"center\"><font color=\"#000000\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><b><font size=\"5\">%s", page_title);
	printf("				</div></b></font></td>\n");
#if 0	/* sprint */
	printf("			<td height=\"50\" bgcolor=\"#CCCCCC\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\">Repeater Location: </br>\n");
	printf("				<div align=\"center\"><input type=\"text\" name=\"txt_cascadecode\" style=\"WIDTH:170pt; HEIGHT:15pt; background:silver\" readonly value=\"%s\"></font></td>\n",p_status->copy_location);
#else

	/*
	memset(cascade_code_str, 0, sizeof(cascade_code_str));
	for(i=0;i<4;i++) {
		if (p_status->status[i].svc_info == 4) {	// PCS
			memset(cascade_code_str, 0, sizeof(cascade_code_str));
			memcpy(cascade_code_str, p_status->status[i].cascade_code, sizeof(cascade_code_str));
			break;
		} else if (p_status->status[i].svc_info == 0) {	// blink
			continue;
		} else {
			memset(cascade_code_str, 0, sizeof(cascade_code_str));
			memcpy(cascade_code_str, p_status->status[i].cascade_code, sizeof(cascade_code_str));
			remember_slot = i;
		}
	}
	*/
	// 1st PCS SVC check
	memset(cascade_code_str, 0, sizeof(cascade_code_str));
	memset(cascade_svc_str, 0, sizeof(cascade_svc_str));
	for(i=0;i<4;i++) {
		if (p_status->status[i].svc_info == 4) {	// PCS
			memcpy(cascade_code_str, p_status->status[i].cascade_code, sizeof(cascade_code_str));
			sprintf(cascade_svc_str, "AMP%d %sM", i+1, service_name[p_status->status[i].svc_info]);
			break;
		} else if (p_status->status[i].svc_info == 0) {	// blink or etc
			continue;
		}
	}
	// 2st ETC SVC check
	if (i>3) {
		for(i=0;i<4;i++) {
			if (p_status->status[i].svc_info == 0) {	// blink
				continue;
			} else {
				memcpy(cascade_code_str, p_status->status[i].cascade_code, sizeof(cascade_code_str));
				sprintf(cascade_svc_str, "AMP%d %sM", i+1, service_name[p_status->status[i].svc_info]);
				break;
			}
		}
	}

	printf("			<td height=\"50\" bgcolor=\"#CCCCCC\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\">Cascade Code : %s</br>&nbsp;&nbsp;\n", cascade_svc_str);
	//printf("				<div align=\"center\"><input type=\"text\" name=\"txt_cascadecode\" style=\"WIDTH:170pt; HEIGHT:17pt; background:silver\" readonly value=\"%s\"></font></td>\n", cascade_code_str);
	printf("				<input type=\"text\" name=\"txt_cascadecode\" style=\"WIDTH:170pt; HEIGHT:17pt; background:silver\" readonly value=\"%s\"></font></td>\n", cascade_code_str);
#endif
	printf("		</tr>\n");
	printf("		<tr>\n");
#if 0	/* sprint */
	printf("			<td height=\"50\" bgcolor=\"#CCCCCC\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\">Installer Contact Information: </br>\n");
	printf("				<div align=\"center\"><input name=\"txt_longitude\" type=\"text\" style=\"WIDTH:170pt; HEIGHT:15pt; background:silver\" readonly value=\"%s\" ></div></font></td>\n",p_status->copy_contact_info);
#else
	printf("			<td height=\"50\" bgcolor=\"#CCCCCC\"><font face=\"Arial, Helvetica, sans-serif\" size=\"2\">Lat / Long : </br>&nbsp;&nbsp;\n");
	//printf("				<div align=\"center\"><input name=\"txt_longitude\" type=\"text\" style=\"WIDTH:170pt; HEIGHT:17pt; background:silver\" readonly value=\"%s %s\" ></div></font></td>\n", p_status->copy_latitude, p_status->copy_longitude);
	printf("				<input name=\"txt_longitude\" type=\"text\" style=\"WIDTH:170pt; HEIGHT:17pt; background:silver\" readonly value=\"%s / %s\" ></font></td>\n", p_status->copy_latitude, p_status->copy_longitude);
#endif
	printf("		</tr>\n");
	printf("	</table>");
	printf("</form>");
}


void html_menu_head(struct protocol_web_status *p_status,  char *page_title)
{
//char tmp[256];

	html_menu_head_left(p_status);
//sprintf(tmp, "%s, ver: %02X %02X", page_title, p_status->ver_rcu, p_status->ver_rcu_minor);
//debuglog(tmp);
	
	html_menu_head_gate(p_status,0, 1, page_title);
//sprintf(tmp, "%s, ver: %02X %02X", page_title, p_status->ver_rcu, p_status->ver_rcu_minor);
//debuglog(tmp);
}

void html_page_head(int redirect_action)
{
	printf("Content-type: text/html\r\n\r\n");
	printf("<html>\n");
	printf("<head>\n");
	printf("<title>Sprint SmartCell</title>\n");
	printf("<style type=\"text/css\">\n");
	printf("	body{\n");
	printf("		font-size:12px;\n");
	printf("		font-family:\"Arial, Helvetica, sans-serif\";\n");
	printf("		color:black;\n");
	printf("		line-height:20px;\n");
//	printf("		text-indent:20px;\n");
	printf("		border-style:ridge;\n");
	printf("	}\n");
	printf("	a { color:#333; text-decoration:none; }\n");
	printf("	a:hover { color: #fb8010; }\n");
	printf("</style>\n");
	printf("<script language=\"JavaScript\"> \n");
	printf("var time_id;\n");
	if (redirect_action) {
		printf("<!-- \n");
		printf("var requiredfrom = \"html.cgi\"; //  required prev. page\n");
		printf("if (document.referrer.indexOf(requiredfrom) == -1) {\n");
		printf("	alert(\"Sorry! You should register to access. Please go back to log-in.\" );\n");
		printf("	window.location=requiredfrom;\n");
		printf("}\n");
		printf("//--> \n");
	}
}


void send_command(int command_protocol, int return_value)
{
	struct protocol_web_status protocolstatus;
	int sockfd;
	int client;
	int n;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;
	unsigned char frame[sizeof(protocolstatus)*2];
	int frame_size;

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
	memset(&protocolstatus, 0, sizeof(protocolstatus));
	protocolstatus.type = command_protocol;

	protocol_buffer_to_frame((void*)&protocolstatus, 1,frame, &frame_size);
	sched_yield();
	sendto(sockfd, frame, 3,0,(struct sockaddr*)&serveraddr, client);

	if(return_value == TRUE)
	{
		alarm(30);
		n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
		alarm(0);
		if(n>0) return;
	}
}
	

void GetAlarmColorString(char* str, unsigned char st)
{
	if(st==0) sprintf(str, "lime"); 
	else if(st==1) sprintf(str, "yellow"); 
	else sprintf(str, "red");
}

int get_status(struct protocol_web_status *p_status)
{
	int ret=0;
	int sockfd; 
	int client;
	struct sockaddr_un serveraddr, clientaddr;
	struct protocol_web_status protocol_status;
	unsigned char frame[sizeof(protocol_status)*2];
	char protocol_buffer[sizeof(protocol_status)*2];
	int frame_size, buffer_size;
	int n;
	unsigned short crc_data = 0;

	client = sizeof(serveraddr);
	sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		message_dialog("socket error");
		ret=-1;
		goto RETURN;
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
		ret=-1;
		goto RETURN;
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

	crc_data = crc((void*)&protocol_status.type, (void*)&protocol_status.crc);
	if(crc_data != protocol_status.crc)
	{
		message_dialog("crc error - get_status()");
		ret=-1;
		goto RETURN;
	}
	memcpy(p_status, &protocol_status, sizeof(protocol_status));

RETURN:
	if(sockfd) close(sockfd);
	return ret;
}

int get_communication(struct protocol_communication *p_communications)
{
	int ret=0;
	int sockfd; 
	int client;
	struct sockaddr_un serveraddr, clientaddr;
	struct protocol_communication protocol_communication;
	unsigned char frame[sizeof(struct protocol_communication)*2];
	char protocol_buffer[sizeof(struct protocol_communication)*2];
	int frame_size, buffer_size;
	int n;
	unsigned short crc_data = 0;

	client = sizeof(serveraddr);
	sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		message_dialog("socket error");
		ret=-1;
		goto RETURN;
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
		ret=-1;
		goto RETURN;
	}
	memset(&protocol_communication, 0, sizeof(protocol_communication));
	protocol_communication.type=PROTOCOL_COMMUNICATIONS_READ;
	protocol_buffer_to_frame((void*)&protocol_communication,1,frame, &frame_size);
	sched_yield();
	sendto(sockfd, frame, 3,0,(struct sockaddr*)&serveraddr, client);
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);
	protocol_frame_to_buffer(frame, n, (unsigned char*)protocol_buffer, &buffer_size);
	memmove(&protocol_communication,protocol_buffer,buffer_size);

	crc_data = crc((void*)&protocol_communication.type, (void*)&protocol_communication.crc);
	if(crc_data != protocol_communication.crc)
	{
		message_dialog("crc error - get_communication()");
		ret=-1;
		goto RETURN;
	}
	memcpy(p_communications, &protocol_communication, sizeof(protocol_communication));

RETURN:
	if(sockfd) close(sockfd);
	return ret;
}

int get_alarmconfig(struct protocol_web_alarm *p_web_alarm)
{
	int ret=0;
	int sockfd; 
	int client;
	struct sockaddr_un serveraddr, clientaddr;
	struct protocol_web_alarm protocol_web_alarm;
	unsigned char frame[sizeof(struct protocol_web_alarm)*2];
	char protocol_buffer[sizeof(struct protocol_web_alarm)*2];
	int frame_size, buffer_size;
	int n;
	unsigned short crc_data = 0;

	client = sizeof(serveraddr);
	sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		message_dialog("socket error");
		ret=-1;
		goto RETURN;
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
		ret=-1;
		goto RETURN;
	}
	memset(&protocol_web_alarm, 0, sizeof(protocol_web_alarm));
	protocol_web_alarm.type=PROTOCOL_ALARMCONFIGURATION_READ;
	protocol_buffer_to_frame((void*)&protocol_web_alarm,1,frame, &frame_size);
	sched_yield();
	sendto(sockfd, frame, 3,0,(struct sockaddr*)&serveraddr, client);
	alarm(30);
	n = recvfrom(sockfd, frame, sizeof(frame), 0, NULL, NULL);
	alarm(0);
	protocol_frame_to_buffer(frame, n, (unsigned char*)protocol_buffer, &buffer_size);
	memmove(&protocol_web_alarm,protocol_buffer,buffer_size);

	crc_data = crc((void*)&protocol_web_alarm.type, (void*)&protocol_web_alarm.crc);
	if(crc_data != protocol_web_alarm.crc)
	{
		message_dialog("crc error - get_alarmconfig()");
		ret=-1;
		goto RETURN;
	}
	memcpy(p_web_alarm, &protocol_web_alarm, sizeof(protocol_web_alarm));

RETURN:
	if(sockfd) close(sockfd);
	return ret;
}


