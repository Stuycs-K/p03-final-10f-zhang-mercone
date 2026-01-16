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

int moveAllowed = 0;
// The purpose of this .c is to send stuff (like user input) to server
// or to get stuff (like results) from server.
void flush_stdin(){
	int ch = getchar();
	while(ch != '\n' && ch != EOF){
		ch = getchar();
	}
}

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

void client_logic(int server_socket){
	char opponentName[64]; //grab opponent username
	char username[64];
	char userbool[8];
	// debug printf(": Server Socket %d\n", server_socket);

	//some fun stuffs
	printf("Welcome to Rock! Paper! Scissors!, the newest sensation rocking the nation!\n");
	printf("The rules are simple: Three rounds of Rock Paper Scissors, and may the best player win. Good luck!\n");

	//username entering thing
	int usernameBool = -1;
	while (usernameBool == -1){
		printf("Would you like to enter a username?\n 0. Yes\n 1. No\n");
		if(fgets (userbool, 8, stdin) == NULL){
			perror("Error in username boolean.");
			exit(0);
		}

		if(sscanf(userbool, "%d", &usernameBool) != 1){
			usernameBool = -1;
		}

		if (usernameBool == 0 || usernameBool == 1){
			if (usernameBool == 0){
				printf("Enter your username below:\n");
				if(fgets (username, 63, stdin) == NULL){
					perror("Your username was too powerful. Bye.");
					exit(0);
				}
			}
			if (usernameBool == 1){
				strncpy(username, "opponent", 64);
			}
			username[63] = '\0';
			send(server_socket, &username, sizeof(username), 0);
		}
		else {
			usernameBool = -1;
		}
	}
	//placate customer while waiting
	//maybe 'welcome x'!
	printf("Please wait while we connect you to a fellow player...\n");

	//recieve other client's username
	if (recv(server_socket, &opponentName, 64, 0) <= 0){
		perror("Client exiting due to empty message...\n");
		exit(0);
	}
	//match is successful, display this once
	if (strcmp(opponentName, "opponent") == 0){
		//if no username was entered
		printf("Match made! Your opponent is a mysterious person who shall remain unnamed.\n");
	}
	else {
		printf("Match made! Your opponent is...%s\n", opponentName);
	}

	int gamesPlayed = 0;
	int myScore = 0;
	int opponentScore = 0;

	while (gamesPlayed < 2){ //i.e., repeat until 3 rounds have been played
		int bytes_received = recv(server_socket, &gamesPlayed, sizeof(int), 0);
		if (bytes_received <= 0){
			perror("Client exiting due to empty message...\n");
			exit(0);
		}
		printf ("\n~~~~~~~~~~~~~\n   ROUND %d\n~~~~~~~~~~~~~\n", gamesPlayed);
		printf("Current score:\n    You (%d)\n    %s (%d)\n", myScore, opponentName, opponentScore);
		moveAllowed = 1;
		int moveint = -1;
		printf ("Please enter the number of one of the options:\n\t0. Rock\n\t1. Paper\n\t2. Scissors\n");
		while (1) {
			fd_set read_fds;
			FD_ZERO(&read_fds);

			FD_SET(STDIN_FILENO, &read_fds);
			FD_SET(server_socket, &read_fds);

			int maxfd = server_socket + 1;
			select(maxfd, &read_fds, NULL, NULL, NULL);

			char buf[32];
			fgets(buf, sizeof(buf), stdin);
			buf[strcspn(buf, "\n")] = 0;

			if (FD_ISSET(STDIN_FILENO, &read_fds)) {
				if (!moveAllowed) {
					printf("Too early! Wait for your turn.\n");
					flush_stdin();
					continue;
				}


				if (sscanf(buf, "%d", &moveint) == 1 &&
					moveint >= 0 && moveint <= 2) {

						printf("Your move: ");
						printMove(moveint);

						moveAllowed = 0;
						send(server_socket, &moveint, sizeof(int), 0);
					}

					else {
						printf("Invalid input! Please enter again.\n");
					}

				}

				if (FD_ISSET(server_socket, &read_fds)) {
					break;
				}
			}
			printf("Waiting for %s's move...", opponentName);

			//Receive opponent move
			int opponentMove;
			recv(server_socket, &opponentMove, sizeof(int), 0);
			printf("%s played: ", opponentName); printMove(opponentMove);
			int result;
			sleep(1);
			recv(server_socket, &result, sizeof(int), 0);
			printResult(result);
			recv(server_socket, &myScore, sizeof(int), 0);
			recv(server_socket, &opponentScore, sizeof(int), 0);
			sleep(1);

		}
		printf("The score is %d to %d\n", myScore, opponentScore);
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
