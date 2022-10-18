//
// Created by riba on 10.10.22.
//

#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H
#include <semaphore.h>
#include <ncurses.h>
#include <stdlib.h>
enum player_type_t
{
    HUMAN = 0,
    CPU
};


struct player_t{
    int id;
    pid_t  pid; 

    enum player_type_t type;
    char map_fragment[26];
    int is_init;
    unsigned int x;
    unsigned int y;

    unsigned int currentCoins;
    unsigned int collectedCoins;

    unsigned int startX;
    unsigned int startY;

    unsigned int deaths;
    unsigned int canMove;

    sem_t semaphore;
};

#endif //SERVER_PLAYER_H
