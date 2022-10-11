//
// Created by riba on 10.10.22.
//

#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H
enum player_type_t
{
    HUMAN = 0,
    CPU
};


struct player_t{
    const int id;

    unsigned int x;
    unsigned int y;

    unsigned int currentCoins;
    unsigned int collectedCoins;

    unsigned int startX;
    unsigned int startY;

    unsigned int deaths;
    char canMove;
};
#endif //SERVER_PLAYER_H
