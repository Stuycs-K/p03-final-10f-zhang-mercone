#include 'Networking.h'

	// accept clients using server_accept
	// add clients to waiting queue
	// when 2 clients available: call fork_subserver(clientA, clinetB)
	// don't end unless ctrl-c
	void server_logic_loop(int server_fd){
		
	}
	
	// creates a child process to run the match between clientA and clientB
	// Child: closes listening socket, call run_match(clientA_fd, clientB_fd). exit when match ends
	// Parent: closes both client sockets. Return to logic loop. 
	
	void fork_subserver(int clientA_fd, int clientB_fd){
		
	}
	
	// clearer substitute for calling signal(SIGNUM, sighandler)
	// checks for SIGCHLD and SIGINT
	void setup_sighandlers(){
		
	}
	
	// handles SIGCHLD and SIGINT
	static void sighandler(int signo){
		
	}