#ifndef __CGIUTIL_H__
#define __CGIUTIL_H__

void getword(char *word, char *line, char stop);
char *makeword(char *line, char stop);
char *fmakeword(FILE *f, char stop, int *cl);
char *smakeword(char *str, char stop, int *cl);
char x2c(char *what);
void unescape_url(char *url);
void plustospace(char *str);
int  ind(char *s, char c);
int  rind(char *s, char c);
//int  getline(char *s, int n, FILE *f);
void send_fd(FILE *f, FILE *fd);
void escape_shell_cmd(char *cmd);
char *rmCtrlChar(char *str);
char *rmCrLf(char *str);
void prtErrMsg(char *str);
void back(void);

void set_bit_in_array(unsigned char ary[], unsigned char idx, unsigned char val);
unsigned char get_bit_in_array(unsigned char ary[], unsigned char idx);

char* post_value(char* name);

#endif

