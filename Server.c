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
#include 'Networking.h'
#include 'GameLogic.h'

// accept clients using server_accept
// add clients to waiting queue
// when 2 clients available: call fork_subserver(clientA, clinetB)
// don't end unless ctrl-c
void server_logic_loop(int server_fd, int num_connected){
	//if a client_server handshake is successful, up num_connected by 1
	//if num_connected would be >= max #, backlog the connect and refuse it
}

// creates a child process to run the match between clientA and clientB
// Child: closes listening socket, call run_match(clientA_fd, clientB_fd). exit when match ends
// Parent: closes both client sockets. Return to logic loop.
void fork_subserver(int clientA_fd, int clientB_fd){

}

//Main server logic
//Runs server_logic_loop
int main (){
	int listen_socket = server_setup();
	int connected = 0;
	//one time setup for a listening socket
	while (1){
		//server_logic_loop
	}
	return 0;
}



// clearer substitute for calling signal(SIGNUM, sighandler)
// checks for SIGCHLD and SIGINT
void setup_sighandlers(){

}

// handles SIGCHLD and SIGINT
static void sighandler(int signo){

}
