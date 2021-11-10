#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

#define WIDTH 10
#define HEIGHT 7
#define N 4

#define RED_COUNTER 1
#define YELLOW_COUNTER 2
#define EMPTY 0

#define RED_COLOUR_PAIR RED_COUNTER
#define YELLOW_COLOUR_PAIR YELLOW_COUNTER

int getIndexForGrid(int x, int y) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return -1;
  return x + y * WIDTH;
}

void printGrid ( int * grid ) {
  for (int y = HEIGHT - 1; y >= 0; y--) {
    for (int x = 0; x < WIDTH; x++) {
      if (grid[getIndexForGrid(x, y)] == EMPTY) {
        printw(" -");
      } else {
          if (grid[getIndexForGrid(x, y)] == RED_COUNTER) {
          attron(COLOR_PAIR(RED_COLOUR_PAIR));
          printw(" 0");
          attroff(COLOR_PAIR(RED_COLOUR_PAIR));
        } else if (grid[getIndexForGrid(x, y)] == YELLOW_COUNTER) {
          attron(COLOR_PAIR(YELLOW_COLOUR_PAIR));
          printw(" 0");
          attroff(COLOR_PAIR(YELLOW_COLOUR_PAIR));
        }
      }
    }
    printw("\n");
  }
}

void printGridWin ( int * grid, int winX, int winY ) {
  for (int y = HEIGHT - 1; y >= 0; y--) {
    for (int x = 0; x < WIDTH; x++) {
      if (grid[getIndexForGrid(x, y)] == EMPTY) {
        printw(" -");
      } else {
          if (grid[getIndexForGrid(x, y)] == RED_COUNTER) {
          attron(COLOR_PAIR(RED_COLOUR_PAIR));
          printw(" 0");
          attroff(COLOR_PAIR(RED_COLOUR_PAIR));
        } else if (grid[getIndexForGrid(x, y)] == YELLOW_COUNTER) {
          attron(COLOR_PAIR(YELLOW_COLOUR_PAIR));
          printw(" 0");
          attroff(COLOR_PAIR(YELLOW_COLOUR_PAIR));
        }
      }
    }
    if (y == winY) printw(" <");
    printw("\n");
  }

  for (int x = 0; x < winX; x++) {
    printw("  ");
  }
  printw(" ^\n");
}

int hasPlayerWon ( int * grid, int x, int y, int counter ) {
  int hasPlayerWonUpDown ( int * grid, int x, int y, int counter ) {
    // We know we are at the top of the column

    int n = 0;
    for (int i = y; i <= N && i >= 0 && grid[getIndexForGrid(x, i)] == counter; i--) {
      n++;
    }

    return n >= N;
  }

  int hasPlayerWonDiagonals ( int * grid, int x, int y, int counter ) {

    int hasPlayerWonLeftRightDiagonal ( int * grid, int x, int y, int counter ) {
      //Back trace to get the start
      int offset = 0;
      for (; offset <= N && x - offset >= 0 && y - offset >= 0
          && grid[getIndexForGrid(x - offset, y - offset)] == counter;
          offset++);

      //Count Counters the other way
      int n = 1;
      for(; n <= N - offset && x + n < WIDTH && y + n < HEIGHT
        && grid[getIndexForGrid(x + n, y + n)] == counter; n++);

      return n - 1 + offset >= N;
    }

    int hasPlayerWonRightLeftDiagonal ( int * grid, int x, int y, int counter ) {
      //Back trace to get the start
      int offset = 0;
      for (; offset <= N && y + offset < HEIGHT && x - offset >= 0
        && grid[getIndexForGrid(x - offset, y + offset)] == counter;
          offset++);

      //Count Counters the other way
      int n = 1;
      for(; n <= N - offset && y - n >= 0 && x + n < WIDTH
        && grid[getIndexForGrid(x + n, y - n)] == counter; n++);

      return n - 1 + offset >= N;
    }

    return hasPlayerWonLeftRightDiagonal(grid, x, y, counter)
      || hasPlayerWonRightLeftDiagonal(grid, x, y, counter);
  }

  int hasPlayerWonLeftRight ( int * grid, int x, int y, int counter ) {
    //Back trace to get the start
    int offset = 0;
    for(; x - offset >= 0 && offset <= N
      && grid[getIndexForGrid(x - offset, y)] == counter; offset++);

    //Count Counters the other way
    int n = 1;
    for(; n <= N - offset && x + n < WIDTH &&
      grid[getIndexForGrid(x + n, y)] == counter; n++);

    return n - 1 + offset >= N;
  }

  return hasPlayerWonUpDown (grid, x, y, counter)
    || hasPlayerWonDiagonals(grid, x, y, counter)
    || hasPlayerWonLeftRight (grid, x, y, counter);
}

int validMove ( int * grid, int x) {
  return grid[getIndexForGrid(x, ( HEIGHT - 1 ))] == EMPTY;
}

int getColumnInput( int * grid ) {
  // Get column to insert counter in
  int x = 0;
  int valid = 1;
  char input = 0;

  while ( !((input == 's' || input == 'S' || input == '\n') && valid) ) {
    clear();

    attron(COLOR_PAIR(RED_COLOUR_PAIR));
    for (int i = 0; i < x || x < 0; i++) {
      printw("  ");
    }

    printw(">0<");
    if(valid) {
      printw("\n");
    } else {
      printw("x\n");
    }

    attroff(COLOR_PAIR(RED_COLOUR_PAIR));

    printGrid(grid);
    refresh();

    input = getch();
    if (input == 'd' || input == 'D') {
      if (x < WIDTH - 1) x++;
    } else if (input == 'a' || input == 'A') {
      if (x > 0) x--;
    }

    valid = validMove(grid, x);
  }

  return x;
}

int yToInsertCounter ( int * grid, int x ) {
  int y = 0;
  for(; y < HEIGHT && grid[getIndexForGrid(x, y)] != EMPTY; y++);
  return y == HEIGHT ? -1 : y;
}

int react ( int * grid ) {
  for (int x = 0; x < WIDTH; x++) {
    int y = yToInsertCounter(grid, x);

    if (y != -1) {
      // Win if it can
      grid[getIndexForGrid(x, y)] = YELLOW_COUNTER;
      if (hasPlayerWon(grid, x, y, YELLOW_COUNTER)) {
        grid[getIndexForGrid(x, y)] = EMPTY;
        return x;
      }

      // Stop a win
      grid[getIndexForGrid(x, y)] = RED_COUNTER;
      if (hasPlayerWon(grid, x, y, RED_COUNTER)) {
        grid[getIndexForGrid(x, y)] = EMPTY;
        return x;
      }
      grid[getIndexForGrid(x, y)] = EMPTY;
    }
  }
  // Stop the longest chain

  // Put a counter in any place
  const int mid = WIDTH / 2;
  for (int x = mid; x < WIDTH; x++) {
    int y = yToInsertCounter(grid, x);
    if (grid[getIndexForGrid(x, HEIGHT - 1)] == EMPTY && HEIGHT - y >= N + 1) {
      return x;
    }
  }
  for (int x = 0; x < mid; x++) {
    int y = yToInsertCounter(grid, x);
    if (grid[getIndexForGrid(x, HEIGHT - 1)] == EMPTY && HEIGHT - y >= N + 1) {
      return x;
    }
  }
  return -1;
}

void initGrid( int * grid ) {
  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    grid[i] = EMPTY;
  }
}

void gameLoop () {
  int * grid = malloc(sizeof(int) * WIDTH * HEIGHT);
  int lost = 0;
  int won = 0;
  int itt = 0;
  int xAi, yAi, x, y;
  initGrid(grid);

  while (!lost && !won && itt < WIDTH * HEIGHT) {
    // Get player move

    x = getColumnInput(grid);
    y = yToInsertCounter(grid, x);
    grid[getIndexForGrid(x, y)] = RED_COUNTER;

    // Check for win
    won = hasPlayerWon(grid, x, y, RED_COUNTER);

    if (!won) {
      // React
      xAi = react(grid);
      yAi = yToInsertCounter(grid, xAi);
      grid[getIndexForGrid(xAi, yAi)] = YELLOW_COUNTER;

      // Check for loss
      lost = hasPlayerWon(grid, xAi, yAi, YELLOW_COUNTER);
    }

    itt += 2;
  }

  clear();
  if(won) {
    printGridWin(grid, x, y);
  } else {
    printGridWin(grid, xAi, yAi);
  }

  printw(lost ? "You lost.\n" : "You Won.\n");
  printw("Press any key to exit.\n");
  refresh();

  free(grid);
}

void main() {
  initscr();
  cbreak();
  noecho();

  // Colours and that jazz
  if (has_colors() == FALSE) {
    printw("Your terminal does not support color\n");
    endwin();
    exit(1);
  }

  start_color();
  init_pair(YELLOW_COLOUR_PAIR, COLOR_YELLOW, COLOR_BLACK);
  init_pair(RED_COLOUR_PAIR, COLOR_RED, COLOR_BLACK);

  gameLoop();

  getch();
  endwin();
  exit(0);
}
