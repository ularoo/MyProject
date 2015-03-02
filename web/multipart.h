#ifndef __multipart_h__
#define __multipart_h__ __multipart_h__

#include <stdio.h>

struct input
{
	FILE* file;
	unsigned char* buffer;
	int buffer_size;
	int position;
};

void input_initial(struct input* input);
void input_final(struct input* input);
int input_get(struct input* input);
void input_unget(struct input* input, int c);
void input_unget_string(struct input* input, char* string, int length);

struct parameter
{
	char* attribute;
	char* value;
};

void parameter_initial(struct parameter* parameter);
void parameter_final(struct parameter* parameter);
int parameter_parse(char** r_parameter, struct parameter* parameter, struct input* input);

struct content_disposition
{
	char* type;
	char* filename;
	char* creation_date;
	char* modification_date;
	char* read_date;
	char* size;
	struct parameter* parameter;
	int parameter_size;
};

void content_disposition_initial(struct content_disposition* content_disposition);
void content_disposition_final(struct content_disposition* content_disposition);
int content_disposition_parse(char** r_stirng, struct content_disposition* content_disposition, struct input* input);

struct content_type
{
	char* type;
	char* subtype;
	struct parameter* parameter;
	int parameter_size;
};

void content_type_initial(struct content_type* content_type);
void content_type_final(struct content_type* content_type);
int content_type_parse(char** r_string, struct content_type* type, struct input* input);
char* content_type_value(struct content_type* type, char* attribute);

int content_transfer_encoding_parse(char** string, char** encoding, struct input* input);

extern char* boundary;

struct form_data
{
	struct content_disposition disposition;
	struct content_type* type;
	char* encoding;
	void* content;
	int content_size;
};

void form_data_initial(struct form_data* form_data);
void form_data_final(struct form_data* form_data);
int form_data_parse(char** r_string, struct form_data* form_data, struct input* input);
char* form_data_value(struct form_data* form_data, char* attribute);

struct multipart
{
	struct form_data* form_data;
	int form_data_size;
};

void multipart_initial(struct multipart* multipart);
void multipart_final(struct multipart* multipart);
int multipart_parse(char** r_string, struct multipart* multipart, struct input* input);
struct form_data* multipart_form_data(struct multipart* multipart, char* attribute, char* value);

#endif
