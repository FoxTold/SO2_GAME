//
// Created by riba on 10.10.22.
//

#ifndef SERVER_BEAST_H
#define SERVER_BEAST_H
#include <semaphore.h>


struct beast_t{
    unsigned int x;
    unsigned int y;

    sem_t semaphore;
    pthread_t thread;
    char isTriggered;
};



#endif //SERVER_BEAST_H
