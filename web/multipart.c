#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include "multipart.h"

int isblank(int c);

char* boundary = NULL;

/*----------------------------------------------------------------------------*/

//static char character;

//static int ischaracter(int c)
//{
//	return c == character;
//}

static char* string;

static int isstring(int c)
{
	if(!*string)
		return 0;
	if(*string == c)
	{
		string++;
		return 1;
	}
	return 0;
}

static int istspecial(int c);

static int istoken(int c)
{
	return isascii(c) && !isspace(c) && !iscntrl(c) && !istspecial(c);
}

static int istspecial(int c)
{
	switch(c)
	{
	case '(':
	case ')':
	case '<':
	case '>':
	case '@':
	case ',':
	case ';':
	case ':':
	case '\\':
	case '"':
	case '/':
	case '[':
	case ']':
	case '?':
	case '=':
		return 1;
	}
	return 0;
}

static int isqtext(int c)
{
	switch(c)
	{
	case '"':
	//case '\\':
	case '\r':
		return 0;
	}
	return 1;
}

static char* concatenate(char** parsed, int parsed_size)
{
	int i;
	char* string = NULL;
	int length = 0;
	for(i = 0; i < parsed_size; i++)
		if(parsed[i])
			length += strlen(parsed[i]);
	string = malloc(length + 1);
	length = 0;
	for(i = 0; i < parsed_size; i++)
		if(parsed[i])
		{
			strcpy(string + length, parsed[i]);
			length += strlen(parsed[i]);
		}
	return string;
}

static void list_free(char** parsed, int parsed_size)
{
	int i;
	for(i = 0; i < parsed_size; i++)
		if(parsed[i])
			free(parsed[i]);
	if(parsed)
		free(parsed);
}

/*----------------------------------------------------------------------------*/

void input_initial(struct input* input)
{
	memset(input, 0, sizeof(*input));
}

void input_final(struct input* input)
{
	if(input->file)
		fclose(input->file);
	if(input->buffer)
		free(input->buffer);
}

inline int input_get(struct input* input)
{
	int c = EOF;
	if(input->buffer && input->position < input->buffer_size)
	{
		c = input->buffer[input->position];
		input->position++;
	}
	else
	{
		if(input->buffer)
		{
			free(input->buffer);
			input->buffer = NULL;
			input->buffer_size = 0;
			input->position = 0;
		}
		if(input->file)
			c = fgetc(input->file);
	}
	return c;
}

void input_unget(struct input* input, int c)
{
	char buffer[2] = { c, 0x00 };
	input_unget_string(input, buffer, 1);
}

void input_unget_string(struct input* input, char* string, int length)
{
	if(length <= input->position)
	{
		input->position -= length;
		memcpy(input->buffer + input->position, string, length);
	}
	else
	{
		char* buffer;
		int buffer_size;

		buffer_size = length + input->buffer_size - input->position;
		buffer = malloc(buffer_size + 1);
		memcpy(buffer, string, length);
		buffer[buffer_size] = '\0';
		if(input->buffer)
		{
			memcpy(buffer + length, input->buffer + input->position, input->buffer_size - input->position);
			free(input->buffer);
		}
		input->buffer = (unsigned char*)buffer;
		input->buffer_size = buffer_size;
	}
}

void input_unget_list(struct input* input, char** parsed, int parsed_size)
{
	int i;
	for(i = parsed_size - 1; i >= 0; i--)
		if(parsed[i])
			input_unget_string(input, parsed[i], strlen(parsed[i]));
}

void parameter_initial(struct parameter* parameter)
{
	memset(parameter, 0, sizeof(*parameter));
}

void parameter_final(struct parameter* parameter)
{
	if(parameter->attribute)
		free(parameter->attribute);
	if(parameter->value);
		free(parameter->value);
}

static int parse(char** r_string, int size, struct input* input, int (*is)(int));

static int string_parse(char** r_string, struct input* input, char* attribute)
{
	int result;
	int length;

	length = strlen(attribute);
	string = attribute;
	result = parse(&string, INT_MAX, input, isstring);
	if(length != result)
	{
		input_unget_string(input, string, result);
		if(r_string)
			*r_string = NULL;
		if(string)
			free(string);
		result = 0;
	}
	else
	{
		if(r_string)
			*r_string = string;
		else
		if(string)
			free(string);
	}
	return result;
}

static int parse(char** r_string, int size, struct input* input, int (*is)(int))
{
	char* string = NULL;
	int string_size = 0;
	int c;
	int i;

	for(i = 0, string = realloc(string, string_size + 1);
		i < size && EOF != (c = input_get(input)) && is(c);
		i++, string_size++, string = realloc(string, string_size + 1))
			string[string_size] = c;
	if(i < size && EOF != c)
		input_unget(input, c);

	if(string_size == 0 && (string))
	{
		free(string);
		string = NULL;
	}
	if(string)
	{
		if(r_string)
		{
			string[string_size] = '\0';
			*r_string = string;
		}
		else
			free(string);
	}
	else
	{
		if(r_string)
			*r_string = NULL;
	}
	return string_size;
}

static int token_parse(char** r_token, struct input* input)
{
	return parse(r_token, INT_MAX, input, istoken);
}

static int qtext_parse(char** r_qtext, struct input* input)
{
	return parse(r_qtext, INT_MAX, input, isqtext);
}

static int x_token_parse(char** r_x_token, struct input* input)
{
	int count = 0;
	int result;
	char** parsed = NULL;
	int parsed_size = 0;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "x-")))
		if(0 >= (result = string_parse(parsed + parsed_size, input, "X-")))
			goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = token_parse(parsed + parsed_size, input)))
		goto RETURN;
	parsed_size++;
	count += result;

	result = count;
RETURN:
	if(0 >= result)
	{
		input_unget_list(input, parsed, parsed_size);
		if(r_x_token)
			*r_x_token = NULL;
	}
	else
	if(r_x_token)
		*r_x_token = concatenate(parsed, parsed_size);
	list_free(parsed, parsed_size);
	return result;
}

static int quoted_string_parse(char** r_quoted, struct input* input)
{
	int count = 0;
	int result;
	char** parsed = NULL;
	int parsed_size = 0;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "\"")))
		goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	result = qtext_parse(parsed + parsed_size, input);
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "\"")))
		goto RETURN;
	parsed_size++;
	count += result;

	result = count;
RETURN:	
	if(0 >= result)
	{
		input_unget_list(input, parsed, parsed_size);
		if(r_quoted)
			*r_quoted = NULL;
	}
	else
	if(r_quoted)
		*r_quoted = concatenate(parsed, parsed_size);
	list_free(parsed, parsed_size);
	return result;
}

static int blank_parse(char** r_blank, struct input* input)
{
	return parse(r_blank, INT_MAX, input, isblank);
}

int parameter_parse(char** r_string, struct parameter* parameter, struct input* input)
{
	int count = 0;
	int result;
	char** parsed = NULL;
	int parsed_size = 0;
	int attribute = -1;
	int value = -1;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = token_parse(parsed + parsed_size, input)))
		goto RETURN;
	attribute = parsed_size;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "=")))
		goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = token_parse(parsed + parsed_size, input)))
		if(0 >= (result = quoted_string_parse(parsed + parsed_size, input)))
			goto RETURN;
	value = parsed_size;
	parsed_size++;
	count += result;

	result = count;
RETURN:
	if(0 >= result)
	{
		input_unget_list(input, parsed, parsed_size);
		if(r_string)
			*r_string = NULL;
	}
	else
	{
		if(r_string)
			*r_string = concatenate(parsed, parsed_size);
		if(0 <= attribute)
		{
			parameter->attribute = parsed[attribute];
			parsed[attribute] = NULL;
		}
		if(0 <= value)
		{
			parameter->value = parsed[value];
			parsed[value] = NULL;
		}
	}
	list_free(parsed, parsed_size);
	return result;
}

void content_disposition_initial(struct content_disposition* disposition)
{
	memset(disposition, 0, sizeof(*disposition));
}

void content_disposition_final(struct content_disposition* disposition)
{
	if(disposition->type)
		free(disposition->type);
	if(disposition->parameter)
	{
		int i;
		for(i = 0; i < disposition->parameter_size; i++)
			parameter_final(disposition->parameter + i);
		free(disposition->parameter);
	}
}

int content_disposition_parse(char** r_string, struct content_disposition* disposition, struct input* input)
{
	int count = 0;
	int result;
	char** parsed = NULL;
	int parsed_size = 0;
	int type = -1;
	int i;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "Content-Disposition:")))
		goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 < (result = blank_parse(parsed + parsed_size, input)))
	{
		parsed_size++;
		count += result;
	}

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = token_parse(parsed + parsed_size, input)))
		goto RETURN;
	type = parsed_size;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	while(0 < (result = string_parse(parsed + parsed_size, input, ";")))
	{
		parsed_size++;
		count += result;

		parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
		if(0 < (result = blank_parse(parsed + parsed_size, input)))
		{
			parsed_size++;
			count += result;
		}

		parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
		disposition->parameter = realloc(disposition->parameter, sizeof(*disposition->parameter) * (disposition->parameter_size + 1));
		parameter_initial(disposition->parameter + disposition->parameter_size);
		if(0 >= (result = parameter_parse(parsed + parsed_size, disposition->parameter + disposition->parameter_size, input)))
			goto RETURN;
		
		disposition->parameter_size++;
		parsed_size++;
		count += result;

		parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	}
	result = count;
RETURN:
	if(0 >= result)
	{
		input_unget_list(input, parsed, parsed_size);
		if(r_string)
			*r_string = NULL;
		content_disposition_final(disposition);
		content_disposition_initial(disposition);
	}
	else
	{
		if(r_string)
			*r_string = concatenate(parsed, parsed_size);
		if(0 <= type)
		{
			disposition->type = parsed[type];
			parsed[type] = NULL;
		}
		for(i = 0; i < disposition->parameter_size; i++)
		{
			if(!strcmp("filename", disposition->parameter[i].attribute))
				disposition->filename = disposition->parameter[i].value;
			else
			if(!strcmp("creation-date", disposition->parameter[i].attribute))
				disposition->creation_date = disposition->parameter[i].value;
			else
			if(!strcmp("modification-date", disposition->parameter[i].attribute))
				disposition->modification_date = disposition->parameter[i].value;
			else
			if(!strcmp("read-date", disposition->parameter[i].attribute))
				disposition->read_date = disposition->parameter[i].value;
		}
	}
	list_free(parsed, parsed_size);
	return result;
}

void content_type_initial(struct content_type* type)
{
	memset(type, 0, sizeof(*type));
}

void content_type_final(struct content_type* type)
{
	if(type->type)
		free(type->type);
	if(type->subtype)
		free(type->subtype);
	if(type->parameter)
	{
		int i;
		for(i = 0; i < type->parameter_size; i++)
			parameter_final(type->parameter + i);
		free(type->parameter);
	}
}

int content_type_parse(char** r_type, struct content_type* content_type, struct input* input)
{
	int count = 0;
	int result;
	char** parsed = NULL;
	int parsed_size = 0;
	int type = -1;
	int subtype = -1;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "Content-Type:")))
		goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 < (result = blank_parse(parsed + parsed_size, input)))
	{
		parsed_size++;
		count += result;
	}

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = token_parse(parsed + parsed_size, input)))
		goto RETURN;
	type = parsed_size;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "/")))
		goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = token_parse(parsed + parsed_size, input)))
		goto RETURN;
	subtype = parsed_size;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	while(0 < (result = string_parse(parsed + parsed_size, input, ";")))
	{
		parsed_size++;
		count += result;

		parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
		if(0 < (result = blank_parse(parsed + parsed_size, input)))
		{
			parsed_size++;
			count += result;
		}

		parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
		content_type->parameter = realloc(content_type->parameter, sizeof(*content_type->parameter) * (content_type->parameter_size + 1));
		if(0 >= (result = parameter_parse(parsed + parsed_size, content_type->parameter + content_type->parameter_size, input)))
			goto RETURN;
		
		content_type->parameter_size++;
		parsed_size++;
		count += result;

		parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	}
	result = count;
RETURN:
	if(0 >= result)
	{
		input_unget_list(input, parsed, parsed_size);
		if(r_type)
			*r_type = NULL;;
		content_type_final(content_type);
		content_type_initial(content_type);
	}
	else
	{
		if(r_type)
			*r_type = concatenate(parsed, parsed_size);
		if(0 <= type)
		{
			content_type->type = parsed[type];
			parsed[type] = NULL;
		}
		if(0 <= subtype)
		{
			content_type->subtype = parsed[subtype];
			parsed[subtype] = NULL;
		}
	}
	list_free(parsed, parsed_size);
	return result;
}

static char* unquote(char* quoted)
{
	char* unquoted = quoted;
	if(unquoted[0] == '"')
	{
		int length;
		length = strlen(unquoted);
		unquoted = malloc(length - 2 + 1);
		memcpy(unquoted, quoted + 1, length - 2);
		unquoted[length - 2] = '\0';
		free(quoted);
	}
	return unquoted;
}

char* content_type_value(struct content_type* type, char* attribute)
{
	int i;
	for(i = 0; i < type->parameter_size; i++)
	{
		struct parameter* parameter = type->parameter + i;
		if(!strcmp(attribute, parameter->attribute))
			return parameter->value = unquote(parameter->value);
	}
	return NULL;
}

int content_transfer_encoding_parse(char** r_string, char** r_encoding, struct input* input)
{
	int count = 0;
	int result;
	char** parsed = NULL;
	int parsed_size = 0;
	int encoding = -1;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "Content-Transfer-Encoding:")))
		goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 < (result = blank_parse(parsed + parsed_size, input)))
	{
		parsed_size++;
		count += result;
	}

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "7bit")))
		if(0 >= (result = string_parse(parsed + parsed_size, input, "quoted-printable")))
			if(0 >= (result = string_parse(parsed + parsed_size, input, "base64")))
				if(0 >= (result = string_parse(parsed + parsed_size, input, "binary")))
					if(0 >= (result = x_token_parse(parsed + parsed_size, input)))
						goto RETURN;
	encoding = parsed_size;
	parsed_size++;
	count+= result;

	result = count;
RETURN:
	if(0 >= result)
	{
		input_unget_list(input, parsed, parsed_size);
		if(r_string)
			*r_string = NULL;
		if(r_encoding)
			*r_encoding = NULL;
	}
	else
	{
		if(r_string)
			*r_string = concatenate(parsed, parsed_size);
		if(0 <= encoding && r_encoding)
		{
			*r_encoding = parsed[encoding];
			parsed[encoding] = NULL;
		}
	}
	list_free(parsed, parsed_size);
	return result;
}

void form_data_initial(struct form_data* data)
{
	memset(data, 0, sizeof(*data));
}

void form_data_final(struct form_data* data)
{
	content_disposition_final(&data->disposition);
	if(data->type)
	{
		content_type_final(data->type);
		free(data->type);
	}
	if(data->encoding)
		free(data->encoding);
	if(data->content)
		free(data->content);
}

static int content_parse(unsigned char** r_content, int* r_content_size, struct input* input)
{
	int result = 0;
	int c;
	unsigned char* content = NULL;
	int content_size = 0;
	enum { buffer_size = 250 * 1024 };

	for(content = realloc(content, (content_size / buffer_size + 1) * buffer_size);
		EOF != (c = input_get(input));
		content_size++)
	{
		if('\r' == (content[content_size] = c))
		{
			if(0 < (result = string_parse(NULL, input, "\n--")))
			{
				if(0 < (result = string_parse(NULL, input, boundary)))
				{
					input_unget_string(input, boundary, strlen(boundary));
					input_unget_string(input, "\n--", strlen("\n--"));
					input_unget(input, '\r');
					content[content_size] = '\0';
					result = content_size;
					break;
				}
				else
				{
					input_unget_string(input, "\n--", strlen("\n--"));
				}
			}
			result = 0;
		}
		if(!((content_size + 1) % buffer_size))
		{
			content_size++;
			content = realloc(content, (content_size / buffer_size + 1) * buffer_size);
			content_size--;
		}
	}
	goto RETURN;
RETURN:
	if(0 >= result)
	{
		input_unget_string(input, (char*)content, content_size);
		if(content)
			free(content);
		if(r_content)
			*r_content = NULL;
		if(r_content_size)
			*r_content_size = 0;
	}
	else
	{
		if(r_content)
			*r_content = content;
		else
		if(content)
			free(content);
		if(r_content_size)
			*r_content_size = content_size;
	}
	return result;
}

int form_data_parse(char** r_string, struct form_data* data, struct input* input)
{
	int count = 0;
	int result;
	char** parsed = NULL;
	int parsed_size = 0;
	int content = -1;
	int content_size = 0;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = content_disposition_parse(parsed + parsed_size, &data->disposition, input)))
		goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "\r\n")))
		goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	data->type = malloc(sizeof(*data->type));
	content_type_initial(data->type);
	if(0 < (result = content_type_parse(parsed + parsed_size, data->type, input)))
	{
		parsed_size++;
		count += result;

		parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
		if(0 >= (result = string_parse(parsed + parsed_size, input, "\r\n")))
			goto RETURN;
		parsed_size++;
		count += result;
	}
	else
	{
		content_type_final(data->type);
		free(data->type);
		data->type = NULL;
	}

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 < (result = content_transfer_encoding_parse(parsed + parsed_size, &data->encoding, input)))
	{
		parsed_size++;
		count += result;

		parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
		if(0 >= (result = string_parse(parsed + parsed_size, input, "\r\n")))
			goto RETURN;
		parsed_size++;
		count += result;
	}

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "\r\n")))
		goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 < (result = content_parse((unsigned char**)parsed + parsed_size, &content_size, input)))
	{
		content = parsed_size;
		parsed_size++;
		count += result;
	}

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "\r\n--")))
		goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, boundary)))
		goto RETURN;
	parsed_size++;
	count += result;

	result = count;
RETURN:
	if(0 >= result)
	{
		input_unget_list(input, parsed, parsed_size);
		if(r_string)
			*r_string = NULL;
		form_data_final(data);
		form_data_initial(data);
	}
	else
	{
		if(r_string)
			*r_string = concatenate(parsed, parsed_size);
		if(0 <= content)
		{
			data->content = (void*)parsed[content];
			parsed[content] = NULL;
			data->content_size = content_size;
		}
	}
	list_free(parsed, parsed_size);
	return result;
}

char* form_data_value(struct form_data* form_data, char* attribute)
{
	int i;
	for(i = 0; i < form_data->disposition.parameter_size; i++)
	{
		struct parameter* parameter = form_data->disposition.parameter + i;
		if(!strcmp(attribute, parameter->attribute))
			return parameter->value = unquote(parameter->value);
	}
	return NULL;
}

void multipart_initial(struct multipart* multipart)
{
	memset(multipart, 0, sizeof(*multipart));
}

void multipart_final(struct multipart* multipart)
{
	if(multipart->form_data)
	{
		int i;
		for(i = 0; i < multipart->form_data_size; i++)
			form_data_final(multipart->form_data + i);
		free(multipart->form_data);
	}
}

int multipart_parse(char** r_string, struct multipart* multipart, struct input* input)
{
	int count = 0;
	int result;
	char** parsed =  NULL;
	int parsed_size = 0;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "--")))
		goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, boundary)))
		goto RETURN;
	parsed_size++;
	count += result;

	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "\r\n")))
		goto RETURN;
	parsed_size++;
	count += result;

	while(1)
	{
		multipart->form_data = realloc(multipart->form_data, 
								sizeof(*multipart->form_data) 
								* (multipart->form_data_size + 1));
		form_data_initial(multipart->form_data + multipart->form_data_size);
		parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
		if(0 >= (result = form_data_parse(parsed + parsed_size, multipart->form_data + multipart->form_data_size, input)))
			break;
		multipart->form_data_size++;
		parsed_size++;
		count += result;

		parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
		if(0 >= (result = string_parse(parsed + parsed_size, input, "\r\n")))
			break;
		parsed_size++;
		count += result;
	}
	parsed = realloc(parsed, sizeof(*parsed) * (parsed_size + 1));
	if(0 >= (result = string_parse(parsed + parsed_size, input, "--")))
		goto RETURN;
	parsed_size++;
	count += result;

	result = count;
RETURN:
	if(0 >= result)
	{
		input_unget_list(input, parsed, parsed_size);
		if(r_string)
			*r_string = NULL;
		multipart_final(multipart);
		multipart_initial(multipart);
	}
	else
	if(r_string)
		*r_string = concatenate(parsed, parsed_size);
	list_free(parsed, parsed_size);
	return result;
}

static int quoted_compare(char* a, char* b)
{
	if((a[0] == '"' && b[0] == '"') || (a[0] != '"' && b[0] != '"'))
		return strcmp(a, b);
	else
	if(a[0] == '"')
	{
		int result;
		int length = strlen(b);
		result = strncmp(a + 1, b, length);
		if(result)
			return result;
		if(a[1 + length] == '"')
			return 0;
		return -1;
	}
	else
		return -1 * quoted_compare(b, a);
	return 0;
}

struct form_data* multipart_form_data(struct multipart* multipart, char* attribute, char* value)
{
	int i;
	for(i = 0; i < multipart->form_data_size; i++)
	{
		struct form_data* form_data = multipart->form_data + i;
		int i;
		for(i = 0; i < form_data->disposition.parameter_size; i++)
		{
			struct parameter* parameter = form_data->disposition.parameter + i;
			if(!strcmp(attribute, parameter->attribute))
			{
				if(!quoted_compare(value, parameter->value))
					return form_data;
			}
		}
	}
	return NULL;
}

