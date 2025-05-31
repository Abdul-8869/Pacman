#include <cstdlib>
#include <curses.h>
#include <iostream>
#define MIN_TERM_HEIGHT 30
#define MIN_TERM_WIDTH 27
#define ESC 27
#define ENTER 10

using namespace std;

// enumerations
enum StartMenu { Start, Exit };
enum PauseMenu { Resume, Quit };
enum Direction { Left, Up, Right, Down };
enum Colors { Yellow = 1, Green };

// structures
struct Position {
  int x;
  int y;
};

struct Pacman {
  Position pos;
  Direction dir;
  const unsigned int speed;
  bool MouthOpen;
  unsigned short tick;

  // constructors
  Pacman() : MouthOpen(false), dir(Right), speed(4), tick(0) {}
  Pacman(Position p)
      : pos(p), MouthOpen(false), dir(Right), speed(4), tick(0) {}
};

// function prototypes
void initCurses();
void initColors();
void printPacman(WINDOW *, Pacman);
void printFood(WINDOW *, const Position, const int);
void tickPacman(WINDOW *, Pacman &);
void menuInput(short &highlight, const int key, const int menuItems);
StartMenu displayStartMenu();
void pacmanInput(Direction &d, const int key);
void startGame();

// global variables
// term is short for terminal
int term_width, term_height, menu_width, menu_height;
const string pacmanStates[8] = { "\u0254", "u", "c", "n",
                                 "\u0186", "U", "C", "\u2229" };

int main() {
  initCurses();
  initColors();
  if (displayStartMenu() == Exit) {
    endwin();
    return 0;
  }

  startGame();
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

  initscr();       // initialize screen
  set_escdelay(0); // no delay when ESC is pressed
  curs_set(0);     // hide cursor
  cbreak();        // enables Ctrl + C to exit program
  noecho();        // do not echo on getch() call

  // get width and height of terminal
  getmaxyx(stdscr, term_height, term_width);

  // check if terminal height or width is less than 29 and 23 respectively
  if (term_height < MIN_TERM_HEIGHT || term_width < MIN_TERM_WIDTH) {
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

void tickPacman(WINDOW *win, Pacman &p) {
  // invert mouth every 5 ticks
  if (++p.tick % 5 == 0)
    p.MouthOpen = !p.MouthOpen;

  // update position of pacman every 10 ticks
  if (p.tick % 10 == 0) {

    // remove pacman from previous position
    mvwprintw(win, p.pos.y, p.pos.x, " ");

    // update position
    switch (p.dir) {
    case Left:
      --p.pos.x;
      break;
    case Up:
      --p.pos.y;
      break;
    case Right:
      ++p.pos.x;
      break;
    case Down:
      ++p.pos.y;
      break;
    }
  }

  // reset tick back to 0 to avoid variable overflow
  if (p.tick == 60)
    p.tick = 0;

  // draw pacman at its current position and refresh screen
  printPacman(win, p);
  wrefresh(win);
}

void menuInput(short &highlight, const int key, const int menuItems) {

  switch (key) {
  case KEY_UP:
    highlight--;
    if (highlight < 0)
      highlight = 0;
    break;
  case KEY_DOWN:
    highlight++;
    if (highlight > menuItems - 1)
      highlight = menuItems - 1;
    break;
  }
}

// returns the selected choice in the menu
StartMenu displayStartMenu() {

  /* create a new window for menu of height one third of the height of the
     terminal, and width two third of the width of the terminal. the top left
     corner of the window will be at position (width / 6, height / 3). */
  const int start_x = term_width / 2 - menu_width / 2;
  const int start_y = term_height / 2 - menu_height / 2;
  WINDOW *menu_scr = newwin(menu_height, menu_width, start_y, start_x);
  keypad(menu_scr, true);
  refresh(); // update standard screen

  string choices[] = { "Start", "Exit" };
  const short SIZE = sizeof(choices) / sizeof(choices[0]);
  const short centerOffset_x = 3;
  // initial position of pacman
  const Position pacStart = { menu_width / 2 - 7 - centerOffset_x,
                              menu_height - 2 };
  const int foodNum = 19; // number of food
  short highlight = 0;

  // initialize pacman
  Pacman pac(pacStart);

  // set wgetch timeout based on speed in characters per second
  wtimeout(menu_scr, 1000 / (10 * pac.speed));
  box(menu_scr, 0, 0); // draw border

  // write Pacman in yellow at the top center of menu
  wattron(menu_scr, COLOR_PAIR(Yellow));
  mvwprintw(menu_scr, 1, menu_width / 2 - centerOffset_x, "Pacman");
  wattroff(menu_scr, COLOR_PAIR(Yellow));

  // print food for animation
  printFood(menu_scr, { pacStart.x + 1, pacStart.y }, foodNum);

  int keyPressed;
  do {

    // draw menu
    for (int i = 0; i < SIZE; i++) {
      if (i == highlight) {
        wattron(menu_scr, COLOR_PAIR(Yellow));
        wattron(menu_scr, A_REVERSE); // reverse colors in color pair
      }
      mvwprintw(menu_scr, menu_height / 2 + i, menu_width / 2 - centerOffset_x,
                "%s", choices[i].c_str());
      wattroff(menu_scr, COLOR_PAIR(Yellow));
      wattroff(menu_scr, A_REVERSE);
    }

    // update pacman every tick
    tickPacman(menu_scr, pac);

    // start pacman at initial position again after eating all food
    if (pac.pos.x > pacStart.x + foodNum) {
      mvwprintw(menu_scr, pac.pos.y, pac.pos.x, " ");
      pac.pos.x = pacStart.x;
    }

    // get input
    keyPressed = wgetch(menu_scr);

    // handel input
    menuInput(highlight, keyPressed, SIZE);
  } while (keyPressed != ENTER);

  // exit loop if Enter key is pressed
  napms(150);       // pause for 150 milli seconds
  delwin(menu_scr); // free memory of menu's window
  clear();

  return StartMenu(highlight);
}

void pacmanInput(Direction &d, const int key) {
  switch (key) {
  case KEY_LEFT:
  case static_cast<int>('a'):
  case static_cast<int>('A'):
    d = Left;
    break;
  case KEY_UP:
  case static_cast<int>('w'):
  case static_cast<int>('W'):
    d = Up;
    break;
  case KEY_RIGHT:
  case static_cast<int>('d'):
  case static_cast<int>('D'):
    d = Right;
    break;
  case KEY_DOWN:
  case static_cast<int>('s'):
  case static_cast<int>('S'):
    d = Down;
    break;
  }
}

void startGame() {

  const int start_x = term_width / 2 - MIN_TERM_WIDTH / 2;
  const int start_y = term_height / 2 - MIN_TERM_HEIGHT / 2;
  WINDOW *game_scr = newwin(MIN_TERM_HEIGHT, MIN_TERM_WIDTH, start_y, start_x);
  keypad(game_scr, true);
  refresh();

  box(game_scr, 0, 0);
  const Position pacmanStart = { 5, 1 };
  Pacman pac(pacmanStart);

  wtimeout(game_scr, 1000 / (10 * pac.speed));
  int key;
  do {
    // print and update pacman every tick
    tickPacman(game_scr, pac);

    key = wgetch(game_scr);
    pacmanInput(pac.dir, key);

  } while (key != ESC);
  napms(150);
  delwin(game_scr);
  clear();
}
