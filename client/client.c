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
#include "../common.h"
#include "string.h"
pthread_mutex_t mutex;
sem_t sem;
int ruchPipe = 0;
int playerPipe = 0;

void generatePanel(struct player_t* player)
{
    for(int i = 0;i< PANEL_HEIGHT;i++)
    {
        for(int j = MAP_WIDTH; j < MAP_WIDTH+PANEL_WIDTH;j++)
        {
            mvaddch(i,j,' ');
        }
    }
    //printing serer and game info
    mvprintw(0,MAP_WIDTH+1,"Server's PID: %d",player->serverPid);
    mvprintw(1,MAP_WIDTH+1,"SCampsite X/Y: UNKOWN");
    mvprintw(2,MAP_WIDTH+1,"Round number: %d",player->round);    
    mvprintw(4,MAP_WIDTH+1,"player");
    mvprintw(5,MAP_WIDTH+1,"%d",player->id);
    mvprintw(6,MAP_WIDTH+1,"HUMAN");

        // mvprintw(4,MAP_WIDTH+1,"Parameter:   Player1  Player2  Player3  Player4 ");
    // mvprintw(5,MAP_WIDTH+1,"PID         %4d     %4d     %4d     %4d",(players+0)->pid,(players+1)->pid,(players+2)->pid,(players+3)->pid);
    // mvprintw(6,MAP_WIDTH+1,"Type         %s     %s      %s    %s",tab[(players+0)->type].data(),tab[(players+1)->type].data(),tab[(players+2)->type].data(),tab[(players+3)->type].data());
    // mvprintw(7,MAP_WIDTH+1,"Curr X/Y    %2d/%2d    %2d/%2d    %2d/%2d    %2d/%2d",(players+0)->x,(players+0)->y,(players+1)->x,(players+1)->y,(players+2)->x,(players+2)->y,(players+3)->x,(players+3)->y);
    // mvprintw(8,MAP_WIDTH+1,"Deaths        %d        %d        %d        %d",(players+0)->deaths,(players+1)->deaths,(players+2)->deaths,(players+3)->deaths);

    // //printing coins info
    // mvprintw(10,MAP_WIDTH+1,"Coins");
    mvprintw(11,MAP_WIDTH+1 + strlen("Coins"),"carried %2d ",player->currentCoins);
    mvprintw(12,MAP_WIDTH+1 + strlen("Coins"),"brought %2d ",player->collectedCoins);

    //printing legend
    mvprintw(16,MAP_WIDTH+1,"Legend:");
    mvprintw(17,MAP_WIDTH+3,"1234 - players");
    mvprintw(18,MAP_WIDTH+3,"[]    - wall");
    mvprintw(19,MAP_WIDTH+3,"#    - bushes (slow down)");
    mvprintw(20,MAP_WIDTH+3,"*    - wild beast");
    mvprintw(21,MAP_WIDTH+3,"c    - one coin                  D - dropped treasure");
    mvprintw(22,MAP_WIDTH+3,"t    - treasure (10 coins)");
    mvprintw(23,MAP_WIDTH+3,"T    - large treasure (50 coins)");
    mvprintw(24,MAP_WIDTH+3,"A    - campsite");


    attroff(COLOR_PAIR(PANEL));
}

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
int main(int argc,char** args)
{
    char* ruchP = args[1];
    char* playerP = args[2];
    sem_init(&sem,0,1);
    pthread_mutex_init(&mutex,NULL);
    pthread_t playerThread;
    initscr();
    start_color();
    init_pair(1,COLOR_RED,COLOR_RED);
    init_pair(2,COLOR_BLACK,COLOR_YELLOW);
    cbreak();
    noecho();

    int t1 = open(ruchP,O_WRONLY);
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
    int t2 = open(playerP,O_RDONLY);
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
        
        read(t2,&player,sizeof(struct player_t));
        generatePanel(&player);

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
