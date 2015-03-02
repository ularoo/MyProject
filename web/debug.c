#include <sys/un.h>
#include "debug.h"

int sockfd = -1;
int client = 0;
struct sockaddr_un serveraddr;
struct sockaddr_un clientaddr;
