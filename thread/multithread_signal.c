#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#define NUM_THREADS 3


typedef struct _thread_args
{
 pthread_t tId;
 void* (*func)();
}thread_args;

void *startSigThread(void*); // signal processing thread func
void *startThread(void*);    // worker thread func

salkdkj
sdjkn'sadjckasdl
cjsa
dcm
ksdmc
ksadmc
lsdanc
ojsadn
onwae
fn
asdnvcoasuhnf
oewhf]oaisnuefp
nab
inas
pu
baunf]awun80waufn]wahf]08wqf\-af]
void *startThread(void* pArgs)
{
 thread_args* tArgs = (thread_args*)pArgs;
 sigset_t tSigSetMask;
 int nSigNum;
 int nErrno;
 sigemptyset(&tSigSetMask);
 sigaddset(&tSigSetMask, SIGUSR2);
	struct sigaction act;
	act.sa_handler = sa_handler_usr;
	sigaction(SIGUSR2,&act,NULL);

 pthread_sigmask(SIG_SETMASK, &tSigSetMask, NULL);
 printf("* Start signal thread (tid = %lu)\n", (long)pthread_self());
 // 수신할 signal set setting


	
signal(SIGUSR1,sa_handler_usr);
 
 while(1)
 {
  // signal 대기
/*  nErrno = sigwait(&tSigSetMask, &nSigNum);
  if(nErrno >  0)
  {
   perror("sigwait error\n");
   return NULL;
  }
 */ // signal no. 에 따라 signal handler 함수 호출 혹은 종료

 }
 return tArgs;
}

void cleanThread(thread_args* pArgs)
{
 thread_args* tArgs;
 int i;

 for(i = 0; i < NUM_THREADS; i++)
 {
  pthread_join(pArgs->tId, (void**)&tArgs);
  printf("Thread id(%lu) joined\n", pArgs->tId);
  pArgs++;
 }
}

void sa_handler_usr(int nSigNum)
{
 printf("\t[%lu] Signal(%s)\n", 
   pthread_self(),
   nSigNum == SIGUSR1? "USR1":"USR2");
}

