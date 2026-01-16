.PHONY: client server clean

compile: client server

client: Client.o Networking.o GameLogic.o
	@gcc -o client Client.o Networking.o GameLogic.o

server: Server.o Networking.o GameLogic.o
	@gcc -o server Server.o Networking.o GameLogic.o

Client.o: Client.c Networking.h GameLogic.h
	@gcc -Wall -c Client.c

Server.o: Server.c Networking.h GameLogic.h
	@gcc -Wall -c Server.c

Networking.o: Networking.c Networking.h
	@gcc -Wall -c Networking.c

GameLogic.o: GameLogic.c GameLogic.h
	@gcc -Wall -c GameLogic.c

clean:
	rm -rf *.o *~ client server userStats *.lock
