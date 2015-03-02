#include <stdio.h>
#include "protocol.h"
#include "main.h"

void dummy(int flag)
{
	int i;
	html_page_head(1);  // arg==requiredfrom true(1), false(0)
	printf("%s", "<!-- \n");
	printf("%s", "function logout()\n");
	printf("%s", "{\n");
	printf("%s", "  if (document.cookie != \"\") \n");
	printf("%s", "  {\n");
	printf("%s", "        thisCookie = document.cookie.split(\"; \")\n");
	printf("%s", "        expireDate = new Date\n");
	printf("%s", "        expireDate.setDate(expireDate.getDate()-1)\n");
	printf("%s", "        for (i=0; i<thisCookie.length; i++) \n");
	printf("%s", "      {\n");
	printf("%s", "          document.cookie = thisCookie + \";expires=\" + expireDate.toGMTString()\n");
	printf("%s", "      }\n");
	printf("%s", "  }\n");
	printf("%s", "  location.replace(\"html.cgi?function=logout\");\n");
	printf("%s", "}\n");
	printf("%s", "\n");
	printf("%s", "//--> \n");
	printf("%s", "</script>\n");

	printf("%s", "<title>Sprint SmartCell</title>\n");
	printf("%s", "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	if(flag == DOWNLOAD_PAGE)
		printf("%s", "<meta http-equiv=\"refresh\" content=\"0; url = html.cgi?function=system_download\">\n");
	else if(flag == TRANSFER_UPLOAD_PAGE)
		printf("%s", "<meta http-equiv=\"refresh\" content=\"0; url = html.cgi?function=transfer_except_upload\">\n");
	else if(flag == WRONG_FILE)
		printf("%s", "<meta http-equiv=\"refresh\" content=\"3; # logout()\">\n");

	printf("%s", "\n");
	printf("%s", "</head>\n");
	printf("%s", "\n");
	printf("%s", "<body bgcolor=\"#FFFFFF\" text=\"#000000\">\n");
	if(flag == WRONG_FILE)
	{
		printf("%s", "<font color=\"#FF0000\" size=\"3\" face=\"Arial, Helvetica, sans-serif\"><b>");
		printf("%s", "File can not be uploaded </br>\n");
	}

	for(i=0;i<1000;i++)
		printf("    ");

	printf("%s", "</body>");
	printf("%s", "</html>");
}
	



