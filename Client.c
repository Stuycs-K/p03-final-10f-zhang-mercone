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

void printMove(int move){
	if(move == 0){
		printf("ROCK\n");
	}
	if(move == 1){
		printf("PAPER\n");
	}
	if(move == 2){
		printf("SCISSORS\n");
	}
}

void printResult(int result){
	if(result == -1){
		printf("There's a tie!\n");
	}
	if(result == 1){
		printf("You won! \n");
	}
	if(result == 0){
		printf("You lost. \n");
	}
}

// Asks for user move and sends to server. Then waits for server response for the game.
// Displays the result to user.
// Later: can have the option to ask for username, send to server, and get opponent's username and display etc.
// Later 2: after each match ask if the users want another game. Redo if both replied yes.
// Later 3: if no response after a certain time duration (e.g. 10s) quit automatically.

/*
void client_logic(int server_socket){
int gamesPlayed = 0;
int server_stat;
printf("Server socket: %d\n", server_socket);

while (gamesPlayed < 3){ //i.e., repeat until 3 rounds have been played
//server_stat = ping(server_socket);
printf("Server status: DUMMIED\n Games played (pre recv): %d\n", gamesPlayed);

//PING CODE - CURRENTLY NOT IMPLEMENTED
// if (server_stat <= 0) {
//     printf("Server disconnected\n");
// 		exit(0);
// }
// else {
//struct timeval tv = {5, 0}; // 5 second timeout
//setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

<<<<<<< HEAD
int bytes_received = recv(server_socket, &gamesPlayed, sizeof(int), 0);
if (bytes_received <= 0){
perror("Client exiting due to empty message...");
exit(0);
}
//set move tracker
printf("Client's games played recieved: %d\n", gamesPlayed);
int moveint = -1;

while (moveint < 0 || moveint > 2){ //while loop to ensure the sent value is greater than -1
printf ("Enter the number of one of the options:\n\t0. Rock\n\t1. Paper\n\t2. Scissors\n");
char move[16];
moveint = -1;
if(fgets (move, 16, stdin) == NULL){
perror("Input closed. ");
=======
*/
void client_logic(int server_socket){
	int gamesPlayed = 0;
	char opponentName[64]; //grab opponent username
	char username[64];
	char userbool[8];
	printf("DEBUG: Server Socket %d\n", server_socket);

	int usernameBool = -1;
	while (usernameBool == -1){
		printf("Would you like to enter a username?\n 0. Yes\n1. No\n");
		if(fgets (userbool, 8, stdin) == NULL){
			perror("Pain.");
			exit(0);
		}

		if(sscanf(userbool, "%d", &usernameBool) != 1){
			usernameBool = -1;
		}

		if (usernameBool == 0 || usernameBool == 1){
			if (usernameBool == 0){
				printf("Enter your username below:\n");
				if(fgets (username, 64, stdin) == NULL){
					perror("Your username was too powerful. Bye.");
					exit(0);
				}
			}
			if (usernameBool == 1){
				strncpy(username, "Anonymous", 64);
			}
				int id;
				//recv(server_socket, &id, sizeof(int), 0);
				//printf("id %d", id);
				send(server_socket, &username, 64, 0);

		}
		else {
			usernameBool = -1;
		}
	}


	printf("Please wait while we match you to an opponent.\n");


	if (recv(server_socket, &opponentName, 64, 0) <= 0){
		perror("Error. Client not connected to server.\n");
		exit(0);
	}

	printf("Match made! Your opponent is...%s!\n", opponentName);

	while (gamesPlayed < 2){ //i.e., repeat until 3 rounds have been played
		int bytes_received = recv(server_socket, &gamesPlayed, sizeof(int), 0);
		if (bytes_received <= 0){
			perror("Client exiting due to empty message...\n");
			exit(0);
		}

		int moveint = -1;

		while (1){ //while loop to ensure the sent value is greater than -1
			printf ("\n~~~~~~~~~~~~~\n");
			printf ("    ROUND %d\n~~~~~~~~~~~~~\nPlease enter the number of one of the options:\n\t0. Rock\n\t1. Paper\n\t2. Scissors\n", gamesPlayed);
			char move[16];
			moveint = -1;
			if(fgets (move, 16, stdin) == NULL){
				perror("Input closed. ");
				exit(0);
			}
			// if(strcasecmp(move, "Info\n") == 0){
			// 	printf("SCOREBOARD");
			// }
			printf("Move: %s", move);
			if(sscanf(move, "%d", &moveint) != 1){
				moveint = -1;
			}
			if(moveint >= 0 && moveint<= 2){
				break;
			}
			else {
				printf("Invalid input! Please try again.\n");
			}
		}

		printf("You played: ");
		printMove(moveint);

		//Send move to server
		send(server_socket, &moveint, sizeof(int), 0);

		//Receive opponent move
		int opponentMove;
		printf("Waiting for opponent...\n");
		printMove(opponentMove);
		recv(server_socket, &opponentMove, sizeof(int), 0);
		printf("Opponent played "); printMove(opponentMove);

		int result;
		recv(server_socket, &result, sizeof(int), 0); printResult(result);
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
	int server_socket = connect_to_server(IP);
	client_logic(server_socket);

	//before exiting, send a 'down' to server to reduce # of connections by 1
	return (0);
}
