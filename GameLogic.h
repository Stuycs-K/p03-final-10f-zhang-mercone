#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>

#ifndef GAMELOGIC_H
#define GAMELOGIC_H

void scorehandler (int games_played, int a_score, int b_score, int a_move, int b_move);
int compare_move(int a_move, int b_move);

#endif
