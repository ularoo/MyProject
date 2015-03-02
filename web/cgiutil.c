#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cgiutil.h"
#include "main.h"

#define LF      10  /* Line Feed 문자 */
#define CR      13  /* Carriage Return 문자 */
#define EOT     4   /* End Of Text */


// 특정 문자(stop)까지 line 문자열을 읽어 word로 리턴, 
//   line은 특정 문자의 다음 문자를 기준으로 문자열을 line에 리턴 

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


// 특정 문자(stop)까지 line 문자열을 읽어 함수의 리턴값으로 문자열을 리턴, 
//   line은 특정 문자의 다음 문자를 기준으로 문자열을 line에 리턴 

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


// 파일스트림에서 stop 문자 전까지 문자열을 읽어, 읽은 문자열은 함수의 리턴값으로 전
//달하고, 읽은 만큼 cl를 줄여 주는 함수 

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


// fmakeword와 기본 동작 원리는 같은데, 차이점은 첫 번째 인수에 파일포인터대신
//   문자열에 대한 포인터가 전달된다는 것만 다르다. 

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

        // 특정 문자(stop)를 만났거나, str이 NUL이거나 cl이 0이면 word값을
        //   함수의 리턴값으로 전달  

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

// 16진수값을 문자로 변환시켜 리턴 
char x2c(char *what) 
{
    register char digit;

    digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A') + 10 : (what[0] - '0'));
    digit *= 16;
    digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A') + 10 : (what[1] - '0'));

    return(digit);
}

// %로 시작하는 1바이트 16진수값을 해당 문자값으로 바꾸어 다시 인자로 리턴 
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

// '+' 문자를 공백 문자로 변환 
void plustospace(char *str) 
{
    register int x;

    for(x =0; str[x]; x++)
        if(str[x] == '+')
            str[x] = ' ';
}

// 문자열 s에서 특정 문자 c와 같은 문자의 위치를 앞에서부터 찾아 리턴하고,
// 그렇지 않으면 -1을 리턴 
int ind(char *s, char c) 
{
    register int x;

    for(x = 0; s[x]; x++)
        if(s[x] == c)
            return x;

    return -1;
}

// 문자열 s에서 특정 문자 c와 같은 문자의 위치를 뒤에서부터 찾아 리턴하고,
//   그렇지 않으면 -1을 리턴 

int rind(char *s, char c) 
{
    register int x;

    for(x = strlen(s) - 1; x != -1; x--)
        if(s[x] == c) 
            return x;

    return -1;
}


// 파일스트림에서 n바이트만큼 데이터를 읽어 s로 리턴하고,
//   만약 파일의 끝에 도달했으면, 1을 그렇지 않으면 0을 리턴 

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


/* 파일스트림 f에서 fd로 데이터 복사 */
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

/* cmd[x]가 지정된 특수문자에 속하면, 그 문자는 모두 앞에 \\를 붙여
   특수문자로 인식할 수 없도록 한다 */

void escape_shell_cmd(char *cmd) 
{
    register int x, y, l;

    l = strlen(cmd);

    for(x = 0; cmd[x]; x++) 
	{
        /* 특수문자가 cmd[x]에 포함돼 있으면 */
        if(ind("&;`'\"|*?~<>^()[]{}$\\", cmd[x]) != -1) 
		{

            /* l+1은 '\0'을 가리키며 */
            for(y = l + 1; y > x; y--)
                cmd[y] = cmd[y - 1];

            cmd[x] = '\\';

            /* l은 '\\'문자가 하나 추가됐으므로, 전체 문자열의 길이를 하나 증가 */
            l++; 
            x++; /* 현재 포함된 \\문자를 한번 건너뛴 다음 */
        }
    }
}

/* 특수문자를 제거한 후 그 값을 리턴 */
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

/* CR+LF 문자 제거 */ 
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

/* 에러메시지 출력 */
void prtErrMsg(char *str)
{
    printf("<FONT FACE=\"굴림체\">에러: %s</FONT>", str);
    printf("<HR>\n");
    back();
    exit(-1);
}

/* 전 화면을 이동 */
void back(void)
{
    printf("<HR SIZE=3>\n");
    printf("<A HREF=\"JavaScript:history.back()\">전화면/A>\n");
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

