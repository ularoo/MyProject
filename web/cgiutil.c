#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cgiutil.h"
#include "main.h"

#define LF      10  /* Line Feed ���� */
#define CR      13  /* Carriage Return ���� */
#define EOT     4   /* End Of Text */


// Ư�� ����(stop)���� line ���ڿ��� �о� word�� ����, 
//   line�� Ư�� ������ ���� ���ڸ� �������� ���ڿ��� line�� ���� 

void getword(char *word, char *line, char stop)
{
    int x = 0, y;

    for(x = 0; ((line[x]) && (line[x] != stop)); x++)
        word[x] = line[x];

    word[x] = '\0';

    if(line[x])
        ++x;

    y = 0;

    while((line[y++]=line[x++]));
}


// Ư�� ����(stop)���� line ���ڿ��� �о� �Լ��� ���ϰ����� ���ڿ��� ����, 
//   line�� Ư�� ������ ���� ���ڸ� �������� ���ڿ��� line�� ���� 

char *makeword(char *line, char stop)
{
	int x = 0, y;
	char *word = (char *)malloc(sizeof(char) * (strlen(line) + 1));
	if(!(word))
	{ 
		message_dialog((char*)__FUNCTION__); 
		exit(-1); 
	}
	for(x = 0; ((line[x]) && (line[x] != stop)); x++)
		word[x] = line[x];

	word[x] = '\0';

	if(line[x])
		++x;

	y = 0;

	while((line[y++]=line[x++]));

	return word;
}


// ���Ͻ�Ʈ������ stop ���� ������ ���ڿ��� �о�, ���� ���ڿ��� �Լ��� ���ϰ����� ��
//���ϰ�, ���� ��ŭ cl�� �ٿ� �ִ� �Լ� 

char *fmakeword(FILE *f, char stop, int *cl)
{
    int i;
    int c;
    char* word = NULL;

	for(i = 0; (0 < *cl) && (EOF != (c = fgetc(f))); i++)
	{
        (*cl)--;
        if(!(word = realloc(word, ((i >> 10) + 1) << 10)))
        { message_dialog((char*)__FUNCTION__); exit(-1); }
        if(stop == (word[i] = (char)c))
            break;
    }

    if(!(word = realloc(word, i + 1)))
    { message_dialog((char*)__FUNCTION__); exit(-1); }
    word[i] = '\0';
    return word;
}


// fmakeword�� �⺻ ���� ������ ������, �������� ù ��° �μ��� ���������ʹ��
//   ���ڿ��� ���� �����Ͱ� ���޵ȴٴ� �͸� �ٸ���. 

char *smakeword(char *str, char stop, int *cl) 
{
    int wsize, ll;
    char *word;

    wsize = 102400;
    ll = 0;
	if(!(word = (char *)malloc(sizeof(char) * (wsize + 1))))
    { message_dialog((char*)__FUNCTION__); exit(-1); }
    while(1) 
	{
        word[ll] = *str++;

        if(ll == wsize) 
		{
            word[ll+1] = '\0';
            wsize += 102400;
            if(!(word = (char *)realloc(word, sizeof(char) * (wsize + 1))))
    		{ message_dialog((char*)__FUNCTION__); exit(-1); }
        }

        --(*cl);

        // Ư�� ����(stop)�� �����ų�, str�� NUL�̰ų� cl�� 0�̸� word����
        //   �Լ��� ���ϰ����� ����  

        if((word[ll] == stop) || (str == NULL || (!(*cl)))) 
		{
            if(word[ll] != stop)
                ll++;

            word[ll] = '\0';
            if(!(word = (char *) realloc(word, ll+1)))
			{ message_dialog((char*)__FUNCTION__); exit(-1); }
            return word;
        }
        ++ll;
    }
}

// 16�������� ���ڷ� ��ȯ���� ���� 
char x2c(char *what) 
{
    register char digit;

    digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A') + 10 : (what[0] - '0'));
    digit *= 16;
    digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A') + 10 : (what[1] - '0'));

    return(digit);
}

// %�� �����ϴ� 1����Ʈ 16�������� �ش� ���ڰ����� �ٲپ� �ٽ� ���ڷ� ���� 
void unescape_url(char *url) 
{
    register int x, y;

    for(x = 0, y = 0; url[y]; ++x,++y) 
	{
        if((url[x] = url[y]) == '%') 
		{
            url[x] = x2c(&url[y+1]);
            y += 2;
        }
    }

    url[x] = '\0';
}

// '+' ���ڸ� ���� ���ڷ� ��ȯ 
void plustospace(char *str) 
{
    register int x;

    for(x =0; str[x]; x++)
        if(str[x] == '+')
            str[x] = ' ';
}

// ���ڿ� s���� Ư�� ���� c�� ���� ������ ��ġ�� �տ������� ã�� �����ϰ�,
// �׷��� ������ -1�� ���� 
int ind(char *s, char c) 
{
    register int x;

    for(x = 0; s[x]; x++)
        if(s[x] == c)
            return x;

    return -1;
}

// ���ڿ� s���� Ư�� ���� c�� ���� ������ ��ġ�� �ڿ������� ã�� �����ϰ�,
//   �׷��� ������ -1�� ���� 

int rind(char *s, char c) 
{
    register int x;

    for(x = strlen(s) - 1; x != -1; x--)
        if(s[x] == c) 
            return x;

    return -1;
}


// ���Ͻ�Ʈ������ n����Ʈ��ŭ �����͸� �о� s�� �����ϰ�,
//   ���� ������ ���� ����������, 1�� �׷��� ������ 0�� ���� 

/*
int getline(char *s, int n, FILE *f) 

{

    register int i = 0;



    while(1) {

        s[i] = (char)fgetc(f);

        if(s[i] == CR)

            s[i] = fgetc(f);



        if((s[i] == EOT) || (s[i] == LF) || (i == (n - 1))) {

            s[i] = '\0';



            return(feof(f) ? 1 : 0);

        }



        ++i;

    }

}

*/


/* ���Ͻ�Ʈ�� f���� fd�� ������ ���� */
void send_fd(FILE *f, FILE *fd)
{
//    int num_chars = 0;
    char c;

    while (1) 
	{
        c = fgetc(f);

        if(feof(f))
            return;

        fputc(c, fd);
    }
}

/* cmd[x]�� ������ Ư�����ڿ� ���ϸ�, �� ���ڴ� ��� �տ� \\�� �ٿ�
   Ư�����ڷ� �ν��� �� ������ �Ѵ� */

void escape_shell_cmd(char *cmd) 
{
    register int x, y, l;

    l = strlen(cmd);

    for(x = 0; cmd[x]; x++) 
	{
        /* Ư�����ڰ� cmd[x]�� ���Ե� ������ */
        if(ind("&;`'\"|*?~<>^()[]{}$\\", cmd[x]) != -1) 
		{

            /* l+1�� '\0'�� ����Ű�� */
            for(y = l + 1; y > x; y--)
                cmd[y] = cmd[y - 1];

            cmd[x] = '\\';

            /* l�� '\\'���ڰ� �ϳ� �߰������Ƿ�, ��ü ���ڿ��� ���̸� �ϳ� ���� */
            l++; 
            x++; /* ���� ���Ե� \\���ڸ� �ѹ� �ǳʶ� ���� */
        }
    }
}

/* Ư�����ڸ� ������ �� �� ���� ���� */
char *rmCtrlChar(char *str)
{
    char *src, *fp;

    if(str) 
	{
        for(src = str, fp = str; *src; src++)
            if((*src & 0x7f) >= 0x20)
                *fp++ = *src;

        *fp = '\0';
    }

    return(str);
}

/* CR+LF ���� ���� */ 
char *rmCrLf(char *str)
{
    char *src, *dest;

    if(str) 
	{
        for(src = str, dest = str; *src; src++)

            if(src[0] != '\r' || src[1] != '\n')

                *dest++ = *src;

        *dest = '\0';
    }

    return(str);
}

/* �����޽��� ��� */
void prtErrMsg(char *str)
{
    printf("<FONT FACE=\"����ü\">����: %s</FONT>", str);
    printf("<HR>\n");
    back();
    exit(-1);
}

/* �� ȭ���� �̵� */
void back(void)
{
    printf("<HR SIZE=3>\n");
    printf("<A HREF=\"JavaScript:history.back()\">��ȭ��/A>\n");
}

void set_bit_in_array(unsigned char ary[], unsigned char idx, unsigned char val)
{
	unsigned char byte_idx, bit_idx;
	byte_idx = idx / 8;
	bit_idx = idx % 8;

	if(val)
	{
		ary[byte_idx] |= (1<<bit_idx);
	}
	else
	{
		ary[byte_idx] &= ~(1<<bit_idx);
	}
}

unsigned char get_bit_in_array(unsigned char ary[], unsigned char idx)
{
	unsigned char byte_idx, bit_idx;
	byte_idx = idx / 8;
	bit_idx = idx % 8;

	return (ary[byte_idx]&(1<<bit_idx))? 1: 0;
}

static int post_compare(const void* p1, const void* p2)
{
	return strcmp(
		((struct post*)p1)->name,
		((struct post*)p2)->name);
}

char* post_value(char* name)
{
	static struct post* sorted = NULL;
	struct post* found = NULL;
	struct post key = { name, NULL };

	if(!sorted)
	{
		qsort(post, post_size, sizeof(struct post), post_compare);
		sorted = post;
	}
	found = bsearch(&key, (void*)post, post_size, sizeof(struct post), post_compare);
	if(!found)
		GOTO_ERROR;
	return found->value;
ERROR:
	return NULL;
}

