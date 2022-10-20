#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../player.h"
#include <stdlib.h>
#include "ncurses.h"
#include <sys/file.h>
#include <errno.h>
#include <pthread.h>

pthread_mutex_t mutex;
sem_t sem;
int ruchPipe = 0;
int playerPipe = 0;
void* playerInput(void* args)
{
    struct player_t* player = (struct player_t*)args;
    while(1)
    {
        char ruch = getch();
        write(ruchPipe,&ruch,1);
        sem_wait(&sem);
    }
    

}
int main(void)
{
    sem_init(&sem,0,1);
    pthread_mutex_init(&mutex,NULL);
    pthread_t playerThread;
    initscr();
    start_color();
    init_pair(1,COLOR_RED,COLOR_RED);
    init_pair(2,COLOR_BLACK,COLOR_YELLOW);
    cbreak();
    noecho();
    int t1 = open("/home/riba/Desktop/SO2_GAME/tmp/ruch",O_WRONLY);
    if(t1 == -1)
    {
        printf("Error");
        exit(1);
    }
            pid_t pid = getpid();
    if(write(t1,&pid,sizeof(pid))==-1)
    {
        printf("Failed to read from fifo");
        return 1;
    }
    ruchPipe = t1;
    int t2 = open("/home/riba/Desktop/SO2_GAME/tmp/player",O_RDONLY);
    playerPipe = t2;
    struct player_t player;
    player.pid = 123;
    read(t2,&player,sizeof(struct player_t));
    if(pid == player.pid)
    {
        printf("Pid dobry\n");
        printf("%d %d",player.x,player.y);
    }
    pthread_create(&playerThread,NULL,&playerInput,&player);
int x = 0;
    while(1)
    {
        clear();
        x++;
        mvprintw(0,0,"petla leci %d",x);
        read(t2,&player,sizeof(struct player_t));
        mvprintw(1,0,"%25s",player.map_fragment);

        int z = 0;
        for(int i =player.y-2;i<=player.y+2;i++)
        {
            for(int j = player.x-2;j<=player.x+2;j++)
            {
                if(player.map_fragment[z] == '^')
                {
                    attron(COLOR_PAIR(1));
                    mvaddch(i,j,'^');
                    attroff(COLOR_PAIR(1));   
                }
                else
                {
                   mvaddch(i,j,player.map_fragment[z] ); 

                }
                z++;
            } 
        } 
        sem_post(&sem);
        refresh();
    }

    pthread_join(playerThread,NULL);

    endwin();
}
