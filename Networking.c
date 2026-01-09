#include "Networking.h"
#include "GameLogic.h"
	/*Create and bind a socket.
	* Place the socket in a listening state.
	* returns the socket descriptor
	*/
	int server_setup() {
		
	  //setup structs for getaddrinfo
	  struct addrinfo * hints, * results;
	  hints = calloc(1, sizeof(struct addrinfo));
	  hints->ai_family = AF_INET;
	  hints->ai_socktype = SOCK_STREAM;
	  hints->ai_flags = AI_PASSIVE;	
	  getaddrinfo(NULL,PORT,hints,&results);
	  
	  //create the socket
	  int clientd;//store the socket descriptor here
	  clientd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
	  
	  //this code should get around the address in use error
	  int yes = 1;
	  int sockOpt =  setsockopt(clientd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	  err(sockOpt,"sockopt  error");
	  
	  //bind the socket to address and port
	  bind(clientd, results->ai_addr, results->ai_addrlen);
	  
	  //set socket to listen state
	  listen(clientd,10);

	  //free the structs used by getaddrinfo
	  free(hints);
	  freeaddrinfo(results);
	  
	  return clientd;
	}

	/*Accept a connection from a client
	 *return the socket descriptor for the new socket connected to the client
	 *blocks until connection is made.
	 */
	int server_tcp_handshake(int listen_socket){
		int client_socket;
		
		//accept() the client connection
		int sock_size;
		struct sockaddr_storage client_address;
		sock_size = sizeof(client_address);
		client_socket = accept(listen_socket, (struct sockaddr *)&client_address, &sock_size);
		
		return client_socket;
	}


	/*Connect to the server
	 *return the to_server socket descriptor
	 *blocks until connection is made.*/
	int client_connect(char* IP, char* port) {

	  //getaddrinfo
	  struct addrinfo * hints, * results;
	  hints = calloc(1, sizeof(struct addrinfo));
	  hints->ai_family = AF_INET;
	  hints->ai_socktype = SOCK_STREAM;
	  hints->ai_flags = AI_NUMERICHOST;	
	  getaddrinfo(IP,port,hints,&results);
	  
	  int serverd;//store the socket descriptor here
	  //create the socket
	  serverd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
	  
	  //connect() to the server
	  connect(serverd, results->ai_addr, results->ai_addrlen);
	  
	  free(hints);
	  freeaddrinfo(results);

	  return serverd;
	}


	void err(int i, char*message){
	  if(i < 0){
		  printf("Error: %s - %s\n",message, strerror(errno));
		exit(1);
	  }
	}
	
	
	int run_match(int fdA, int fdB){
		int stillWantGame = 1;
		int gamesPlayed = 0;
		int Ascore = 0;
		int Bscore = 0;
		while(stillWantGame == 1){
			int AMove = -1;
			int BMove = -1;
			while (AMove == -1 || BMove == -1){
				fd_set recv_fds;
				FD_ZERO(&recv_fds);
				FD_SET(fdA, &recv_fds);
				FD_SET(fdB, &recv_fds);
				int maxFD;
				if(fdA > fdB){
					maxFD = fdA;
				}
				else{
					maxFD = fdB;
				}
				
				int i = select(maxFD+1, &recv_fds, NULL, NULL, NULL);
				if(FD_ISSET(fdA, &recv_fds)){
					if(recv(fdA, &AMove, sizeof(int), 0) == 0){
						stillWantGame = 0; 
						break;
					}
				}
				if(FD_ISSET(fdB, &recv_fds)){
					if(recv(fdB, &BMove, sizeof(int), 0) == 0){
						stillWantGame = 0; 
						break;
					} 
				}
			}
			
			if(send(fdB, &AMove, sizeof(int), 0) == 0){ // After moves from both clients are received, send opponent's move to A and B. In client, receive this message and display it. 
				stillWantGame = 0; 
				break;
			}
			if(send(fdA, &BMove, sizeof(int), 0) == 0){
				stillWantGame = 0; 
				break;
			}
			sendResult(fdA, fdB, AMove, BMove); // In server, after displaying opponent's move, receive 1, -1, or 0 (win, loss, tie) and display result. 
			scorehandler(gamesPlayed, Ascore, Bscore, AMove, BMove); // Changes statistics of winning status. 
			
			
		}
		return(stillWantGame);
	}
	
	void sendResult(int fdA, int fdB, int Amove, int Bmove){
		send(fdA, compare_move(Amove, Bmove), sizeof(int), 0); // if A wins, send 1; A loss, send -1; tie: 0
		send(fdB, compare_move(Bmove, Amove), sizeof(int), 0); // if B wins, send 1; B loss, send -1; tie: 0
	}
	

