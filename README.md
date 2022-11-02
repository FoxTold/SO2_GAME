# Overview
Game created by: Tomasz Rybiński 235960.
Game can be built and launched on **LINUX** only.
Simple 2D client server game based on IPC mechanisms. 
Inter-process communication is handled by named pipelines (FIFO).

# Rules
- Players have to collect coins (t, T).
- Players have to avoid beasts (*).
- On contact with beast or other player, players loses all gold they currently possess.
- To turn in gold players have to go to campsite (A).
- Bushes slow you! It takes 1 extra round to leave the bush.

# Technology used
- named pipelines
- pthread_t
- sem_t
- mutex_t


# Prerequisites
- Linux operating system
- Installed ncurses
- Installed CMake ver. 3.13 or higher


# How to build
1. Extract .zip 
2. Open terminal in extracted directory
3. Run ./configure.sh script. It will generate cmake buildsystem and then build both server and client.
4. To run server, open new terminal window and type: ./start_server.sh
5. To run 1st client, open new terminal window and type: ./start_player1.sh
5. To run 2st client, open new terminal window and type: ./start_player2.sh
5. To run 3st client, open new terminal window and type: ./start_player3.sh
5. To run 4st client, open new terminal window and type: ./start_player4.sh
