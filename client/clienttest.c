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


int main(void)
{
    int writeP = open("tmp/server1",O_WRONLY);
    if(writeP == -1)
    {
        printf("Error with write fifo\n");
        return 1;
    }
    int readP = open("tmp/server2",O_RDONLY);
    if(readP == -1)
    {
        printf("Error with read fifo\n");
        return 1;
    }
    printf("Stworzono\n");

    int pid = getpid();
    int writeF = write(writeP,&pid,sizeof(int));

    printf("Wyuslano\n");

    struct player_t xd;
    int readF = read(readP, &xd, sizeof(struct player_t)); 
    printf("%d",xd.pid);
    close(readP);
    close(writeP);

}