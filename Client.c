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
		printf("rock\n");
	}
	if(move == 1){
		printf("paper\n");
	}
	if(move == 2){
		printf("scissors\n");
	}
}

void printResult(int result){
	if(result == -1){
		printf("There's a tie!\n");
	}
	if(result == 1){
		printf("You win! \n");
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
		server_stat = ping(server_socket);
		printf("Server status: %d\n Games played: %d\n", server_stat, gamesPlayed);

		//PING CODE - CURRENTLY NOT IMPLEMENTED
		// if (server_stat <= 0) {
		//     printf("Server disconnected\n");
		// 		exit(0);
		// }
		// else {
		//struct timeval tv = {5, 0}; // 5 second timeout
		//setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

*/
	void client_logic(int server_socket){
		
		printf("Match made! Your opponent is... (this is for later))\n");
		int gamesPlayed = 0;
		int myScore = 0;
		int opponentScore = 0;
		
		while (gamesPlayed < 2){ //i.e., repeat until 3 rounds have been played
			int bytes_received = recv(server_socket, &gamesPlayed, sizeof(int), 0);
			if (bytes_received <= 0){
				perror("Client exiting due to empty message...\n");
				exit(0);
			}
			printf("You have played %d games with this opponent\n", gamesPlayed);
			printf("The score is %d to %d\n", myScore, opponentScore);
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

				if (FD_ISSET(STDIN_FILENO, &read_fds)) {
					if (!moveAllowed) {
						printf("Too early! Wait for your turn.\n");
						flush_stdin();
						continue;
					}

					char buf[32];
					fgets(buf, sizeof(buf), stdin);

					if (sscanf(buf, "%d", &moveint) == 1 &&
						moveint >= 0 && moveint <= 2) {

						printf("Your move: ");
						printMove(moveint);

						moveAllowed = 0;
						send(server_socket, &moveint, sizeof(int), 0);
					}
					else{
						printf("Invalid input! Please enter again.\n");
					}
				}

				if (FD_ISSET(server_socket, &read_fds)) {
					break;
				}
			}
			
			//Receive opponent move
			int opponentMove; 
			recv(server_socket, &opponentMove, sizeof(int), 0);
			printf("Opponent move: "); printMove(opponentMove);
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
