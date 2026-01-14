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
  - 'ping': a small program that sends/recieves an int to check if the server is still active. I initially struggled with getting the wrong results (i.e. 'server is dead' messages when it was in fact alive). This was due to the ping's recv causing issues in the game due to EAGAIN (due to there being no corresponding send() in server), and EWOULDBLOCK was mentioned in the Stack Exchange explanation of EAGAIN that I looked up as enhancing portability despite the two being congruent.
  The implementation of this was a bit difficult, as the loop of the client is dependent on client-side user behavior (i.e., fgets() waits forever for client-side user to input), so having a 'while ping != connection lost' loop on top only messed with the existing logic rather than functioning. The code for the 'ping' is still in networking.c, but is only called in a debug print statement in client.
  - getsockopt(): Since the 'ping' seemed difficult to implement, getsockopt() was briefly tried, as according to documentation, it would return error if the socket specified was not active. Closed, not worth pursuing.
 - Socket configuration: This was not tested (as I am doing this write-up on the 14th and just occurred to me), but when researching socket behavior I stumbled onto the socket configuration 'SO_KEEPALIVE'; however, the caveat of it only working with IP4 and IP6, I am unsure if this is a good idea. Shelved for in-class discussion with partner 1/14.
  - poll: this was mentioned in another Stack Exchange forum post, but since it was not taught in class and seems like it would require further work, I shelved this as a possible solution but did not pursue.

### 2026-01-14
-
