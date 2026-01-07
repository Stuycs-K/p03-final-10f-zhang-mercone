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


#ifndef NETWORKING_H
#define NETWORKING_H
#define MAX_CONNECTIONS 10
#define PORT "12639"
//function headers go here
void err(int i, char*message);
int server_setup();
int client_connect(char* IP, char* port) ;
int server_tcp_handshake(int listen_socket);
#endif
