#ifndef server_saz_h
#define server_saz_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>


struct msg_buf
{
    long type;
    char name[32];
    char buf[32];
};

struct msg_user
{
    long type;
    char buf[32];
    int pid;
    int flag;
};

struct user
{
    char name[32];
    int pid;
};

int  id[2], num = 0;
struct user arr_user[16];
pthread_t tid;

void SigintHandler(int sig);
void *UsersList();

#endif
