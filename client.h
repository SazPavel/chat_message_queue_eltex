#ifndef client_saz_h
#define client_saz_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <curses.h>
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

struct msg_user user;
struct msg_buf msg_out;
struct msg_buf msg_in;
int id[2], num = 0, rows, cols, chat_line = 0;
char **chat;
struct user all_user[16];

WINDOW *wnd, *listwnd, *chatwnd, *chatboxwnd;
pthread_mutex_t lock_print = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_name_arr = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_chat = PTHREAD_MUTEX_INITIALIZER;

void print();
void *UsersList(void *ptr);
void *AcceptMessage(void *ptr);
char** ChatCreateArr();

#endif