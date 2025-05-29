#include <cstdlib>
#include <curses.h>
#include <iostream>

using namespace std;

enum StartMenu { Start, Exit };
enum PauseMenu { Resume, Quit };
enum Direction { Left, Up, Right, Down };
enum Colors { Yellow = 1, Green };

struct Position {
  int x;
  int y;
};

struct Pacman {
  Position pos;
  Direction dir;
  bool MouthOpen;
  unsigned int speed;
  unsigned short tick;
};

// function prototypes
void initCurses();
void initColors();
void printPacman(WINDOW *, Pacman);
void printFood(WINDOW *, const Position, const int);
void tickPacman(WINDOW *, Pacman &, const Position, const int);
StartMenu displayStartMenu();
;

// global variables
// term is short for terminal
int term_width, term_height, menu_width, menu_height;
const string pacmanStates[8] = { "\u0254", "u",      "c", "n",
                                 "\u0186", "\u222A", "C", "\u2229" };

int main() {
  initCurses();
  initColors();
  if (displayStartMenu() == Exit) {
    endwin();
    return 0;
  }

  int c = getch();
  endwin();

  return 0;
}

// function definitions
void initCurses() {

  // set locale
  string locale = setlocale(LC_ALL, "");
  if (locale == "C") {
    cout << "Error: Enviornment variable LANG not set. Use UTF-8 character "
            "encoding to enable Unicode character support."
         << endl;
    exit(1);
  }

  initscr();   // initialize screen
  curs_set(0); // hide cursor
  cbreak();    // enables Ctrl + C to exit program
  noecho();    // do not echo on getch() call

  // get width and height of terminal
  getmaxyx(stdscr, term_height, term_width);

  // check if terminal height or width is less than 29 and 23 respectively
  if (term_height < 29 || term_width < 23) {
    endwin();
    cout << "Error: Terminal size must be at least 29x23. Increase the size "
            "and try again."
         << endl;
    exit(2);
  }

  // check if colors are supported
  if (!has_colors()) {
    endwin();
    cout << "Error: Terminal does not support colors.\n";
    exit(3);
  }

  menu_width = term_width * 2 / 3;
  menu_height = term_height / 2;
}

void initColors() {

  // start colors
  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
}

void printPacman(WINDOW *win, Pacman pac) {
  wattron(win, COLOR_PAIR(Yellow));
  mvwprintw(win, pac.pos.y, pac.pos.x, "%s",
            pacmanStates[pac.dir + 4 * pac.MouthOpen].c_str());
  wattroff(win, COLOR_PAIR(Yellow));
}

void printFood(WINDOW *win, const Position pos, const int NumFood) {
  wmove(win, pos.y, pos.x);
  wattron(win, COLOR_PAIR(Green));
  for (int i = 0; i < NumFood; i++)
    wprintw(win, "\u2022");
  wattroff(win, COLOR_PAIR(Green));
}

void tickPacman(WINDOW *win, Pacman &p, Position pacStart, int foodNum) {
  // draw pacman at its current position and refresh screen
  printPacman(win, p);
  wrefresh(win);

  // invert mouth every 5 frames
  if (++p.tick % 5 == 0)
    p.MouthOpen = !p.MouthOpen;

  // increase position of pacman every 10 frames
  if (p.tick % 10 == 0) {
    // remove pacman from previous position
    mvwprintw(win, p.pos.y, p.pos.x, " ");

    // update position
    if (p.pos.x <= pacStart.x + foodNum)
      p.pos.x++;
    else
      p.pos.x = pacStart.x;
  }

  // reset tick variable back to 0
  if (p.tick == 59)
    p.tick = 0;
}

// returns the selected choice in the menu
StartMenu displayStartMenu() {

  /* create a new window for menu of height one third of the height of the
     terminal, and width two third of the width of the terminal. the top left
     corner of the window will be at position (width / 6, height / 3). */
  WINDOW *menu_scr =
      newwin(menu_height, menu_width, term_height / 4, term_width / 6);
  keypad(menu_scr, true);
  refresh(); // update standard screen

  string choices[] = { "Start", "Exit" };
  const int SIZE = sizeof(choices) / sizeof(choices[0]);
  const int centerOffset_x = 3, scrOffset_beg = 1, scrOffset_end = 2;
  // initial position of pacman
  const Position pacStart = { menu_width / 8 + scrOffset_beg,
                              menu_height - scrOffset_end };
  const int foodNum = menu_width * 6 / 8 - scrOffset_end; // number of food
  short highlight = 0;

  // initialize pacman
  Pacman pac;
  pac.pos = pacStart;
  pac.dir = Right;
  pac.MouthOpen = false;
  pac.speed = 4; // 4 characters per second
  pac.tick = 0;

  wtimeout(menu_scr,
           1000 / (10 * pac.speed)); // set wgetch timeout to 25 milliseconds

  box(menu_scr, 0, 0);
  wattron(menu_scr, COLOR_PAIR(Yellow));
  mvwprintw(menu_scr, 1, menu_width / 2 - centerOffset_x, "Pacman");
  wattroff(menu_scr, COLOR_PAIR(Yellow));
  printFood(menu_scr, { pac.pos.x + 1, pac.pos.y }, foodNum);

  int keyPressed;
  do {
    // draw menu
    for (int i = 0; i < SIZE; i++) {
      if (i == highlight) {
        wattron(menu_scr, COLOR_PAIR(Yellow));
        wattron(menu_scr, A_REVERSE);
      }
      mvwprintw(menu_scr, menu_height / 2 + i, menu_width / 2 - centerOffset_x,
                "%s", choices[i].c_str());
      wattroff(menu_scr, COLOR_PAIR(Yellow));
      wattroff(menu_scr, A_REVERSE);
    }

    // change MouthOpen states and removes pacman from current position
    tickPacman(menu_scr, pac, pacStart, foodNum);

    // get input
    keyPressed = wgetch(menu_scr);

    switch (keyPressed) {
    case KEY_UP:
      highlight--;
      if (highlight < 0)
        highlight = 0;
      break;
    case KEY_DOWN:
      highlight++;
      if (highlight > SIZE - 1)
        highlight = SIZE - 1;
      break;
    }
  } while (keyPressed != '\n');

  // exit loop if Enter key is pressed

  napms(150);       // pause for 150 milli seconds
  delwin(menu_scr); // free memory of menu's window
  clear();

  return StartMenu(highlight);
}
