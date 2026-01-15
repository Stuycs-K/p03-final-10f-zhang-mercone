#include "Networking.h"
#include "GameLogic.h"


	// handles SIGCHLD and SIGINT
	static void sighandler(int signo){
		if(signo == SIGINT){
			printf("Server closed. Hope you had fun! \n");
			exit(0);
		}
		if(signo == SIGCHLD){
			int status;
			while(waitpid(-1, &status, WNOHANG)>0){
				if(!WIFEXITED(status)){
					printf("Child exited.\n");
				}
			}
		}
	}

	// clearer substitute for calling signal(SIGNUM, sighandler)
	// checks for SIGCHLD and SIGINT
	void setup_sighandlers(){
		signal(SIGINT, sighandler);
		signal(SIGCHLD, sighandler);
	}


	// removes first 2 clients in the Queue. returns number of clients left in Queue;
	int moveClientsUp(int Queue[], int numClients){
		int clientsleft = numClients;
		for(int i = 0; i < numClients -2; i ++){
			Queue[i] = Queue[i+2];
		}
		Queue[numClients-1] = -1;
		Queue[numClients-2] = -1;
		return clientsleft -2;
	}

	// creates a child process to run the match between clientA and clientB
	// Child: closes listening socket, call run_match(clientA_fd, clientB_fd). exit when match ends
	// Parent: closes both client sockets. Return to logic loop.
	void fork_subserver(int clientA_fd, int clientB_fd){
		printf("Running fork_subserver...\n");
		int pid = fork();
		if(pid == 0){ // child
			printf("I am a forked server-child....\n");
			run_match(clientA_fd, clientB_fd);
			close(clientA_fd);
			close(clientB_fd);
			exit(0);
		}
		else{ // parent
			printf("I am parent - disregard second one this is expected behavior.\n");
			close(clientA_fd);
			close(clientB_fd);
			return;
		}
	}

	// accept clients using server_accept
	// add clients to waiting queue
	// when 2 clients available: call fork_subserver(clientA, clinetB)
	// don't end unless ctrl-c
	void server_logic_loop(int server_fd){
		int num_Connected = 0;
		int waitingQueue[MAX_CONNECTIONS];
		for(int i = 0; i < MAX_CONNECTIONS; i ++){
			waitingQueue[i] = -1;
		}
		while (1){
			printf("Waiting for clients\n");
			int client_socket = server_tcp_handshake(server_fd);
			if (client_socket < 0){
				perror("Client-Server handshake failed.\n");
				continue;
			}
			printf("Client-Server handshake successful.\n");
			//if a client_server handshake is successful, up num_connected by 1
			//put the client's descriptor into an array
			//if num_connected would be >= max #, backlog the connect and refuse it
			printf("%d\n", num_Connected);
			printf("%d\n", MAX_CONNECTIONS);
			if (num_Connected < MAX_CONNECTIONS){
				printf("Number of connections is acceptable. Proceeding.\n");
				waitingQueue[num_Connected] = client_socket;
				num_Connected ++;
				printf("Current number of connections: %d\nAdded to waiting queue.\n", num_Connected);
				while(num_Connected > 1){
					fork_subserver(waitingQueue[0], waitingQueue[1]);
					num_Connected = moveClientsUp(waitingQueue, num_Connected);
				}
			}
		}
	}

	//Main server logic
	//Runs server_logic_loop
	int main (){
		//one time setup for a listening socket
		setup_sighandlers();
		int listen_socket = server_setup();
		server_logic_loop(listen_socket);
		close(listen_socket);
		return 0;
	}
