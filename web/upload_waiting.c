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

void upload_waiting()
{
//============================
//========1024*768 start========
//============================
	printf("%s", "Content-type: text/html\r\n\r\n");
	printf("%s", "<html>\n");
	printf("%s", "<head>\n");
	printf("%s", "<title>Sprint SmartCell</title>\n");
	printf("%s", "\n");
	printf("%s", "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("%s", "</head>\n");
	printf("%s", "\n");

    printf("%s", "<body bgcolor=\"#FFFFFF\" text=\"#000000\">\n");

	printf("%s", "<p align=center><b><font color=\"#000000\" size=\"5\" face=\"Arial, Helvetica, sans-serif\">Uploading File\n"); 
	printf("%s", "<p align=center><b><font color=\"#FF0000\" size=\"3\" face=\"Arial, Helvetica, sans-serif\">Do not turn off during upgrade\n"); 

//=========================================================

}
