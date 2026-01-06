# Final Project Proposal

## Group Members:

Jason Zhang and Lucia Mercone

# Intentions:

In this project, we are creating a rock paper scissor game server that allows users to pair up over TCP and play rock paper scissor
against each other. 



# Intended usage:

The user runs a client program in the terminal. 
The client connects to the server, waits for an opponent, sends the player’s move, and shows the result. 
The server runs separately, accepts players, pairs them, and starts a match for each pair. 
Everything is done with plain text messages.




# Technical Details:

### Minimal Viable Product (MVP)

#### Systems Concepts Used
- **Sockets:** Client and server talk over TCP. 
- **Processes:** The server forks a child process for each match. 
- **Signals:** Use SIGCHLD to clean up children and SIGINT to shut down safely. 
- **Files + stat():** Save match results to a file and check file info with stat().

### Responsibilities

#### Lucia
- Write the client program 
- Write the basic server (socket setup, accepting players, pairing players) 
- Write the game logic (checking moves, deciding winner) 
- Decide the message format between client and server 
- Make sure client and server work together

#### Jason
- Add forking so each match runs in its own process 
- Add signal handling (SIGCHLD, SIGINT) 
- Add file logging and use stat() 
- Test for race conditions and make sure the server is stable

## Add‑Ons (If Time Allows)
- Shared memory + semaphores for a waiting queue 
- Player stats saved in a binary file 
- Replay option 
- Tournament mode 
- Leader board
- Support for many matches running at the same time



# Intended pacing:

## Jan 5
- Choose partners and confirm project topic.
- Write and submit PROPOSAL.md.

## Jan 7
- Set up repo and project structure.
- Basic client and server created.
- Client can connect to server over TCP.

## Jan 8
- Server can accept multiple clients.
- Pairing system working (two players get matched).

## Jan 9
- Game logic implemented (rock/paper/scissors rules).
- Client and server can exchange moves correctly.

## Jan 12 (MVP COMPLETE)
- Forking added (each match runs in its own process).
- Signals working (SIGCHLD cleanup, SIGINT shutdown).
- File logging implemented.
- stat() used to check log file info.
- Full MVP tested end‑to‑end.

## Jan 13
- Code cleanup and bug fixes.
- Improve client/server messages and flow.

## Jan 14
- Additional testing.
- Optional add‑ons if time allows.

## Jan 15
- Final polish.
- Prepare and finalize presentation.

## Jan 16 (Due 3:30 PM)
- Final commits.
- Submit presentation link.


