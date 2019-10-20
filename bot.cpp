#include <cstdlib>
#include <cmath>
#include <iostream>
#include "bot.h"

using namespace std;

const int MAX_ROWS = 40;
const int MAX_COLS = 40;
const int MAX_NUM = 10;

int ROWS;  // global variables
int COLS;
int NUM;

int GAME_STATE;
/*                                                                                                                                                                               
  game state determines what the dwarfs should be doing                                                                                                               
  0 - chopping wood (all of d1)                                                                                                                                               
  1 - surrounding w/ fences (before zombies)                                                                                                                                    
  2 - break surrounding fences (morning)                                                                                                                                      
  3 - chop some pine trees (all of d2)                                                                                                                                         
  4 - pick apples & pumpkins (rest                                                                                                                                            
  5 - build fences to get real biggie (ending)                                                                                                                               
*/

int isNextToATree(Dwarf & dwarf, int r, int c);
int distance(int r1, int r2, int c1, int c2);
Loc closestTree(Dwarf &dwarf, int dwarf_row, int dwarf_col);
void chopTrees(Dwarf &dwarf, ostream &log);
int numFences(Dwarf &dwarf);                                                                                                                             
bool isNextToAFence(Dwarf & dwarf, int r, int c);
Loc closestFence(Dwarf &dwarf, int dwarf_row, int dwarf_col);
void buildFirstFence(Dwarf &dwarf, int r, int c, ostream &log);
void buildFences(Dwarf &dwarf, int r, int c);
void buildFenceFour(Dwarf &dwarf, int r, int c, ostream &log);
void chopFenceFour(Dwarf &dwarf, int r, int c, ostream &log);
Loc closestApplePumpkin(Dwarf &dwarf, int dwarf_row, int dwarf_col);
void collectApplePumpkin(Dwarf &dwarf, int r, int c, ostream &log);
bool isIsolated(Dwarf &dwarf, int dwarf_row, int dwarf_col);
Loc closestIsolated(Dwarf &dwarf, int dwarf_row, int dwarf_col);
void barricade(Dwarf &dwarf, int r, int c, ostream &log);

/* onStart: 
An Initialization procedure called at the start of the game.
You can use it to initialize certain global variables, or do 
something else before the actual simulation starts.
Parameters:
    rows: number of rows
    cols: number of columns
    num:  number of dwarfs
    log:  a cout-like log */

void onStart(int rows, int cols, int num, std::ostream &log) {
  log << "Start!" << endl; // Print a greeting message

  ROWS = rows; // Save values in global variables
  COLS = cols;
  NUM = num;

  GAME_STATE = 0;
}

/*
  returns the distance between the two points
 */

int distance(int r1, int c1, int r2, int c2) {
  return (abs(r1 - r2) + abs(c1 - c2));
}

/* isNextToATree
returns: 
0 if no trees
1 if north
2 if east
3 if south
4 if west
*/

int isNextToATree(Dwarf & dwarf, int r, int c) {

  if (dwarf.look(r+1, c) == PINE_TREE || dwarf.look(r+1, c) == APPLE_TREE) {
    // tree to south
    return 3;
  }
  else if (dwarf.look(r, c+1) == PINE_TREE || dwarf.look(r, c+1) == APPLE_TREE) {
    // tree to east
    return 2;
  }
  else if (dwarf.look(r-1, c) == PINE_TREE || dwarf.look(r-1, c) == APPLE_TREE) {
    // tree to north
    return 1;
  }
  else if (dwarf.look(r, c-1) == PINE_TREE || dwarf.look(r, c-1) == APPLE_TREE) {
    // tree to west
    return 4;
  }  
  else {
    return 0;
  }
}

/* closestTree
returns location of closest tree in structure Coord
finds closest tree via linearSearch
*/

Loc closestTree(Dwarf &dwarf, int dwarf_row, int dwarf_col) {

  int closestDistance = 1000000;
  Loc closestSpot = {-1, -1};

  for (int r = 0; r <= ROWS; r++) {
    for (int c = 0; c <= COLS; c++) {
      //if it is a tree
      if (dwarf.look(r, c) == PINE_TREE || dwarf.look(r, c) == APPLE_TREE) {
	//if it is closer than current closest
	if (distance(dwarf_row, dwarf_col, r, c+1) < closestDistance && dwarf.look(r,c+1) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r, c+1);
	  closestSpot.r = r;
	  closestSpot.c = c+1;
	}
	if (distance(dwarf_row, dwarf_col, r+1, c) < closestDistance && dwarf.look(r+1,c) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r+1, c);
	  closestSpot.r = r+1;
	  closestSpot.c = c;
	}
	if (distance(dwarf_row, dwarf_col, r, c-1) < closestDistance && dwarf.look(r,c-1) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r, c-1);
	  closestSpot.r = r;
	  closestSpot.c = c-1;
	}
	if (distance(dwarf_row, dwarf_col, r-1, c) < closestDistance && dwarf.look(r-1,c) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r-1, c);
	  closestSpot.r = r-1;
	  closestSpot.c = c;
	}
      }      
    }
  }

  return closestSpot;

}

/*
returns the number of fences currently on the grid
 */

int numFences(Dwarf &dwarf) {
  
  int retVal = 0;
  for (int r = 0; r <= ROWS; r++) {
    for (int c = 0; c <= COLS; c++) {
      if (dwarf.look(r, c) == FENCE) {
	retVal += 1;
      }
    }
  }
  return retVal;

}

/*
  returns if Loc {r, c} is next to a fence
 */
bool isNextToAFence(Dwarf & dwarf, int r, int c) {

  if (dwarf.look(r, c) == EMPTY) {
    if (dwarf.look(r+1, c) == FENCE || dwarf.look(r, c+1) == FENCE || dwarf.look(r, c-1) == FENCE || dwarf.look(r-1, c) == FENCE) {    
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

/*
  returns the closest fence in Loc format
*/

Loc closestFence(Dwarf &dwarf, int dwarf_row, int dwarf_col) {

  int closestDistance = 1000000;
  Loc closestSpot = {-1, -1};

  for (int r = 0; r <= ROWS; r++) {
    for (int c = 0; c <= COLS; c++) {
      //if it is a fence                                                                                                                                                         
      if (dwarf.look(r, c) == FENCE) {
        //if it is closer than current closest                                                                                                                                 
	//2 away
        if (distance(dwarf_row, dwarf_col, r, c+2) < closestDistance && dwarf.look(r,c+2) == EMPTY) {
          closestDistance = distance(dwarf_row, dwarf_col, r, c+2);
          closestSpot.r = r;
          closestSpot.c = c+2;
        }
        if (distance(dwarf_row, dwarf_col, r+2, c) < closestDistance && dwarf.look(r+2,c) == EMPTY) {
          closestDistance = distance(dwarf_row, dwarf_col, r+2, c);
          closestSpot.r = r+2;
          closestSpot.c = c;
        }
        if (distance(dwarf_row, dwarf_col, r, c-2) < closestDistance && dwarf.look(r,c-2) == EMPTY) {
          closestDistance = distance(dwarf_row, dwarf_col, r, c-2);
          closestSpot.r = r;
          closestSpot.c = c-2;
        }
        if (distance(dwarf_row, dwarf_col, r-2, c) < closestDistance && dwarf.look(r-2,c) == EMPTY) {
          closestDistance = distance(dwarf_row, dwarf_col, r-2, c);
          closestSpot.r = r-2;
          closestSpot.c = c;
        }
	//diagonals
        if (distance(dwarf_row, dwarf_col, r-1, c-1) < closestDistance && dwarf.look(r-1,c-1) == EMPTY) {
          closestDistance = distance(dwarf_row, dwarf_col, r-1, c-1);
          closestSpot.r = r-1;
          closestSpot.c = c-1;
        }
        if (distance(dwarf_row, dwarf_col, r+1, c-1) < closestDistance && dwarf.look(r+1,c-1) == EMPTY) {
          closestDistance = distance(dwarf_row, dwarf_col, r+1, c-1);
          closestSpot.r = r+1;
          closestSpot.c = c-1;
        }
        if (distance(dwarf_row, dwarf_col, r-1, c+1) < closestDistance && dwarf.look(r-1,c+1) == EMPTY) {
          closestDistance = distance(dwarf_row, dwarf_col, r-1, c+1);
          closestSpot.r = r-1;
          closestSpot.c = c+1;
        }
        if (distance(dwarf_row, dwarf_col, r+1, c+1) < closestDistance && dwarf.look(r+1,c+1) == EMPTY) {
          closestDistance = distance(dwarf_row, dwarf_col, r+1, c+1);
          closestSpot.r = r+1;
          closestSpot.c = c+1;
        }
      }
    }
  }

  return closestSpot;

}

/*
  tells dwarf to chop trees
 */

void chopTrees(Dwarf &dwarf, ostream &log) {
  // Get current position of the dwarf
  int r = dwarf.row();
  int c = dwarf.col();
  // Look if there is a tree adjacent to the dward                                                                                                                              
  if (isNextToATree(dwarf, r, c) == 1) {
    // If there is a tree to the north, chop it                                                                                                                                 
    //log << "Found a tree -- chop N" << endl;
    dwarf.start_chop(NORTH);
    return;
  }
  else if (isNextToATree(dwarf, r, c) == 2) {
    // If there is a tree to the east, chop it                                                                                                                                  
    //log << "Found a tree -- chop E" << endl;
    dwarf.start_chop(EAST);
    return;
  }
  else if (isNextToATree(dwarf, r, c) == 3) {
    // If there is a tree to the south, chop it                                                                                                                                 
    //log << "Found a tree -- chop S" << endl;
    dwarf.start_chop(SOUTH);
    return;
  }
  else if (isNextToATree(dwarf, r, c) == 4) {
    // If there is a tree to the west, chop it                                                                                                                                  
    //log << "Found a tree -- chop W" << endl;
    dwarf.start_chop(WEST);
    return;
  }
  else {
    // Otherwise, move to closest tree location                                                                                                                                 
    Loc closestSpot = closestTree(dwarf, r, c);
    int rr = closestSpot.r;
    int cc = closestSpot.c;
    //log << "Walk to " << rr << " " << cc << endl;
    dwarf.start_walk(rr, cc);
    return;
  }
}

/*
  is next to pine tree
 */

int isNextToAPineTree(Dwarf & dwarf, int r, int c) {

  if (dwarf.look(r+1, c) == PINE_TREE) {
    // tree to south
    return 3;
  }
  else if (dwarf.look(r, c+1) == PINE_TREE) {
    // tree to east
    return 2;
  }
  else if (dwarf.look(r-1, c) == PINE_TREE) {
    // tree to north
    return 1;
  }
  else if (dwarf.look(r, c-1) == PINE_TREE) {
    // tree to west
    return 4;
  }  
  else {
    return 0;
  }
}

/*
  find closest pine tree
 */

Loc closestPineTree(Dwarf &dwarf, int dwarf_row, int dwarf_col) {

  int closestDistance = 1000000;
  Loc closestSpot = {-1, -1};

  for (int r = 0; r <= ROWS; r++) {
    for (int c = 0; c <= COLS; c++) {
      //if it is a tree
      if (dwarf.look(r, c) == PINE_TREE) {
	//if it is closer than current closest
	if (distance(dwarf_row, dwarf_col, r, c+1) < closestDistance && dwarf.look(r,c+1) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r, c+1);
	  closestSpot.r = r;
	  closestSpot.c = c+1;
	}
	if (distance(dwarf_row, dwarf_col, r+1, c) < closestDistance && dwarf.look(r+1,c) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r+1, c);
	  closestSpot.r = r+1;
	  closestSpot.c = c;
	}
	if (distance(dwarf_row, dwarf_col, r, c-1) < closestDistance && dwarf.look(r,c-1) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r, c-1);
	  closestSpot.r = r;
	  closestSpot.c = c-1;
	}
	if (distance(dwarf_row, dwarf_col, r-1, c) < closestDistance && dwarf.look(r-1,c) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r-1, c);
	  closestSpot.r = r-1;
	  closestSpot.c = c;
	}
      }      
    }
  }

  return closestSpot;

}

/*
  chop pine trees lol
 */

void chopPineTrees(Dwarf &dwarf, ostream &log) {
  // Get current position of the dwarf
  int r = dwarf.row();
  int c = dwarf.col();
  // Look if there is a tree adjacent to the dward                                                                                                                              
  if (isNextToAPineTree(dwarf, r, c) == 1) {
    // If there is a tree to the north, chop it                                                                                                                                 
    //log << "Found a tree -- chop N" << endl;
    dwarf.start_chop(NORTH);
    return;
  }
  else if (isNextToAPineTree(dwarf, r, c) == 2) {
    // If there is a tree to the east, chop it                                                                                                                                  
    //log << "Found a tree -- chop E" << endl;
    dwarf.start_chop(EAST);
    return;
  }
  else if (isNextToAPineTree(dwarf, r, c) == 3) {
    // If there is a tree to the south, chop it                                                                                                                                 
    //log << "Found a tree -- chop S" << endl;
    dwarf.start_chop(SOUTH);
    return;
  }
  else if (isNextToAPineTree(dwarf, r, c) == 4) {
    // If there is a tree to the west, chop it                                                                                                                                  
    //log << "Found a tree -- chop W" << endl;
    dwarf.start_chop(WEST);
    return;
  }
  else {
    // Otherwise, move to closest tree location                                                                                                                                 
    Loc closestSpot = closestPineTree(dwarf, r, c);
    int rr = closestSpot.r;
    int cc = closestSpot.c;
    //log << "Walk to " << rr << " " << cc << endl;
    dwarf.start_walk(rr, cc);
    return;
  }
}


/*
  asks the first dwarf to build the first fence
 */

void buildFirstFence(Dwarf &dwarf, int r, int c, ostream &log) {
  if (r > ROWS/2) {
    if (dwarf.look(r-1, c) == EMPTY) {
      //log << "Placing first fence" << endl;
      dwarf.start_build(NORTH);
      return;
    }
    else {
      //log << "Cannot place, moving over" << endl;
      if (c > COLS/2) {
	dwarf.start_walk(r, c-1);
	return;
      }
      else {
	dwarf.start_walk(r, c+1);
	return;
      }
    }
  }
  else {
    if (dwarf.look(r+1, c) == EMPTY) {
      //log << "Placing first fence" << endl;
      dwarf.start_build(NORTH);
      return;
    }
    else {
      if (c > COLS/2) {
	dwarf.start_walk(r, c-1);
	return;
      }
      else {
	dwarf.start_walk(r, c+1);
	return;	    
      } 
    }
  }
}

/*
  creates 30 fences
 */
void buildFences(Dwarf &dwarf, int r, int c, ostream &log) {
  //----checking if areas around are fence adjacent
  
  if (isNextToAFence(dwarf, r+1, c)) {
    dwarf.start_build(SOUTH);
    return;
  }
  else if (isNextToAFence(dwarf, r-1, c)) {
    dwarf.start_build(NORTH);
    return;
  }
  else if (isNextToAFence(dwarf, r, c+1)) {
    dwarf.start_build(EAST);
    return;
  }
  else if (isNextToAFence(dwarf, r, c-1)) {
    dwarf.start_build(WEST);
    return;
  }

  //----done checking if areas around are fence adjacent
  
  else if (numFences(dwarf) > 0) { //walk to closest fence
    Loc closestSpot = closestFence(dwarf, r, c);
    int rr = closestSpot.r;
    int cc = closestSpot.c;
    //log << "Walk to " << rr << " " << cc << endl;
    dwarf.start_walk(rr, cc);
    return;
  }
  
  else {
    chopTrees(dwarf, log);
  }

}

/*
dwarf surrounds himself by walls
 */
void buildFenceFour(Dwarf &dwarf, int r, int c, ostream &log) {

  //look for trees around first
  if (dwarf.look(r+1, c) == PINE_TREE || dwarf.look(r+1,c) == APPLE_TREE) {
    dwarf.start_chop(SOUTH);
    return;
  }
  else if (dwarf.look(r-1, c) == PINE_TREE || dwarf.look(r-1,c) == APPLE_TREE) {
    dwarf.start_chop(NORTH);
    return;
  }
  else if (dwarf.look(r, c+1) == PINE_TREE || dwarf.look(r,c+1) == APPLE_TREE) {
    dwarf.start_chop(EAST);
    return;
  }
  else if (dwarf.look(r, c-1) == PINE_TREE || dwarf.look(r,c-1) == APPLE_TREE) {
    dwarf.start_chop(WEST);
    return;
  }
  //build around yourself to stay safe
  else if (dwarf.look(r+1, c) == EMPTY) {
    dwarf.start_build(SOUTH);
    return;
  }
  else if (dwarf.look(r-1, c) == EMPTY) {
    dwarf.start_build(NORTH);
    return;
  }
  else if (dwarf.look(r, c+1) == EMPTY) {
    dwarf.start_build(EAST);
    return;
  }
  else if (dwarf.look(r, c-1) == EMPTY) {
    dwarf.start_build(WEST);
    return;
  }
  //idk something off
  else {
    return;
  }
    
}

void chopFenceFour(Dwarf &dwarf, int r, int c, ostream &log) {

  // free yourself!
  if (dwarf.look(r+1, c) == FENCE) {
    dwarf.start_chop(SOUTH);
    return;
  }
  else if (dwarf.look(r-1, c) == FENCE) {
    dwarf.start_chop(NORTH);
    return;
  }
  else if (dwarf.look(r, c+1) == FENCE) {
    dwarf.start_chop(EAST);
    return;
  }
  else if (dwarf.look(r, c-1) == FENCE) {
    dwarf.start_chop(WEST);
    return;
  }
  //idk something off
  else {
    return;
  }
  
}



Loc closestApplePumpkin(Dwarf &dwarf, int dwarf_row, int dwarf_col) {

  int closestDistance = 1000000;
  Loc closestSpot = {-1, -1};

  for (int r = 0; r <= ROWS; r++) {
    for (int c = 0; c <= COLS; c++) {
      //if it is a tree
      if (dwarf.look(r, c) == PUMPKIN || dwarf.look(r, c) == APPLE_TREE) {
	//if it is closer than current closest
	if (distance(dwarf_row, dwarf_col, r, c+1) < closestDistance && dwarf.look(r,c+1) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r, c+1);
	  closestSpot.r = r;
	  closestSpot.c = c+1;
	}
	if (distance(dwarf_row, dwarf_col, r+1, c) < closestDistance && dwarf.look(r+1,c) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r+1, c);
	  closestSpot.r = r+1;
	  closestSpot.c = c;
	}
	if (distance(dwarf_row, dwarf_col, r, c-1) < closestDistance && dwarf.look(r,c-1) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r, c-1);
	  closestSpot.r = r;
	  closestSpot.c = c-1;
	}
	if (distance(dwarf_row, dwarf_col, r-1, c) < closestDistance && dwarf.look(r-1,c) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r-1, c);
	  closestSpot.r = r-1;
	  closestSpot.c = c;
	}
      }      
    }
  }

  return closestSpot;

}

void collectApplePumpkin(Dwarf &dwarf, int r, int c, ostream &log) {

  if (dwarf.look(r+1, c) == PUMPKIN || dwarf.look(r+1, c) == APPLE_TREE) {
    // tree to south
    dwarf.start_pick(SOUTH);
    return;
  }
  else if (dwarf.look(r, c+1) == PUMPKIN || dwarf.look(r, c+1) == APPLE_TREE) {
    // tree to east
    dwarf.start_pick(EAST);
    return;
  }
  else if (dwarf.look(r-1, c) == PUMPKIN || dwarf.look(r-1, c) == APPLE_TREE) {
    // tree to north
    dwarf.start_pick(NORTH);
    return;
  }
  else if (dwarf.look(r, c-1) == PUMPKIN || dwarf.look(r, c-1) == APPLE_TREE) {
    // tree to west
    dwarf.start_pick(WEST);
    return;
  }  
  else {
    // Otherwise, move to closest tree/pumpkin location                                                                                              
    Loc closestSpot = closestApplePumpkin(dwarf, r, c);
    int rr = closestSpot.r;
    int cc = closestSpot.c;
    //log << "Walk to " << rr << " " << cc << endl;
    dwarf.start_walk(rr, cc);
    return;
  }

}

bool isIsolated(Dwarf &dwarf, int r, int c) {
  bool retVal = (dwarf.look(r+1,c) == EMPTY || dwarf.look(r+1,c) == DWARF || dwarf.look(r+1,c) == FENCE) && 
    (dwarf.look(r-1,c) == EMPTY || dwarf.look(r-1,c) == DWARF || dwarf.look(r-1,c) == FENCE) && 
    (dwarf.look(r,c+1) == EMPTY || dwarf.look(r,c+1) == DWARF || dwarf.look(r,c+1) == FENCE) && 
    (dwarf.look(r,c-1) == EMPTY || dwarf.look(r,c-1) == DWARF || dwarf.look(r,c-1) == FENCE);
  return retVal;
}

Loc closestIsolated(Dwarf &dwarf, int dwarf_row, int dwarf_col) {

  int closestDistance = 1000000;
  Loc closestSpot = {-1, -1};
  
  for (int r = 2; r <= ROWS - 2; r++) {
    for (int c = 2; c <= COLS - 2; c++) {
      //if it isolated
      if (isIsolated(dwarf, r, c)) {
	if (distance(dwarf_row, dwarf_col, r, c) < closestDistance && dwarf.look(r,c) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r, c);
	  closestSpot.r = r;
	  closestSpot.c = c;
	}
      }            
    }
  }

  return closestSpot;

}

/*
  either build around yourself or find a place where you can build around yourself
 */

void barricade(Dwarf &dwarf, int r, int c, ostream &log) {
  
  if (isIsolated(dwarf, r, c)) {
    if (dwarf.look(r+1, c) == EMPTY) {
      dwarf.start_build(SOUTH);
      return;
    }
    else if (dwarf.look(r-1, c) == EMPTY) {
      dwarf.start_build(NORTH);
      return;
    }
    else if (dwarf.look(r, c+1) == EMPTY) {
      dwarf.start_build(EAST);
      return;
    }
    else if (dwarf.look(r, c-1) == EMPTY) {
      dwarf.start_build(WEST);
      return;
    } 
  }
  else {
    Loc closestSpot = closestIsolated(dwarf, r, c);
    int rr = closestSpot.r;
    int cc = closestSpot.c;
    //log << "Walk to " << rr << " " << cc << endl;
    dwarf.start_walk(rr, cc);
    return;
  }
}

int numPumpkins(Dwarf &dwarf) {
  
  int retVal = 0;
  for (int r = 0; r <= ROWS; r++) {
    for (int c = 0; c <= COLS; c++) {
      if (dwarf.look(r, c) == PUMPKIN) {
	retVal += 1;
      }
    }
  }
  return retVal;

}

int numPine(Dwarf &dwarf) {
  
  int retVal = 0;
  for (int r = 0; r <= ROWS; r++) {
    for (int c = 0; c <= COLS; c++) {
      if (dwarf.look(r, c) == PINE_TREE) {
	retVal += 1;
      }
    }
  }
  return retVal;

}

Loc closestApple(Dwarf &dwarf, int dwarf_row, int dwarf_col) {

  int closestDistance = 1000000;
  Loc closestSpot = {-1, -1};

  for (int r = 0; r <= ROWS; r++) {
    for (int c = 0; c <= COLS; c++) {
      //if it is a tree
      if (dwarf.look(r, c) == APPLE_TREE) {
	//if it is closer than current closest
	if (distance(dwarf_row, dwarf_col, r, c+1) < closestDistance && dwarf.look(r,c+1) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r, c+1);
	  closestSpot.r = r;
	  closestSpot.c = c+1;
	}
	if (distance(dwarf_row, dwarf_col, r+1, c) < closestDistance && dwarf.look(r+1,c) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r+1, c);
	  closestSpot.r = r+1;
	  closestSpot.c = c;
	}
	if (distance(dwarf_row, dwarf_col, r, c-1) < closestDistance && dwarf.look(r,c-1) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r, c-1);
	  closestSpot.r = r;
	  closestSpot.c = c-1;
	}
	if (distance(dwarf_row, dwarf_col, r-1, c) < closestDistance && dwarf.look(r-1,c) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r-1, c);
	  closestSpot.r = r-1;
	  closestSpot.c = c;
	}
      }      
    }
  }

  return closestSpot;

}

void collectApple(Dwarf &dwarf, int r, int c, ostream &log) {

  if (dwarf.look(r+1, c) == APPLE_TREE) {
    // tree to south
    dwarf.start_pick(SOUTH);
    return;
  }
  else if (dwarf.look(r, c+1) == APPLE_TREE) {
    // tree to east
    dwarf.start_pick(EAST);
    return;
  }
  else if (dwarf.look(r-1, c) == APPLE_TREE) {
    // tree to north
    dwarf.start_pick(NORTH);
    return;
  }
  else if (dwarf.look(r, c-1) == APPLE_TREE) {
    // tree to west
    dwarf.start_pick(WEST);
    return;
  }  
  else {
    // Otherwise, move to closest tree/pumpkin location                                                                                              
    Loc closestSpot = closestApple(dwarf, r, c);
    int rr = closestSpot.r;
    int cc = closestSpot.c;
    //log << "Walk to " << rr << " " << cc << endl;
    dwarf.start_walk(rr, cc);
    return;
  }

}

Loc closestPumpkin(Dwarf &dwarf, int dwarf_row, int dwarf_col) {

  int closestDistance = 1000000;
  Loc closestSpot = {-1, -1};

  for (int r = 0; r <= ROWS; r++) {
    for (int c = 0; c <= COLS; c++) {
      //if it is a tree
      if (dwarf.look(r, c) == PUMPKIN) {
	//if it is closer than current closest
	if (distance(dwarf_row, dwarf_col, r, c+1) < closestDistance && dwarf.look(r,c+1) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r, c+1);
	  closestSpot.r = r;
	  closestSpot.c = c+1;
	}
	if (distance(dwarf_row, dwarf_col, r+1, c) < closestDistance && dwarf.look(r+1,c) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r+1, c);
	  closestSpot.r = r+1;
	  closestSpot.c = c;
	}
	if (distance(dwarf_row, dwarf_col, r, c-1) < closestDistance && dwarf.look(r,c-1) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r, c-1);
	  closestSpot.r = r;
	  closestSpot.c = c-1;
	}
	if (distance(dwarf_row, dwarf_col, r-1, c) < closestDistance && dwarf.look(r-1,c) == EMPTY) {
	  closestDistance = distance(dwarf_row, dwarf_col, r-1, c);
	  closestSpot.r = r-1;
	  closestSpot.c = c;
	}
      }      
    }
  }

  return closestSpot;

}

void collectPumpkin(Dwarf &dwarf, int r, int c, ostream &log) {

  if (dwarf.look(r+1, c) == PUMPKIN) {
    // tree to south
    dwarf.start_pick(SOUTH);
    return;
  }
  else if (dwarf.look(r, c+1) == PUMPKIN) {
    // tree to east
    dwarf.start_pick(EAST);
    return;
  }
  else if (dwarf.look(r-1, c) == PUMPKIN) {
    // tree to north
    dwarf.start_pick(NORTH);
    return;
  }
  else if (dwarf.look(r, c-1) == PUMPKIN) {
    // tree to west
    dwarf.start_pick(WEST);
    return;
  }  
  else {
    // Otherwise, move to closest tree/pumpkin location                                                                                              
    Loc closestSpot = closestPumpkin(dwarf, r, c);
    int rr = closestSpot.r;
    int cc = closestSpot.c;
    //log << "Walk to " << rr << " " << cc << endl;
    dwarf.start_walk(rr, cc);
    return;
  }

}


/* onAction: 
A procedure called each time an idle dwarf is choosing 
their next action.
Parameters:
    dwarf:   dwarf choosing an action
    day:     day (1+)
    hours:   number of hours in 24-hour format (0-23)
    minutes: number of minutes (0-59)
    log:     a cout-like log  */

void onAction(Dwarf &dwarf, int day, int hours, int minutes, ostream &log) {
  // Get current position of the dwarf
  int r = dwarf.row();
  int c = dwarf.col();

  //CHANGING GAME STATES
  if (GAME_STATE != 0 && GAME_STATE != 5) { //not the first day
    if (hours == 18 && minutes > 30) { //end of day
      GAME_STATE = 1;
      log << "GAME_STATE = 1" << endl;
    }
    else if (day == 2 && hours == 6 && minutes > 30) { //day 2, chop pine trees
      GAME_STATE = 3;
      log << "GAME_STATE = 3" << endl;
    }
    else if (GAME_STATE == 3 && numPine(dwarf) == 0) { //day 2, out of pines
      GAME_STATE = 4;
      log << "GAME_STATE = 4" << endl;
    }
    else if (day != 2 && hours == 6 && minutes > 30 && GAME_STATE != 5) { //day 3+, collect pumpkins
      GAME_STATE = 4;
      log << "GAME_STATE = 4" << endl;
    }
    else if (GAME_STATE == 4 && numPumpkins(dwarf) == 0) { //day 3+, collect either
      GAME_STATE = 6;
      log << "GAME_STATE = 6" << endl;
    }
    else if (hours == 6) { //break out of barricade
      GAME_STATE = 2;
      log << "GAME_STATE = 2" << endl;
    }
    else if (day == 7 && hours == 14) { //last day, build big structure
      GAME_STATE = 5;
      log << "GAME_STATE = 5" << endl;
    }
  }
  else if (GAME_STATE == 0) { //first day
    if (hours == 12) {
      GAME_STATE = 3;
      log << "GAME_STATE = 3" << endl;      
    }
    if (hours == 20) { //end of day
      GAME_STATE = 1;
      log << "GAME_STATE = 1" << endl;
    }
  }
  else if (GAME_STATE == 5) { //last day
    if (hours == 20) { //end of day
      GAME_STATE = 1;
      log << "GAME_STATE = 1" << endl;
    }
  }
  
  //GAME PLAY

  //CHOPPING TRESS
  if (GAME_STATE == 0) {
    chopTrees(dwarf, log);
  }
  //SURROUND w/ FENCES
  else if (GAME_STATE == 1) {
    barricade(dwarf, r, c, log);
  }  
  //BREAK DOWN FENCES
  else if (GAME_STATE == 2) {
    chopFenceFour(dwarf, r, c, log);
  }
  //CHOP SOME PINE TREES
  else if (GAME_STATE == 3) {
    chopPineTrees(dwarf, log);
  }
  //COLLECT PUMPKINS
  else if (GAME_STATE == 4) {
    collectPumpkin(dwarf, r, c, log);
  }
  //BUILDING FENCES
  else if (GAME_STATE == 5) {
    if (dwarf.name() == 0 && numFences(dwarf) == 0) { //first fence
      buildFirstFence(dwarf, r, c, log);
    }
    else {
      buildFences(dwarf, r, c, log);
    }
  }
  //COLLECT APPLE
  else if (GAME_STATE == 6) {
    collectApple(dwarf, r, c, log);
  }
  //DEFAULT TO CHOPPING TRESS
  else { 
    chopTrees(dwarf, log);
  }
}

