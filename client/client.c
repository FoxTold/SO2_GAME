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
#include <signal.h>

pthread_mutex_t mutex;
sem_t sem;
    pthread_t playerThread;

int ruchPipe = 0;
int playerPipe = 0;
int end = 0;
void generatePanel(struct player_t* player)
{
        attron(COLOR_PAIR(PANEL));

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
    mvprintw(5,MAP_WIDTH+1,"PID         %4d",player->pid);
    mvprintw(6,MAP_WIDTH+1,"Type         %s","PLAYER");
    mvprintw(7,MAP_WIDTH+1,"Curr X/Y    %2d/%2d",player->x,player->y);
    mvprintw(8,MAP_WIDTH+1,"Deaths        %d ",player->deaths);

    // //printing coins info
    mvprintw(10,MAP_WIDTH+1,"Coins");
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
int term = 0;
void* playerInput(void* args)
{
    char ruch = 'l';
    write(ruchPipe,&ruch,1);
    struct player_t* player = (struct player_t*)args;
    while(1)
    {
        
        if(term == 1)return NULL;
        
        ruch = getch();
        write(ruchPipe,&ruch,1);
        if(ruch == 'q')
        {
            endwin();
            end = 1;
            term = 1;
            char ruch = 'q';
            write(ruchPipe,&ruch,1);
            close(ruchPipe);
            close(playerPipe);
            system("clear");
            printf("Disconected from gameserver\n");
            endwin();
            exit(1);
        }
        sem_wait(&sem);
    }
    

}
void sig_handler(int signum){
    end = 1;

    term = 1;
    char ruch = 'q';
    write(ruchPipe,&ruch,1);
    close(ruchPipe);
    close(playerPipe);
    system("clear");
    printf("Disconected from gameserver\n");
    endwin();
    exit(1);
}

int connected = 0;
void* timer (void* args)
{
    sleep(3);
    if(!connected)
    {
        endwin();
        printf("Max players reached\n");
        exit(1);
    }
}

int main(int argc,char** args)
{

    signal(SIGINT,sig_handler);
    signal(SIGQUIT,sig_handler);
    signal(SIGTSTP,sig_handler);
    signal(0,sig_handler);

    char* ruchP = args[1];
    char* playerP = args[2];
    sem_init(&sem,0,1);
    pthread_mutex_init(&mutex,NULL);
    initscr();
    start_color();
    init_pair(1,COLOR_RED,COLOR_RED);
    init_pair(2,COLOR_BLACK,COLOR_YELLOW);
    cbreak();
    noecho();

    int t1 = open(ruchP,O_WRONLY);
    if(t1 == -1)
    {
        printf("START SERVER FIRST....");
        sem_close(&sem);
        endwin();
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
    char z = '0';
    write(t1,&z,1);
    read(t2,&player,sizeof(struct player_t));

    if(player.isActive == 1)
    {
        endwin();
        sem_close(&sem);

        printf("User already connected!\n");
        return 1;
    }
    pthread_create(&playerThread,NULL,&playerInput,&player);
    char* tab[] = {"","Can't connect to this client"};
    while(1)
    {
        player.pid = getpid();
        clear();
        if(end)
        {
        endwin();
	    pthread_cancel(playerThread);
        system("clear");
        printf("%s\n",tab[end-1]);
            exit(0);
        }
        
        read(t2,&player,sizeof(struct player_t));

        if(player.id == 69)
        {
            endwin();
            system("clear");
            printf("Server OFFLINE\n");
            exit(1);
        }
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
        refresh();
        sem_post(&sem);

    }
    endwin();
}
