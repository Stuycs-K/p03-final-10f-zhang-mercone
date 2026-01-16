# Dev Log:

This document must be updated daily every time you finish a work session.

## Lucia Mercone

### 2026-01-06 - Worked on proposal
- Worked out proposal of project in class: which files are necessary, overall plan
  - Client: Handles client-side interaction
  - Server:
  - GameLogic: Handles game-related functions, hooks into both client and server.
    The intention was to have this section be separate, so as to be more easily
    replaced/modified in case we change our minds/something comes up.
  - Leaderboard: Should handle reading/writing to file so as to store a score.
      - Could store based on IP, PID, or client ID (discuss)
  - Networking: handles networking protocols.


### 2026-01-07 - Created skeleton of project
- Added necessary headers to files
- Added commenting on files + small edits
- Added further functions and a bit of the code for client

### 2026-01-13
- Several days later, I am no longer dead (Yay!)
- Attempted to implement ping system to check if server is dead and then auto-kill client. Multiple versions were tried and then quickly tested:
  - SIGPIPE: first, I tried using a sighandler in client.c, which would catch SIGPIPE. I got the idea from the ssh connection we use to connect, which reads 'broken pipe' after connection disconnects. If client tries to write to a dead server (i.e. the file descriptor for server_socket is -1), SIGPIPE should be sent. This did not function when a basic version was implemented, but feels like a viable option.
  - 'ping': a small program that sends/receives an int to check if the server is still active. The recv() in this has the flags 'MSG_PEEK' and 'MSG_DONTWAIT' to not remove data from queue (as that would interfere with client-to-server communication) and not throw EAGAIN/EWOULDBLOCK errors respectively. I initially struggled with getting the wrong results (i.e. 'server is dead' messages when it was in fact alive). This was due to the ping's recv causing issues in the game due to EAGAIN (due to there being no corresponding send() in server), and EWOULDBLOCK was mentioned in the Stack Exchange explanation of EAGAIN that I looked up as enhancing portability despite the two being congruent. I had the flag 'MSG_DONTWAIT' to prevent this but it did not in fact prevent these errors (as the Linux Manual states that MSG_DONTWAIT 'Enables nonblocking operation; if the operation would block, the call fails with the error EAGAIN or EWOULDBLOCK' thus I hoped that since no blocking, there would be no errors).
  The implementation of this was a bit difficult, as the loop of the client is dependent on client-side user behavior (i.e., fgets() waits forever for client-side user to input), so having a 'while ping != connection lost' loop on top only messed with the existing logic rather than functioning. The code for the 'ping' is still in networking.c, but is only called in a debug print statement in client.
  - getsockopt(): Since the 'ping' seemed difficult to implement, getsockopt() was briefly tried, as according to documentation, it would return error if the socket specified was not active. Closed, not worth pursuing.
 - Socket configuration: This was not tested (as I am doing this write-up on the 14th and just occurred to me), but when researching socket behavior I stumbled onto the socket configuration 'SO_KEEPALIVE'; however, the caveat of it only working with IP4 and IP6, I am unsure if this is a good idea. Shelved for in-class discussion with partner 1/14.
  - poll: this was mentioned in another Stack Exchange forum post, but since it was not taught in class and seems like it would require further work, I shelved this as a possible solution but did not pursue.
- Added debug statements to solve the following issue:
  - A set of matched clients are supposed to send their moves for to the subserver, and wait for the other client (i.e., move 1, A enters and waits for B. When B enters A is allowed to enter another move). Instead, this happens once or twice, before one of the two clients can enter 2+ moves when the other is still on a previous 'turn'.
  - Gamesplayed (the variable set to keep track of 'rounds' of completed games, not including ties) somehow decreases/changes randomly, which may trigger or otherwise contribute to the previous issue.

### 2026-01-14
- Debugging continues:
  - GamesPlayed variation is caused due to the client actually recieving moveint (the move of the opposite client) and subbing it into gamesPlayed.
- cute little semi-UI added to client to make it look more palatable.
- First draft of username interface created. Did not function correctly, was temporarily commented out.

### 2026-01-15
- Username implemented correctly this morning; the issue was due to the string being sent being shorter than 64 bytes, resulting in size mismatches when the opponent recieves your username. This mismatch means 64 bytes are read into queue, when only the arbitrary size would actually get read, and result in the remaining bytes clogging queue.

### 2026-01-16
-
