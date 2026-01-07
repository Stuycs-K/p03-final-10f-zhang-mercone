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
		int gameEnd = 0;

		int
		while (gameEnd < 3){ //i.e., repeat until 3 rounds have been played
			int bytes_received = recv(client_socket, buff, sizeof(int), 0);
		} //end while loop
		printf("Game ended.\n");
	}

	// a wrapper function that calls client_connect(char* IP, char* port)
	// which returns connected socket descriptor.
	int connect_to_server(char* IP){
		client_connect(IP, PORT);
	}

	// optionally takes IP address from user input
	// connects to the server (call connect_to_server)
	// runs the client loop (call client_loop())
	// close up socket at the end

	int main(int argc, char* argv[]){
		char * IP;
		if (argc > 1){
			strcpy(IP, argv[1]);
		}

		int server_socket = connect_to_server(IP);
		client_loop();

		//before exiting, send a 'down' to server to reduce # of connections by 1
		exit(0);
	}
