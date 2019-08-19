#include "server.h"

int main()
{
    struct sigaction sigint;
    sigint.sa_handler = SigintHandler;
    sigint.sa_flags = 0;
    sigemptyset(&sigint.sa_mask);
    sigaddset(&sigint.sa_mask, SIGINT);
    sigaction(SIGINT, &sigint, 0);
    key_t key[2];
    long type;
    int size, i, num_tmp, cycle = 1;
    struct msg_buf msg;
    key[0] = ftok("server", 'a');
    key[1] = ftok("server", 'b');
    if(key[0] == -1 || key[1] == -1)
    {
        perror("ftok");
        exit(-1);
    }
    id[0] = msgget(key[0], IPC_CREAT|0600);
    id[1] = msgget(key[1], IPC_CREAT|0600);
    if(id[0] == -1 || id[1] == -1)
    {
        perror("msgget1");
        exit(-1);
    }
    type = 1L;
    pthread_create(&tid, NULL, UsersList, NULL);
    while(cycle)
    {
        if(msgrcv(id[0], &msg, (sizeof(msg) - sizeof(long)), type, 0) == -1) //new message
        {
            perror("msgrcv");
        }
        size = sizeof(msg) - sizeof(long);
        for(i = 0; i < num; i++)
        {
            msg.type = arr_user[i].pid;
            if(msgsnd(id[0], &msg, size, 0) == -1)
            {
                perror("msgsnd");
            }
        }
    }
    pthread_join(tid, NULL);
    exit(0);
}

void SigintHandler(int sig)
{
    pthread_cancel(tid);
    pthread_join(tid, NULL);
    if(msgctl(id[0], IPC_RMID, 0) == -1)
    {
        perror("msgctl");
        exit(-1);
    }
    if(msgctl(id[1], IPC_RMID, 0) == -1)
    {
        perror("msgctl");
        exit(-1);
    }
    printf("\nServer out\n");
    exit(0);
}

void *UsersList()
{
    int size, i, change = 0;
    struct msg_user user;
    size = sizeof(user) - sizeof(long);
    while(1)
    {
        if(msgrcv(id[1], &user, size, 1L, 0) == -1)
        {
            perror("msgrcvlist");
        }
        if(user.flag == 1)
        {
            arr_user[num].pid = user.pid;
            strcpy(arr_user[num].name, user.buf);
            size = sizeof(user) - sizeof(long);
            user.type = user.pid;
            user.flag = 1;
            for(i = 0; i < num; i++)
            {
                memset(&user.buf[0], 0, sizeof(user.buf));
                strcpy(user.buf, arr_user[i].name);
                user.pid = arr_user[i].pid;
                if(msgsnd(id[1], &user, size, 0) == -1)
                {
                    perror("msgsndlist");
                }
                
            }
            memset(&user.buf[0], 0, sizeof(user.buf));
            strcpy(user.buf, arr_user[i].name);
            num ++;
            user.flag = 2;
            change = 1;
        }else{
            for(i = 0; i < num; i++)
            {
                if(arr_user[i].pid == user.pid)
                {
                    for(; i < num - 1; i++)
                    {
                        arr_user[i] = arr_user[i + 1];
                    }
                    num --;
                    user.flag = 0;
                    change = 1;
                    break;
                }
            }
        }
        if(change)
        {
            for(i = 0; i < num; i++)
            {
                user.type = arr_user[i].pid;
                if(msgsnd(id[1], &user, size, 0) == -1)
                {
                    perror("msgsndlist");
                }
            }
        }
        
    }
}
