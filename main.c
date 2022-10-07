#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "signature.h"

#define MAP_HEIGHT 32
#define MAP_WIDTH 64
#define BORDERS_THICK 3

#define PANEL_HEIGHT 32
#define PANEL_WIDTH 64

#define BACKGROUND 1
#define PANEL 2

struct player_t{
    int x;
    int y;
};

void generateBorders(char* map)
{
    attron(COLOR_PAIR(BACKGROUND));

    for(int i=0;i<MAP_HEIGHT;i++)
    {
        for(int j=0;j<MAP_WIDTH;j++)
        {
            if(*(map + i*MAP_WIDTH + j)== '^')
            {
                mvaddch(i,j,'^');
            }
        }
    }
    attroff(COLOR_PAIR(BACKGROUND));
}
void updatePlayer(struct player_t* player)
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
    mvprintw(2,MAP_WIDTH+1,"Round number: %d",1337);

    //printing players info
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
void loadMap(char* map)
{
    FILE* fp = fopen("assets/map3.txt","r");
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

int main()
{
    char map[MAP_WIDTH*MAP_HEIGHT+1] = {0};

    loadMap(map);
    initscr();
    noecho();
    start_color();
    init_pair(BACKGROUND,COLOR_RED,COLOR_RED);
    init_pair(PANEL,COLOR_BLACK,COLOR_YELLOW);
    struct player_t player1;
    player1.x = 0 + BORDERS_THICK;
    player1.y = 0 + BORDERS_THICK;
    updatePlayer(&player1);
    generateBorders(map);
    generatePanel();
    generateFooter();


    while(1)
    {
        char ruch = getch();
        if(ruch == 'z') break;
        switch(ruch)
        {
            case 'w':
                if((char)mvinch(player1.y-1,player1.x) != '^')
                    player1.y -= 1;
                break;
            case 's':
                if((char)mvinch(player1.y+1,player1.x) != '^')
                    player1.y += 1;
                break;
            case 'a':
                if((char)mvinch(player1.y,player1.x - 1) != '^')
                    player1.x -= 1;
                break;
            case 'd':
                if((char)mvinch(player1.y,player1.x + 1) != '^')
                player1.x += 1;
                break;
        }
        clear();
        generateBorders(map);
        generateFooter();
        generatePanel();
        updatePlayer(&player1);
        refresh();
    }
    endwin();
    return EXIT_SUCCESS;
}