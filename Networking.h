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
#include <sys/wait.h>


#ifndef NETWORKING_H
#define NETWORKING_H
#define MAX_CONNECTIONS 10
#define PORT "12639"
//function headers go here
void err(int i, char*message);
int server_setup();
int client_connect(char* IP, char* port) ;
int server_tcp_handshake(int listen_socket);
int run_match(int fdA, int fdB);
void sendResult(int fdA, int fdB, int Amove, int Bmove);
void sendScore(int fdA, int fdB, int Ascore, int Bscore);
int getUsername(int fd, char name[]);
void sendUsernames(int fdA, int fdB);

int ping (int server_socket);
#endif
