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

#include "Networking.h"
#include "GameLogic.h"
// The purpose of this .c is to send stuff (like user input) to server
// or to get stuff (like results) from server.


// Asks for user move and sends to server. Then waits for server response for the game.
// Displays the result to user.
// Later: can have the option to ask for username, send to server, and get opponent's username and display etc.
// Later 2: after each match ask if the users want another game. Redo if both replied yes.
// Later 3: if no response after a certain time duration (e.g. 10s) quit automatically.

void client_logic(int server_socket){
	
	printf("Match made! Your opponent is... (this is for later))\n");
	int gamesPlayed = 0;
	printf("%d\n", server_socket);
	while (gamesPlayed < 3){ //i.e., repeat until 3 rounds have been played
		int bytes_received = recv(server_socket, &gamesPlayed, sizeof(int), 0);
		if (bytes_received <= 0){
			perror("Client exiting due to empty message...\n");
			exit(0);
		}
		printf("You have played %d games with this opponent\n", gamesPlayed);
		
		
		int moveint = -1;
		
		while (moveint < 0 || moveint > 2){ //while loop to ensure the sent value is greater than -1
			printf ("Please enter the number of one of the options:\n\t0. Rock\n\t1. Paper\n\t2. Scissors\n");
			char move[16];
			moveint = -1;
			if(fgets (move, 16, stdin) == NULL){
				perror("Input closed. ");
			}
			if(sscanf (move, "%d", &moveint) != 1){
				moveint = -1;
			}
			if(moveint >= 0 && <= 2){
				break;
			}
			else{
				printf("Invalid input! Please enter again.\n");
			}
		}
		printf("moveint %d\n", moveint);
		send(server_socket, &moveint, sizeof(int), 0);
	}
		printf("Game ended.\n");
		exit(0);
}
	// a wrapper function that calls client_connect(char* IP, char* port)
	// which returns connected socket descriptor.
	int connect_to_server(char* IP){
		int sockfd = client_connect(IP, PORT);
		if (sockfd < 0){
			perror("client_connect failed");
		}
		return sockfd;
	}

	// optionally takes IP address from user input
	// connects to the server (call connect_to_server)
	// runs the client loop (call client_logic())
	// close up socket at the end
	int main(int argc, char* argv[]){
		char* IP = "127.0.0.1";
		if (argc > 1){
			strcpy(IP, argv[1]);
		}
		printf("This is a Rock Paper Scissors Game Server. Welcome!\n");
		printf("Please enter the name you wish to be called(for later! Ignore for now)\n");

		int server_socket = connect_to_server(IP);
		client_logic(server_socket);

		//before exiting, send a 'down' to server to reduce # of connections by 1
		return (0);
	}
