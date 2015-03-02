#ifndef __debug_h__
#define __debug_h__ __debug_h__

//#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>

extern int sockfd;
extern int client;
extern struct sockaddr_un serveraddr;
extern struct sockaddr_un clientaddr;

#define ____ \
{ \
	struct protocol_debug protocol_debug; \
	unsigned char frame[sizeof(protocol_debug)*2]; \
	int frame_size; \
	if(-1 == sockfd) \
	{ \
		client = sizeof(serveraddr); \
		sockfd = socket(PF_UNIX, SOCK_DGRAM, 0); \
		if(sockfd < 0) \
		{ \
			message_dialog("socket error"); \
				exit(0); \
		} \
		bzero(&serveraddr, sizeof(serveraddr)); \
		serveraddr.sun_family = AF_UNIX; \
		strcpy(serveraddr.sun_path, "/tmp/socket_app"); \
 \
		bzero(&clientaddr, sizeof(clientaddr)); \
		clientaddr.sun_family = AF_UNIX; \
		strcpy(clientaddr.sun_path, "/tmp/socket_web"); \
		remove("/tmp/socket_web"); \
 \
		if(bind(sockfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr))) \
		{ \
			message_dialog("socket error"); \
			exit(0); \
		} \
	} \
 \
	protocol_debug.type = PROTOCOL_DEBUG; \
	sprintf(protocol_debug.debug, "%s %d %s\n", __FILE__, __LINE__, __FUNCTION__); \
	protocol_debug.crc = crc((void*)&protocol_debug.type, (void*)&protocol_debug.crc); \
	protocol_buffer_to_frame((void*)&protocol_debug,sizeof(protocol_debug),frame, &frame_size); \
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client); \
}

#define debug(format, __ARGS__) \
{ \
	struct protocol_debug protocol_debug; \
	unsigned char frame[sizeof(protocol_debug)*2]; \
	int frame_size; \
	if(-1 == sockfd) \
	{ \
		client = sizeof(serveraddr); \
		sockfd = socket(PF_UNIX, SOCK_DGRAM, 0); \
		if(sockfd < 0) \
		{ \
			message_dialog("socket error"); \
				exit(0); \
		} \
		bzero(&serveraddr, sizeof(serveraddr)); \
		serveraddr.sun_family = AF_UNIX; \
		strcpy(serveraddr.sun_path, "/tmp/socket_app"); \
 \
		bzero(&clientaddr, sizeof(clientaddr)); \
		clientaddr.sun_family = AF_UNIX; \
		strcpy(clientaddr.sun_path, "/tmp/socket_web"); \
		remove("/tmp/socket_web"); \
 \
		if(bind(sockfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr))) \
		{ \
			message_dialog("socket error"); \
			exit(0); \
		} \
	} \
 \
	protocol_debug.type = PROTOCOL_DEBUG; \
	sprintf(protocol_debug.debug, format, __ARGS__); \
	protocol_debug.crc = crc((void*)&protocol_debug.type, (void*)&protocol_debug.crc); \
	protocol_buffer_to_frame((void*)&protocol_debug,sizeof(protocol_debug),frame, &frame_size); \
	sendto(sockfd, frame, frame_size,0,(struct sockaddr*)&serveraddr, client); \
}

#define GOTO_ERROR { ____; goto ERROR; }

/*
#define ____   \
{  \
	char buffer[512]; \
	sprintf(buffer, "%s %d %s", __FILE__, __LINE__, __FUNCTION__); \
	syslog(LOG_CRIT, buffer, strlen(buffer)); \
}
*/

#endif
