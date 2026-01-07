#include 'Networking.h'
// The purpose of this .c is to send stuff (like user input) to server 
// or to get stuff (like results) from server. 


	// Asks for user move and sends to server. Then waits for server response for the game. 
	// Displays the result to user. 
	// Later: can have the option to ask for username, send to server, and get opponent's username and display etc. 
	// Later 2: after each match ask if the users want another game. Redo if both replied yes. 
	// Later 3: if no response after a certain time duration (e.g. 10s) quit automatically. 
	
	void client_logic(int server_socket){ 
		
	}
	
	// a wrapper function that calls clinet_connect(char* IP, char* port)
	// which returns connected socket descriptor. 
	
	int connect_to_server(char* IP){
		
	}
	
	// optionally takes IP address from user input 
	// connects to the server (call connect_to_server)
	// runs the client loop (call clinet_loop())
	// close up socket at the end
	
	int main(int argc, char* argv[]){
		
	}