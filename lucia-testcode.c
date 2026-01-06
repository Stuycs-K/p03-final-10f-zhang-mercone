

#define ROCK 0
#define PAPER 1
#define SCISSORS 2

//sketch of client ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//does a connection song and dance
//ask for input, either 0/1/2 corresponding to r/p/s or strcmp to find 'rock', 'paper', and 'scissors'
//sscanf for the former and if/else otherwise
//


//this would be in the subserver ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//will take two scores + two moves, compare moves and add one to the correct score
void scorehandler (int games_played, int a_score, int b_score, int a_move, int b_move){
  if ( (compare_move (a_move, b_move)) == 1){
    a_score++;
    games_played++;
  }
  if ( (compare_move (a_move, b_move)) == 0){
    b_score++;
    games_played++;
  }
  if ( (compare_move (a_move, b_move)) == -1){
    //tie
    //don't add one
  }

}

//compares two moves for the correct
//returns 1 for a win
//returns 0 for b win
//returns -1 for tie
int compare_move(int a_move, int b_move){
  int winner = 0;
  if (a_move == ROCK){
    if (b_move == ROCK){
      winner = -1; //tie
    }
    if (b_move == PAPER){
      winner = 0; //b wins
    }
    if (b_move == SCISSORS){
      winner = 1; //a wins
    }
    else {
      printf ("This is an error and should not occur.\n");
    }
  }

  if (a_move == PAPER){
    if (b_move == PAPER){
      winner = -1; //tie
    }
    if (b_move == SCISSORS){
      winner = 0; //b wins
    }
    if (b_move == ROCK){
      winner = 1; //a wins
    }
    else {
      printf ("This is an error and should not occur.\n");
    }
  }
  if (a_move == SCISSORS){
    if (b_move == SCISSORS){
      winner = -1; //tie
    }
    if (b_move == ROCK){
      winner = 0; //b wins
    }
    if (b_move == PAPER){
      winner = 1; //a wins
    }
    else {
      printf ("This is an error and should not occur.\n");
    }
  }

  else {
    printf ("This is an error and should not occur. No move was assigned to A.\n");
  }
  return winner;
}
