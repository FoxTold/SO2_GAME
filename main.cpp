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
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include "coin.h"
#include <string>
#include <vector>
#include <signal.h>


void* playerRoutineServer(void* args);
unsigned int roundCounter = 0;

std::string ruchy[] = {"tmp/ruch1\0","tmp/ruch2\0","tmp/ruch3\0","tmp/ruch4\0"};
std::string playersfifo[] = {"tmp/player1\0","tmp/player2\0","tmp/player3\0","tmp/player4\0"};

std::vector <beast_t*> beasts;
std::vector <pthread_t*> threads;
std::vector <player_t*> players;
std::vector <coin_t*> coins;

sem_t serverSem;

void initFifos()
{
    for(int i = 0;i<ruchy->length();i++)
    {
        mkfifo(ruchy[i].data(),0777);
        mkfifo(playersfifo[i].data(),0777);
    }
    mkfifo("tmp/server1",0777);
    mkfifo("tmp/server2",0777);

}
 void initPlayer()
{
    pthread_t* thread = (pthread_t*) calloc(1,sizeof(pthread_t));
    player_t* player = (player_t*)calloc(1,sizeof(player_t));
    sem_init(&player->sem,0,1);
    player->serverPid = getpid();
    player->isActive = 0;

    while(1)
    {
        player->startX = rand()%64;
        player->startY = rand()%32;
        if(mvinch(player->startY,player->startX)== ' ')
        {
            player->id = players.size()+1;
            player->x = player->startX;
            player->y = player->startY;
            sem_init(&player->semaphore,0,1);
            pthread_create(thread,NULL,playerRoutine,player);
            threads.push_back(thread);
            players.push_back(player);
            break;
        } 
    }
    strcpy(player->ruch_fifo,ruchy[player->id-1].data());
    strcpy(player->player_fifo,playersfifo[player->id-1].data());

}
 void initPlayerServer()
{
    pthread_t* thread = (pthread_t*) calloc(1,sizeof(pthread_t));
    player_t* player = (player_t*)calloc(1,sizeof(player_t));
    while(1)
    {
        player->startX = 100;
        player->startY = 100;
        if(mvinch(player->startY,player->startX)== ' ')
        {
            player->id = players.size();
            player->x = player->startX;
            player->y = player->startY;
            sem_init(&player->semaphore,0,1);
            pthread_create(thread,NULL,playerRoutineServer,player);
            threads.push_back(thread);
            players.push_back(player);
            break;
        }
        
    }
}
void  spawnCoin(int count)
{
    struct coin_t* coin = (coin_t*)calloc(1,sizeof(struct coin_t));

    while(1)
    {
        coin->x = rand()%64;
        coin->y = rand()%32;
        if(mvinch(coin->y,coin->x) == ' ')
        break;
    }
    coin->count = count;
    coins.push_back(coin);
}
void  spawnCoinAtLocation(int y,int x,int count)
{
    if(count == 0)return;
    struct coin_t* coin = (coin_t*)calloc(1,sizeof(struct coin_t));

    coin->x = x;
    coin->y = y;
    coin->count = count;
    coins.push_back(coin);
}
void drawCoins()
{
    for(auto& a : coins)
    {
        if(a->count == 10)
        {
            mvaddch(a->y,a->x,'t');
        }
        else if (a->count == 50 )
        {
            mvaddch(a->y,a->x,'T');
        }
        else
        {
            mvaddch(a->y,a->x,'D');

        }
    }
}
void drawBeasts()
{
    for(auto beast : beasts)
    {
        drawBeast(beast);
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
            else if ( *(map + i*MAP_WIDTH + j)== 'A')
            {
                attron(COLOR_PAIR(PANEL));
                mvaddch(i,j,'A');
                attroff(COLOR_PAIR(PANEL));
            }

        }
    }
    attroff(COLOR_PAIR(BACKGROUND));
}
void drawPlayers()
{


    for(auto& a : players)
    {
        if(!a->isActive)continue;
        char l = a->id + '0';
        mvaddch(a->y,a->x,l);
    }

}
void generatePanel()
{
    pid_t pid = getpid();
    attron(COLOR_PAIR(PANEL));
    for(int i = 0;i< PANEL_HEIGHT;i++)
    {
        for(int j = MAP_WIDTH; j < MAP_WIDTH+PANEL_WIDTH;j++)
        {
            mvaddch(i,j,' ');
        }
    }
    
    //printing serer and game info
    mvprintw(0,MAP_WIDTH+1,"Server's PID: %d",pid);
    mvprintw(1,MAP_WIDTH+1,"SCampsite X/Y: %d/%d",1,10);
    mvprintw(2,MAP_WIDTH+1,"Round number: %d",roundCounter);
    mvprintw(4,MAP_WIDTH+1,"Parameter:   Player1  Player2  Player3  Player4 ");
    mvprintw(5,MAP_WIDTH+1,"PID");
    mvprintw(6,MAP_WIDTH+1,"TYPE");
    mvprintw(7,MAP_WIDTH+1,"Curr X/Y");
    mvprintw(8,MAP_WIDTH+1,"Deaths");
    mvprintw(11,MAP_WIDTH+1,"Coins carried");
    mvprintw(12,MAP_WIDTH+1,"Coins brought");

    std::string tab[] = {"HUMAN","CPU"};
    //printing players infos
    for(int i=0;i<4;i++)
    {
        if(players[i]->isActive)
        {
        mvprintw(5,MAP_WIDTH+1 + strlen("PID") + 10 + i*10,"%d",players[i]->pid);
        mvprintw(6,MAP_WIDTH+1 + strlen("TYPE") + 8 + i*10,"%s",tab[players[i]->type].data());
        mvprintw(7,MAP_WIDTH+1 + strlen("Curr X/Y") + 4 + i*10,"%d/%d",(players[i])->x,(players[i])->y);
        mvprintw(8,MAP_WIDTH+1 + strlen("DEATHS") + 8 + i*10,"%d",players[i]->deaths);
                mvprintw(9,MAP_WIDTH+1 + strlen("isActive") + 8 + i*10,"%d",players[i]->isActive);
                mvprintw(10,MAP_WIDTH+1 + strlen("isActive") + 8 + i*10,"%s",players[i]->player_fifo);

        mvprintw(11,MAP_WIDTH+1 + strlen("Coins carried") + 1  + i*10,"%d",players[i]->currentCoins);
        mvprintw(12,MAP_WIDTH+1 + strlen("Coins brought") + 1  + i*10,"%d",players[i]->collectedCoins);
        }
        else
        {
        mvprintw(5,MAP_WIDTH+1 + strlen("PID") + 10 + i*10,"-");
        mvprintw(6,MAP_WIDTH+1 + strlen("TYPE") + 9 + i*10,"-");
        mvprintw(7,MAP_WIDTH+1 + strlen("Curr X/Y") + 4 + i*10,"-/-");
        mvprintw(8,MAP_WIDTH+1 + strlen("DEATHS") + 7 + i*10,"-");
        mvprintw(9,MAP_WIDTH+1 + strlen("isActive") + 8 + i*10,"%d",players[i]->isActive);
                mvprintw(10,MAP_WIDTH+1 + strlen("isActive") + 2 + i*20,"%s",players[i]->player_fifo);

        mvprintw(11,MAP_WIDTH+1 + strlen("Coins carried") + 1  + i*10,"-");
        mvprintw(12,MAP_WIDTH+1 + strlen("Coins brought") + 1  + i*10,"-");
        }

   }

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
        
        endwin();
        printf("ERROR");
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
void initBeast()
{
    beast_t* beast = (beast_t*) calloc(1,sizeof(beast_t));
    pthread_t* thread = (pthread_t*) calloc(1,sizeof(pthread_t));
    sem_init(&beast->semaphore,0,1);
    do
    {
        beast->y = rand() % 24 + 3;
        beast->x = rand() % 40 + 3;
    } while (mvinch(beast->y,beast->x) != ' ');
    mvaddch(beast->y,beast->x,'*');
    beasts.push_back(beast);
    threads.push_back(thread);
    pthread_create(thread,NULL,moveBeast,beast);

}
void* writeData(void* args)
{
    struct player_t* player1 = (struct player_t*)args;

    while(1)
    {
        sem_wait(&player1->sem);
        if(player1->isActive)
            write(player1->fifo_write,player1,sizeof(struct player_t));
    }
}
void* playerRoutine(void* args)
{   
    struct player_t* player1 = (struct player_t* ) args;
    player1->fifo_read = open(player1->ruch_fifo,O_RDONLY);
    read(player1->fifo_read,&player1->pid,sizeof(pid_t));
    player1->fifo_write = open(player1->player_fifo,O_WRONLY);
    write(player1->fifo_write,player1,sizeof(struct player_t));
    char ruch = 'l';
    player1->isActive = 1;
    pthread_t* thread  = (pthread_t*)calloc(1,sizeof(pthread_t));
    pthread_create(thread,NULL,writeData,player1);
    threads.push_back(thread);
    while(1)
    {
        read(player1->fifo_read,&ruch,1);
        player1->isActive = 1;
        if(player1->canMove == 0)
        {
            player1->canMove=1;
            ruch = 'l';
        }

        switch(ruch)
        {
            case 'q':
            {
                player1->isActive = 0;
                player1->fifo_read = open(ruchy[player1->id-1].data(),O_RDONLY);
                player1->fifo_write = open(playersfifo[player1->id-1].data(),O_WRONLY);

                int pidss=0;
                read(player1->fifo_read,&pidss,4);
                write(player1->fifo_write,&player1,sizeof(struct player_t));
                player1->pid = pidss;

                while(1)
                {
                    player1->startX = rand()%64;
                    player1->startY = rand()%32;
                    if(mvinch(player1->startY,player1->startX)== ' ')
                    {
                        player1->x = player1->startX;
                        player1->y = player1->startY;
                        break;
                    }
                    
                }
                break;
            }
            case 'w':
                if(map[(player1->y-1)*MAP_WIDTH + player1->x] == 'A')
                {
                    player1->collectedCoins += player1->currentCoins;
                    player1->currentCoins = 0;
                }
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
                if(map[(player1->y+1)*MAP_WIDTH + player1->x] == 'A')
                {
                    player1->collectedCoins += player1->currentCoins;
                    player1->currentCoins = 0;
                }
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
                if(map[(player1->y)*MAP_WIDTH + player1->x-1] == 'A')
                {
                    player1->collectedCoins += player1->currentCoins;
                    player1->currentCoins = 0;
                }
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
                if(map[(player1->y)*MAP_WIDTH + player1->x+1] == 'A')
                {
                    player1->collectedCoins += player1->currentCoins;
                    player1->currentCoins = 0;
                }
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
            case 'b':
                {
                    initBeast();
                    break;
                }
            case 't':
            {
                spawnCoin(10);
                break;
            }
            case 'T':
            {
                spawnCoin(50);
                break;
            }
                sem_wait(&player1->semaphore);
        }

    }
}
void* playerRoutineServer(void* args)
{
    struct player_t* player1 = (struct player_t* ) args;
    while(1)
    {
        char ruch = getch();
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
            case 'b':
                {
                    initBeast();
                    break;
                }
                case 't':
            {
                spawnCoin(10);
                break;
            }
            case 'T':
            {
                spawnCoin(50);
                break;
            }

        }

        sem_wait(&player1->semaphore);
    }
}
void* moveBeast(void* args)
{
    struct beast_t* beast = (struct beast_t*)args;
    while(1)
    {
        int move = 0;
        int automat = 0;
        for(auto& player : players)
        {
            if((beast->x+1 == player->x ||beast->x+2 == player->x || beast->x+3 == player->x) && beast->y == player->y)
            {
                move =2; 
                automat = 1;
            }
            if((beast->x-1 == player->x ||beast->x-2 == player->x || beast->x-3 == player->x) && beast->y == player->y)
            {
                move =3; 
                automat = 1;
            }
            if((beast->y+1 == player->y ||beast->y+2 == player->y || beast->y+3 == player->y) && beast->x == player->x)
            {
                move =0; 
                automat = 1;
            }
            if((beast->y-1 == player->y ||beast->y-2 == player->y || beast->y-3 == player->y) && beast->x == player->x)
            {
                move =1; 
                automat = 1;
            }
        }
        if(!automat)
        {
            move = rand()%4;
        }
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
void checkIfAnyOfPlayersIsDead()
{
    for(auto&player : players)
    for(auto&beast : beasts)
    {
        if(beast->x == player->x && beast->y == player->y)
        {
            spawnCoinAtLocation(player->y,player->x,player->currentCoins);
            killPlayer(player);
        }
    }
}
void killPlayer(struct player_t* player)
{
    player->x = player->startX;
    player->y = player->startY;
    player->currentCoins = 0;
}
void checkIfPlayersKill()
{
    for(int i=0;i<players.size();i++)
    {
        for(int j=i+1;j<players.size();j++)
        {
            if(players[i]->x == players[j]->x && players[i]->y == players[j]->y)
            {
                if(players[i]->isActive && players[j]->isActive)
                {
                spawnCoinAtLocation(players[i]->y,players[i]->x,players[i]->currentCoins + players[j]->currentCoins);
                killPlayer(players[i]);
                killPlayer(players[j]);
                }
            }
        }
    }
}
void checkIfPlayerCollectedCoin()
{
    for(auto&player : players)
    {
        for(auto&coin : coins)
        {
            if(coin->x == player->x && coin->y == player->y)
            {
                player->currentCoins += coin->count;
                coin->count = 0;
                coin->x = -1;
                coin->y=-1;
            }
        }
    }
}
void sig_handler(int signum){

    struct player_t player_exit;
    player_exit.id = 69;
    for(int i=0;i<4;i++)
    {
        if(players[i]->isActive)
            write(players[i]->fifo_write,&player_exit,sizeof(player_exit));
        close(players[i]->fifo_write);
        close(players[i]->fifo_read);
    }
    for(auto x : threads)
    {
        pthread_cancel(*x);
        free(x);
    }    
        for(auto& x : players)
    {
        sem_close(&x->sem);
        free(x);
    }
    for(auto x : beasts)
    {
        sem_close(&x->semaphore);
        free(x);
    }
    for(auto& x : coins)
    {
        free(x);
    }
    system("clear");
    system("rm tmp/*");
    system("clear");
    printf("Server OFFLINE...\n");
    endwin();
    exit(1);
}
void* serverConsole(void* args)
{
    while(1)
    {
        char command = getch();
        switch(command)
        {
            case 'q':
                for(auto x : threads)
                {
                    pthread_cancel(*x);
                    free(x);
                }
                struct player_t player_exit;
                player_exit.id = 69;
                for(int i=0;i<4;i++)
                {
                    if(players[i]->isActive)
                    write(players[i]->fifo_write,&player_exit,sizeof(player_exit));
                }
                for(auto& x : players)
                {
                    sem_close(&x->sem);
                    free(x);
                }
                for(auto& x : beasts)
                {
                    sem_close(&x->semaphore);
                    free(x);
                }
                for(auto& x : coins)
                {
                    free(x);
                }
                system("clear");
                system("rm tmp/*");
                system("clear");
                printf("Server OFFLINE...\n");
                endwin();
                exit(1);
                break;
            case 't':
                spawnCoin(10);
                break;
            case 'T':
                spawnCoin(50);
                break;
            case 'b':
                initBeast();
                break;
            case 'B':
                initBeast();
                break;
            default:
                break;
        }
        sem_wait(&serverSem);
    }
}
void* connectPlayers(void*)
{

    while(1)
    {
        int serverRead = open("tmp/server1",O_RDONLY);
        int serverWrite = open("tmp/server2",O_WRONLY);
        int pid = 0;
        read(serverRead,&pid,sizeof(int));
        mvprintw(0,0,"DOSTALEM PID");
        int found = 0;
        for(int i=0;i<4;i++)
        {
                    mvprintw(0,0,"%d",players[i]->isActive);

            if(players[i]->isActive == 0)
            {
                players[i]->pid = pid;
                players[i]->isActive = 1;

                write(serverWrite,players[i],sizeof(struct player_t));
                close(serverRead);
                close(serverWrite);
                found = 1;
                break;
            }
        }
        if(!found)
        {
        struct player_t errorPlayer;
        errorPlayer.pid = -1;
        write(serverWrite,&errorPlayer,sizeof(struct player_t));
        }
        close(serverRead);
        close(serverWrite);
    }
}
int main()
{
    sem_init(&serverSem,0,1);
    pthread_t* serverThread = (pthread_t*)calloc(1,sizeof(pthread_t));
    pthread_create(serverThread,NULL,serverConsole,NULL);
    signal(SIGINT,sig_handler);
    signal(SIGQUIT,sig_handler);
    signal(SIGTSTP,sig_handler);
    signal(0,sig_handler);

    srand(time(NULL));
    initFifos();

    loadMap();
    initscr();
    noecho();
    start_color();

    
    init_pair(BACKGROUND,COLOR_RED,COLOR_RED);
    init_pair(PANEL,COLOR_BLACK,COLOR_YELLOW);
    generateBorders();
    generateFooter();
    initPlayer();
    initPlayer();
    initPlayer();
    initPlayer();
    pthread_t* connectThread = (pthread_t*)calloc(1,sizeof(pthread_t));
    pthread_create(connectThread,NULL,connectPlayers,NULL);
    generatePanel();




    while(1)
    {

        clear();
        generateBorders();
        generateFooter();
        generatePanel();
        drawBeasts();
        drawCoins();
        checkIfAnyOfPlayersIsDead();
        checkIfPlayerCollectedCoin();
        checkIfPlayersKill();
        for(auto beast : beasts)
        {
            sem_post(&beast->semaphore);
        }
        drawPlayers();

        for(auto& a : players)
        {
            int z = 0;
            for(int i =a->y-2;i<=a->y+2;i++)
            {
                for(int j = a->x-2;j<=a->x+2;j++)
                {
                    a->map_fragment[z] = (char)mvinch(i,j);
                    z++;
                } 
            }  
            a->round = roundCounter;
            sem_post(&a->semaphore);  
            sem_post(&a->sem);

        }
        sem_post(&serverSem);
        usleep(275000);
        
        refresh();
        roundCounter++;
    }

    endwin();
    return EXIT_SUCCESS;
}