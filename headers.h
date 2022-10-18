void generateBorders();
void drawPlayer(struct player_t* player);
void generatePanel();
void generateFooter();
void loadMap();
void initBeast(struct beast_t* beast);
void* playerRoutine(void* args);
void* moveBeast(void* args);
void drawBeast(struct beast_t* beast);
void drawBeasts(struct beast_t* beast);
void checkIfAnyOfPlayersIsDead(struct player_t* players, struct beast_t* beasts);
void killPlayer(struct player_t* player);
char map[MAP_WIDTH*MAP_HEIGHT+1] = {0};

