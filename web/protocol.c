#include "protocol.h"

unsigned protocol_buffer_to_frame(unsigned char* buffer, int buffer_size, unsigned char* frame, int* frame_size)
{
	int i;
	int j = 0;
	frame[j++] = 0x7E;
	for(i = 0; i < buffer_size; i++)
	{
		switch(buffer[i])
		{
		case 0x7E:
		case 0x7D:
			frame[j++] = 0x7D;
			frame[j++] = 0x20 ^ buffer[i];
			break;
		default:
			frame[j++] = buffer[i];
		}
	}
	frame[j++] = 0x7E;
	*frame_size = j;
	return 1;
}

unsigned protocol_frame_to_buffer(unsigned char* frame, int frame_size, unsigned char* buffer, int* buffer_size)
{
	int i;
	int j;
	if(frame[0] != 0x7E)
		return 0;
	if(frame[frame_size - 1] != 0x7E)
		return 0;
	for(i = 1, j = 0; i < frame_size - 1; i++, j++)
	{
		if(0x7D == frame[i])
		{
			i++;
			buffer[j] = 0x20 ^ frame[i];
		}
		else
			buffer[j] = frame[i];
	}
	*buffer_size = j;
	return 1;
}
