#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include  <time.h>
#include <unistd.h>
#include "signature.h"
#include "common.h"
#include "player.h"
#include "beast.h"
#include <semaphore.h>
#include <pthread.h>
#include "headers.h"
#define numberOfBeasts 5


unsigned int playersCount = 0;
unsigned int roundCounter = 0;
unsigned int amoutOfCoins = 0;
pthread_t beastThreads[numberOfBeasts];

char map[MAP_WIDTH*MAP_HEIGHT+1] = {0};
void initBeasts(struct beast_t* beasts) 
{
    for(int i=0;i<numberOfBeasts;i++)
    {
        initBeast(&beasts[i]);
        drawBeast(&beasts[i]);
        pthread_create(&beastThreads[i],NULL,&moveBeast,&beasts[i]);
    }
}
void drawBeasts(struct beast_t* beasts)
{
    for(int i=0;i<numberOfBeasts;i++)
    {
        drawBeast(&beasts[i]);
    }
}
void generateBorders()
{
    for(int i=0;i<MAP_HEIGHT;i++)
    {
        for(int j=0;j<MAP_WIDTH;j++)
        {
            if(*(map + i*MAP_WIDTH + j)== '^')
            {
                attron(COLOR_PAIR(BACKGROUND));
                mvaddch(i,j,'^');
                attroff(COLOR_PAIR(BACKGROUND));
            }
            else if ( *(map + i*MAP_WIDTH + j)== '#')
            {
                attron(COLOR_PAIR(PANEL));
                mvaddch(i,j,'#');
                attroff(COLOR_PAIR(PANEL));
            }
        }
    }
    attroff(COLOR_PAIR(BACKGROUND));
}
void drawPlayer(struct player_t* player)
{
    mvaddch(player->y,player->x,'&');
}
void generatePanel()
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
    mvprintw(0,MAP_WIDTH+1,"Server's PID: %d",1337);
    mvprintw(1,MAP_WIDTH+1,"SCampsite X/Y: %d/%d",1,10);
    mvprintw(2,MAP_WIDTH+1,"Round number: %d",roundCounter);

    //printing players infos
    mvprintw(4,MAP_WIDTH+1,"Parameter:   Player1  Player2  Player3  Player4 ");
    mvprintw(5,MAP_WIDTH+1,"PID         1390     1420     1900     -");
    mvprintw(6,MAP_WIDTH+1,"Type        CPU      CPU      HUMAN    -");
    mvprintw(7,MAP_WIDTH+1,"Curr X/Y    37/15    16/21    04/03    --/--");
    mvprintw(8,MAP_WIDTH+1,"Deaths      3        9        0        -");

    //printing coins info
    mvprintw(10,MAP_WIDTH+1,"Coins");
    mvprintw(11,MAP_WIDTH+1 + strlen("Coins"),"carried 30       213      0");
    mvprintw(12,MAP_WIDTH+1 + strlen("Coins"),"brought 1300     33       0");

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
void generateFooter()
{
    mvprintw(50,10,tab1);
    mvprintw(51,10,tab2);
    mvprintw(52,10,tab3);
    mvprintw(53,10,tab4);

    mvprintw(55,35,tab5);
    mvprintw(56,35,tab6);
    mvprintw(57,35,tab7);
    mvprintw(58,35,tab8);
}
void loadMap()
{
    FILE* fp = fopen("assets/map1.txt","r");
    if(!fp)
    {
        mvprintw(0,0,"ERRROR");
        endwin();
        exit(1);
    }
    for(int i=0;i<MAP_HEIGHT;i++)
    {
        for(int j=0;j<MAP_WIDTH;j++)
        {
            char temp = fgetc(fp);
            if(temp == '\n')
            {
                temp = fgetc(fp);
            }
            *(map + i*MAP_WIDTH + j ) = temp;
        }
    }
    fclose(fp);
}
void initBeast(struct beast_t* beast)
{
    sem_init(&beast->semaphore,0,1);
    
    do
    {
        beast->y = rand() % 24 + 3;
        beast->x = rand() % 40 + 3;
    } while (mvinch(beast->y,beast->x) != ' ');
    mvaddch(beast->y,beast->x,'*');
}
void* playerRoutine(void* args)
{
    struct player_t* player1 = (struct player_t* ) args;
    while(1)
    {
        char ruch = getch();
        fflush(stdin);

        if(player1->canMove == 0)
        {
            player1->canMove=1;
            ruch = 'l';
        }
        switch(ruch)
        {
            case 'w':
                if(map[(player1->y-1)*MAP_WIDTH + player1->x] != '^')
                {
                if(map[(player1->y-1)*MAP_WIDTH + player1->x] == '#')
                    {
                        player1->canMove = 0;
                    }
                    player1->y -= 1;
                    break;
                }
                break;

                
            case 's':
                if(map[(player1->y+1)*MAP_WIDTH + player1->x] != '^')
                {
                if(map[(player1->y+1)*MAP_WIDTH + player1->x] == '#')
                    {
                        player1->canMove = 0;
                    }
                
                    player1->y += 1;
                    break;
                }
                break;

            case 'a':
                if(map[(player1->y)*MAP_WIDTH + player1->x-1] != '^')
                {
                if(map[(player1->y)*MAP_WIDTH + player1->x-1] == '#')
                    {
                        player1->canMove = 0;
                    }
                    player1->x -= 1;
                    break;
                }
                break;
            case 'd':
                if(map[(player1->y)*MAP_WIDTH + player1->x+1] != '^')
                {
                if(map[(player1->y)*MAP_WIDTH + player1->x+1] == '#')
                    {
                        player1->canMove = 0;
                    }
                    player1->x += 1;
                    break;
                }
                break;

        }
        sem_wait(&player1->semaphore);
    }
}
void* moveBeast(void* args)
{
    struct beast_t* beast = (struct beast_t*)args;
    while(1)
    {
        int move = rand() % 4;
        if(move == 0)
        {
            if(map[(beast->y+1)*MAP_WIDTH + beast->x] != '^')
            {
                beast->y++;
            }
            else continue;

        }
        else if(move == 1)
        {
            if(map[(beast->y-1)*MAP_WIDTH + beast->x] != '^')
            {
                beast->y--;
            }
            else continue;

        }
        else if(move == 2)
        {
            if(map[(beast->y)*MAP_WIDTH + beast->x+1] != '^')
            {
                beast->x++;
            }
            else continue;

        }
        else if(move == 3)
        {
            if(map[(beast->y)*MAP_WIDTH + beast->x-1] != '^')
            {
                beast->x--;
            }
            else continue;
        }
        
        sem_wait(&beast->semaphore);
    }
}
void drawBeast(struct beast_t* beast)
{
    mvaddch(beast->y,beast->x,'*');
}
void checkIfAnyOfPlayersIsDead(struct player_t* players, struct beast_t* beasts)
{
    for(int playerIterator = 0;playerIterator<playersCount;playerIterator++)
    {
        for(int beastsIterator = 0;beastsIterator<numberOfBeasts;beastsIterator++)
        {
            if(players[playerIterator].x == beasts[beastsIterator].x && players[playerIterator].y == beasts[beastsIterator].y)
            {
                killPlayer(&players[playerIterator]);
            }
        }
    }
}
void killPlayer(struct player_t* player)
{
    mvprintw(0,0,"KILLED");
    player->x = player->startX;
    player->y = player->startY;
}

int main()
{
    struct beast_t beasts[numberOfBeasts];
    

    pthread_t playerThread;
    srand(time(NULL));
    loadMap();
    initscr();
    noecho();
    start_color();
    init_pair(BACKGROUND,COLOR_RED,COLOR_RED);
    init_pair(PANEL,COLOR_BLACK,COLOR_YELLOW);
    struct player_t* player1 = calloc(1,sizeof(struct player_t));
    player1->x = 0 + BORDERS_THICK;
    player1->y = 0 + BORDERS_THICK;
    player1->canMove = 1;
    player1->startX = player1->x;
    player1->startY = player1->y;
    sem_init(&player1->semaphore,0,1);
    initBeasts(beasts);
    drawPlayer(player1);
    generateBorders();
    generatePanel();
    generateFooter();
    playersCount = 1;
    pthread_create(&playerThread,NULL,&playerRoutine,player1);
    while(1)
    {
        clear();
        generateBorders(map);
        generateFooter();
        generatePanel();
        drawPlayer(player1);
        drawBeasts(beasts);
        checkIfAnyOfPlayersIsDead(player1,beasts);
        for(int i=0;i<numberOfBeasts;i++)
        {
            sem_post(&beasts[i].semaphore);
        } 

        sem_post(&player1->semaphore);
        refresh();
        usleep(275000);
        roundCounter++;


    }
    endwin();
    return EXIT_SUCCESS;
}