#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#define HEIGHT 25
#define WIDTH 51
char* loadMap()
{
    char* map = calloc(HEIGHT*WIDTH+1,1);
    FILE* fp = fopen("./assets/map.txt","r");
    if(!fp)
    {
        printf("Coudlnt open file!");
        free(map);
        return NULL;
    }

    for(int i=0;i<HEIGHT;++i)
    {
        for(int j=0;j<WIDTH;j++)
        {
            *(map+i*WIDTH+j)=fgetc(fp);

            move(j,i);
            printf("%c",*(map+i*WIDTH+j));
        }
    }
        free(map);

    fclose(fp);
    return map;
}


int main()
{
    initscr();
    getch();  
    endwin();
    return EXIT_SUCCESS;
}