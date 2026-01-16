#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "Networking.h"
#include "GameLogic.h"

int hasUsername = 0;
int moveAllowed = 0;
char userName[64] = {0};
// The purpose of this .c is to send stuff (like user input) to server
// or to get stuff (like results) from server.

static void sighandler(int signo){
		if(signo == SIGINT){
			if(hasUsername){
				char lockpath[128] = {0};
				strcpy(lockpath, "userStats/");
				strcat(lockpath, userName);
				strcat(lockpath, ".lock");
				// Remove lock file
				unlink(lockpath);
				printf("lock file %s removed.\n", lockpath);
			}
			else{
				printf("\n you have exited the game\n");
			}
			exit(0);
		}
	}


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

void trimNewLine(char input[], int leng){
	while (leng > 0 && (input[leng-1] == '\n' || input[leng-1] == '\r')) {
		input[leng - 1] = '\0';
		leng --;
	}
}

int askReplay(int server_socket){
	printf("Would you like to replay? (yes/no)\n");
	char response[16] = {0};
	for(int i = 0; i < 16; i ++){
		response[i] = '\0';
	}
	int opinion;
	fgets(response, 16, stdin);
	int responseLen = strlen(response);
	trimNewLine(response, responseLen);
	while(strcmp(response, "yes")!= 0 && strcmp(response, "no")!= 0){
		printf("please enter yes/no!\n");
		fgets(response, 16, stdin);
		responseLen = strlen(response);
		trimNewLine(response, responseLen);
	}
	if(strcmp(response, "yes")== 0){
		opinion = 1;
		
	}
	else{
		opinion = 0;
	}
	send(server_socket, &opinion, sizeof(int), 0);
	return opinion;
}

int username_exist(char *username) {
    char path[128] = {0};
    // Build: "userStats/" + username + ".txt"
    strcpy(path, "userStats/");
    strcat(path, username);
    strcat(path, ".lock");
    int fd = open(path, O_CREAT | O_EXCL | O_WRONLY, 0666);
    if (fd == -1) {
        if (errno == EEXIST) {
            return 1; // username already exists
        }
        perror("open");
        return 1; // treat other errors as "taken"
    }
    close(fd);
    return 0; // username accepted
}

void release_username(char *username) {
    char lockpath[128] = {0};
    strcpy(lockpath, "userStats/");
    strcat(lockpath, username);
    strcat(lockpath, ".lock");
    unlink(lockpath);  // remove lock file
}


void appendScore(char *username, char *opponent, int myScore, int opponentScore) {
    char path[128] = {0};
    char line[256] = {0};
    char timestamp[64] = {0};
    char numbuf[16] = {0};
	
    // Build path: userStats/<username>.txt
    strcpy(path, "userStats/");
    strcat(path, username);
    strcat(path, ".txt");
	
    // Build timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);
	
    // Start building the one-line entry
    strcpy(line, "[");
    strcat(line, timestamp);
    strcat(line, "] ");
	
    strcat(line, "vs ");
    strcat(line, opponent);
    strcat(line, " | Result: ");
	
    if (myScore > opponentScore){
		strcat(line, "Win");
	}
    else if (myScore < opponentScore){ 
		strcat(line, "Loss");
	}
    else{
		strcat(line, "Tie");
	}
	
    strcat(line, " | Final Score: ");
    sprintf(numbuf, "%d", myScore);
    strcat(line, numbuf);
    strcat(line, "-");
    sprintf(numbuf, "%d", opponentScore);
    strcat(line, numbuf);
    strcat(line, "\n");
	
    // Append to file
    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd == -1) {
        perror("appendScore open");
        return;
    }
    write(fd, line, strlen(line));
    close(fd);
}

void printStats(char *username) {
    char path[128] = {0};
    char buffer[256] = {0};
    int fd;
    int bytes;
	
    // Build path: userStats/<username>.txt
    strcpy(path, "userStats/");
    strcat(path, username);
    strcat(path, ".txt");
	
    fd = open(path, O_RDONLY);
    if (fd == -1) {
        printf("No stats found for user '%s'.\n", username);
        return;
    }
	
    printf("\n===== Match History for %s =====\n", username);
	
	bytes = read(fd, buffer, sizeof(buffer) - 1);

    // Read and print file contents
    while (bytes > 0) {
        buffer[bytes] = '\0'; 
        printf("%s", buffer);
		bytes = read(fd, buffer, sizeof(buffer) - 1);
    }
	
    printf("================================\n\n");
	
    close(fd);
}

	// Asks for user move and sends to server. Then waits for server response for the game.
	// Displays the result to user.
	// Later: can have the option to ask for username, send to server, and get opponent's username and display etc.
	// Later 2: after each match ask if the users want another game. Redo if both replied yes.
	// Later 3: if no response after a certain time duration (e.g. 10s) quit automatically.

	void client_logic(int server_socket){
		char opponentName[64] = {0}; //grab opponent username
		char username[64] = {0};
		char usertemp[64] = {0};
		char userbool[8] = {0};
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
					int nameFitsRequirement = 0;
					while(!nameFitsRequirement){
						printf("Enter your username below:\n");
						if(fgets (usertemp, 63, stdin) == NULL){
							printf("Please enter a shorter username!\n");
							continue;
						}
						usertemp[strcspn(usertemp, "\n")] = '\0';
						if(username_exist(usertemp)){
							printf("Username already exists! Try another one.\n");
							continue;
						}
						nameFitsRequirement = 1; // if nothing's wrong, break the loop. 
						hasUsername = 1;
					}
				}
				if (usernameBool == 1){
					strcpy(usertemp, "opponent");
				}
				strcpy(username, usertemp);
				username[63] = '\0';
				send(server_socket, &username, sizeof(username), 0);
			}
			else {
				usernameBool = -1;
			}
		}
		if(hasUsername){
			strcpy(userName, username); // copies to global variable
			char path[128] = {0};
			// Build path: userStats/ + username + .txt
			strcpy(path, "userStats/");
			strcat(path, username);
			strcat(path, ".txt");
			int fd = open(path, O_CREAT | O_WRONLY | O_APPEND, 0666);
			if (fd == -1) {
				perror("Could not create userStats file");
			} 
			else {
				close(fd);
			}
		}
		
		//placate customer while waiting
		//maybe 'welcome x'
		if (strcmp(username, "opponent") == 0){
				//if no username was entered
				printf("Welcome, guest who shall remain unnamed.\nPlease wait while we connect you to a fellow player...\n");
			}
			else {
				printf("Welcome, %s.\nPlease wait while we connect you to a fellow player...\n", username);
			}

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
		
		printf("Feel free to type in 'history' to see past game stats\n");
		while (gamesPlayed < 2){ //i.e., repeat until 3 rounds have been played
			int bytes_received = recv(server_socket, &gamesPlayed, sizeof(int), 0);
			if (bytes_received <= 0){
				perror("Client exiting due to empty message...\n");
				exit(0);
			}
			printf ("\n~~~~~~~~~~~~~\n   ROUND %d\n~~~~~~~~~~~~~\n", gamesPlayed+1);
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

				if (FD_ISSET(STDIN_FILENO, &read_fds)) {
					char buf[32] = {0};
					fgets(buf, sizeof(buf), stdin);
					buf[strcspn(buf, "\n")] = 0;
					if(strcmp(buf, "history") == 0){
						printStats(username);
						continue;
					}
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
							printf("Waiting for %s's move...\n", opponentName);
						}

						else {
							printf("Invalid input! Please try again.\n");
							flush_stdin();
							continue;
						}

				}

				if (FD_ISSET(server_socket, &read_fds)) {
					break;
				}
			}

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
			
			//checks for replay
			if(gamesPlayed == 2){
				printf("The score is %d to %d\n", myScore, opponentScore);
				appendScore(username, opponentName, myScore, opponentScore);
				int myOpinion = askReplay(server_socket);
				if(myOpinion == 0){ // if user don't want to play anymore
					break;
				}
				int replayOrNot;
				recv(server_socket, &replayOrNot, sizeof(int), 0);
				if(replayOrNot == 0){ // opponent doesn't want to play anymore
					printf("Opponent exited. \n");
					break;
				}
				else{
					myScore = 0;
					opponentScore = 0;
					gamesPlayed = 0;
					printf("Opponent agreed to replay\n");
					sleep(1);
				}
			}
		}
		if(hasUsername){
			release_username(username);
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
		signal(SIGINT, sighandler);
		char* IP = "127.0.0.1";
		if (argc > 1){
			strcpy(IP, argv[1]);
		}
		int server_socket = connect_to_server(IP);
		client_logic(server_socket);

		//before exiting, send a 'down' to server to reduce # of connections by 1
		return (0);
	}
