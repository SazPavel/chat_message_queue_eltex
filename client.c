#include "client.h"

int main()
{
    key_t key[2];
    pid_t my_pid = getpid();
    pthread_t tid[2];
    int i, cycle = 1;
    msg_out.type = msg_in.type = 1L;
    key[0] = ftok("server", 'a');
    key[1] = ftok("server", 'b');
    if(key[0] == -1 || key[1] == -1)
    {
        perror("ftok");
        exit(-1);
    }
    id[0] = msgget(key[0], 0);
    id[1] = msgget(key[1], 0);
    if(id[0] == -1 || id[1] == -1)
    {
        perror("msgget");
        exit(-1);
    }
    printf("Enter name:\n");
    fgets(user.buf, sizeof(user.buf), stdin);
    strtok(user.buf, "\n");
    strcpy(msg_out.name, user.buf);
    user.type = 1L;
    user.pid = my_pid;
    user.flag = 1;
    if(msgsnd(id[1], &user, (sizeof(user) - sizeof(long)), 0) == -1)
    {
        if(msgctl(id[1], IPC_RMID, 0) == -1)
        {
            perror("msgctl");
            exit(-1);
        }
        perror("msgsnd");
        exit(-1);
    }
    
    pthread_create(&tid[0], NULL, UsersList, &my_pid);
    pthread_create(&tid[1], NULL, AcceptMessage, &my_pid);

    initscr();
    cbreak();
    curs_set(0);
    getmaxyx(stdscr, rows, cols);
    chat = ChatCreateArr();
    wnd = newwin(rows, cols, 0, 0);
    listwnd = newwin(rows - 2, cols/3 + 1, 0, 2*cols/3);
    chatwnd = newwin(rows - 4, 2*cols/3-1, 1, 1);
    chatboxwnd = newwin(rows - 2, 2*cols/3 + 1, 0, 0);
    while(cycle)
    {
        print();
        getnstr(msg_out.buf, sizeof(msg_out.buf));
        strtok(msg_out.buf, "\n");
        msg_out.type = 1L;
        if(msgsnd(id[0], &msg_out, (sizeof(msg_out) - sizeof(long)), 0) == -1)
        {
            perror("msgsnd");
        }
        if(!strcmp(msg_out.buf, "exit"))
        {
            cycle = 0;
            pthread_cancel(tid[0]);
            pthread_cancel(tid[1]);
            for(i = 0; i < rows - 4; i++)
                free(chat[i]);
            free(chat);
            strcpy(user.buf, msg_out.name);
            user.type = 1L;
            user.pid = my_pid;
            if(msgsnd(id[1], &user, (sizeof(user) - sizeof(long)), 0) == -1)
            {
                if(msgctl(id[1], IPC_RMID, 0) == -1)
                {
                    perror("msgctl");
                    exit(-1);
                }
                perror("msgsnd");
                exit(-1);
            }
            delwin(listwnd);
            delwin(chatboxwnd);
            delwin(chatwnd);
            delwin(wnd);
            endwin();
        }
    }
    exit(0);
}

void print()
{
    int i;
    pthread_mutex_lock(&lock_print);
    clear();
    wclear(listwnd);
    wclear(chatwnd);
    box(wnd, '|', '-');
    box(listwnd, '|', '-');
    box(chatboxwnd, '|', '-');
    pthread_mutex_lock(&lock_name_arr);
    for(i = 0; i < num; i++)
        mvwprintw(listwnd, i + 1, 1, "%s", all_user[i].name);
    pthread_mutex_unlock(&lock_name_arr);
    pthread_mutex_lock(&lock_chat);
    for(i = 0; i < chat_line; i++)
        mvwprintw(chatwnd, i, 0, "%s", chat[i]);
    pthread_mutex_unlock(&lock_chat);
    mvwprintw(wnd, rows - 2, 2, "%s", "Enter text: ");
    refresh();
    wrefresh(wnd);
    wrefresh(listwnd);
    wrefresh(chatboxwnd);
    wrefresh(chatwnd);
    move(rows - 2, 14);
    pthread_mutex_unlock(&lock_print);
}

void *UsersList(void *ptr)
{
    pid_t *my_pid = (pid_t*)ptr;
    int i;
    while(1)
    {
        if(msgrcv(id[1], &user, (sizeof(user) - sizeof(long)), *my_pid, 0) == -1)
        {
            perror("msgrcvlist");
        }
        if(user.flag >= 1)            //add
        {
            pthread_mutex_lock(&lock_name_arr);
            strcpy(all_user[num].name, user.buf);
            all_user[num].pid = user.pid;
            num++;
            pthread_mutex_unlock(&lock_name_arr);
            if(user.flag == 2)
            {
                print();
            }
        }else{                      //delete
            pthread_mutex_lock(&lock_name_arr);
            for(i = 0; i < num; i++)
            {
                if(all_user[i].pid == user.pid)
                {
                    for(; i < num; i++)
                    {
                        all_user[i] = all_user[i + 1];
                    }
                    num --;
                    break;
                }
            }
            pthread_mutex_unlock(&lock_name_arr);
            print();
        }
    }
}

void *AcceptMessage(void *ptr)
{
    int i;
    pid_t *my_pid = (pid_t*)ptr;
    while(1)
    {
        if(msgrcv(id[0], &msg_in, (sizeof(msg_in) - sizeof(long)), *my_pid, 0) == -1)
        {
            perror("msgrcvmessage");
        }
        pthread_mutex_lock(&lock_chat);
        if(chat_line == rows - 4)
        {
            chat_line --;
            for(i = 0; i < chat_line; i++)
                strcpy(chat[i], chat[i+1]);
        }
        strcpy(chat[chat_line], msg_in.name);
        strcat(chat[chat_line], ": ");
        strcat(chat[chat_line], msg_in.buf);
        chat_line ++;
        pthread_mutex_unlock(&lock_chat);
        print();
    }
}

char** ChatCreateArr()
{
    int i;
    char **a;
    a = malloc((rows - 4) * sizeof(char*));
    for(i = 0; i < rows - 4; i++)
    {
        a[i] = malloc(32 * sizeof(char));
    }
    return a;
}
