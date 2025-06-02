#include <cstdlib>
#include <curses.h>
#include <iostream>
#define GAME_HEIGHT 35
#define GAME_WIDTH 30
#define ESC 27
#define ENTER 10

using namespace std;

// enumerations
enum StartMenu { Start, Exit };
enum WallElement { TL, HZ, TR, VT, BR, BL };
enum Direction { Left, Up, Right, Down };
enum Colors { Yellow = 1, White, Blue };

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
void printFood(WINDOW *, const int = 1, Colors = White);
void printSpace(WINDOW *, const int = 1);
bool detectWall(WINDOW *, Position);
void tickPacman(WINDOW *, Pacman &);
void menuInput(short &, const int, const int);
StartMenu displayStartMenu();
void printWallElement(WINDOW *, WallElement, const int = 1);
void buildWalls(WINDOW *);
void pacmanInput(Direction &, const int);
void startGame();

// global variables
// term is short for terminal
int term_width, term_height, menu_width, menu_height;
const wchar_t pacmanStates[8] = { L'\u0254', L'u', L'c', L'n',
                                  L'\u0186', L'U', L'C', L'\u2229' };

const wchar_t WallEle[7] = { L'\u250c', L'\u2500', L'\u2510',
                             L'\u2502', L'\u2518', L'\u2514' };

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
  if (term_height < GAME_HEIGHT || term_width < GAME_WIDTH) {
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
  init_pair(2, COLOR_WHITE, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
}

void printPacman(WINDOW *win, Pacman pac) {
  wattron(win, COLOR_PAIR(Yellow));
  mvwprintw(win, pac.pos.y, pac.pos.x, "%lc",
            pacmanStates[pac.dir + 4 * pac.MouthOpen]);
  wattroff(win, COLOR_PAIR(Yellow));
}

void printFood(WINDOW *win, const int NumFood, Colors color) {
  wattron(win, COLOR_PAIR(color));
  for (int i = 0; i < NumFood; i++)
    wprintw(win, "\u2022");
  wattroff(win, COLOR_PAIR(color));
}

void printSpace(WINDOW *win, const int n) {
  for (int i = 0; i < n; i++)
    wprintw(win, " ");
}

bool detectWall(WINDOW *win, const Position p) {
  // store wide character at position p in ch_at_p
  cchar_t ch_at_p;
  mvwin_wch(win, p.y, p.x, &ch_at_p);
  // calculate the size of WallEle array
  const int SIZE = sizeof(WallEle) / sizeof(WallEle[0]);
  for (int i = 0; i < SIZE; i++)
    if (ch_at_p.chars[0] == WallEle[i])
      return true;
  return false;
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
      if (detectWall(win, p.pos))
        ++p.pos.x;
      break;
    case Up:
      --p.pos.y;
      if (detectWall(win, p.pos))
        ++p.pos.y;
      break;
    case Right:
      ++p.pos.x;
      if (detectWall(win, p.pos))
        --p.pos.x;
      break;
    case Down:
      ++p.pos.y;
      if (detectWall(win, p.pos))
        --p.pos.y;
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
  wmove(menu_scr, pacStart.y, pacStart.x + 1);
  printFood(menu_scr, foodNum);

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
      pac.dir = Left;
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

// prints wele[WALL] at current cursor position, n number of times. in case no
// n is provided, it prints only once
void printWallElement(WINDOW *win, WallElement WALL, const int n) {
  wattron(win, COLOR_PAIR(Blue));
  for (int i = 0; i < n; i++)
    wprintw(win, "%lc", WallEle[WALL]);
  wattroff(win, COLOR_PAIR(Blue));
}

// build all of the walls
void buildWalls(WINDOW *win) {
  // row 0
  printWallElement(win, TL);
  printWallElement(win, HZ, 28);
  printWallElement(win, TR);

  // row 1
  printWallElement(win, VT);
  printWallElement(win, TL);
  printWallElement(win, HZ, 12);
  printWallElement(win, TR);
  printWallElement(win, TL);
  printWallElement(win, HZ, 12);
  printWallElement(win, TR);
  printWallElement(win, VT);

  // row 2
  printWallElement(win, VT, 2);
  printFood(win, 12);
  printWallElement(win, VT, 2);
  printFood(win, 12);
  printWallElement(win, VT, 2);

  // row 3
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 3);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 3);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, VT, 2);

  // row 4
  printWallElement(win, VT, 2);
  printFood(win, 1, Yellow);
  printWallElement(win, VT);
  printSpace(win, 2);
  printWallElement(win, VT);
  printFood(win);
  printWallElement(win, VT);
  printSpace(win, 3);
  printWallElement(win, VT);
  printFood(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, VT);
  printSpace(win, 3);
  printWallElement(win, VT);
  printFood(win);
  printWallElement(win, VT);
  printSpace(win, 2);
  printWallElement(win, VT);
  printFood(win, 1, Yellow);
  printWallElement(win, VT, 2);

  // row 5
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 2);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 3);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 3);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 2);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, VT, 2);

  // row 6
  printWallElement(win, VT, 2);
  printFood(win, 26);
  printWallElement(win, VT, 2);

  // row 7
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 6);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, VT, 2);

  // row 8
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 2);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 2);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, VT, 2);

  // row 9
  printWallElement(win, VT, 2);
  printFood(win, 6);
  printWallElement(win, VT, 2);
  printFood(win, 4);
  printWallElement(win, VT, 2);
  printFood(win, 4);
  printWallElement(win, VT, 2);
  printFood(win, 6);
  printWallElement(win, VT, 2);

  // row 10
  printWallElement(win, VT);
  printWallElement(win, BL);
  printWallElement(win, HZ, 4);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, VT);
  printWallElement(win, BL);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printSpace(win);
  printWallElement(win, VT, 2);
  printSpace(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printWallElement(win, BR);
  printWallElement(win, VT);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 4);
  printWallElement(win, BR);
  printWallElement(win, VT);

  // row 11
  printWallElement(win, BL);
  printWallElement(win, HZ, 4);
  printWallElement(win, TR);
  printWallElement(win, VT);
  printFood(win);
  printWallElement(win, VT);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printWallElement(win, BR);
  printSpace(win);
  printWallElement(win, BL);
  printWallElement(win, BR);
  printSpace(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printWallElement(win, VT);
  printFood(win);
  printWallElement(win, VT);
  printWallElement(win, TL);
  printWallElement(win, HZ, 4);
  printWallElement(win, BR);

  // row 12
  printSpace(win, 5);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, VT, 2);
  printSpace(win, 10);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, VT, 2);
  printSpace(win, 5);

  // row 13
  printWallElement(win, HZ, 5);
  printWallElement(win, BR);
  printWallElement(win, VT);
  printFood(win);
  printWallElement(win, VT, 2);
  printSpace(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printSpace(win, 2);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printSpace(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, VT);
  printWallElement(win, BL);
  printWallElement(win, HZ, 5);

  // row 14
  printWallElement(win, HZ, 6);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, BR);
  printSpace(win);
  printWallElement(win, VT);
  printSpace(win, 6);
  printWallElement(win, VT);
  printSpace(win);
  printWallElement(win, BL);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 6);

  // row 15
  printSpace(win, 7);
  printFood(win);
  printSpace(win, 3);
  printWallElement(win, VT);
  printSpace(win, 6);
  printWallElement(win, VT);
  printSpace(win, 3);
  printFood(win);
  printSpace(win, 7);

  // row 16
  printWallElement(win, HZ, 6);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, TR);
  printSpace(win);
  printWallElement(win, VT);
  printSpace(win, 6);
  printWallElement(win, VT);
  printSpace(win);
  printWallElement(win, TL);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 6);

  // row 17
  printWallElement(win, HZ, 5);
  printWallElement(win, TR);
  printWallElement(win, VT);
  printFood(win);
  printWallElement(win, VT, 2);
  printSpace(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 6);
  printWallElement(win, BR);
  printSpace(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, VT);
  printWallElement(win, TL);
  printWallElement(win, HZ, 5);

  // row 18
  printSpace(win, 5);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, VT, 2);
  printSpace(win, 10);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, VT, 2);
  printSpace(win, 5);

  // row 19
  printWallElement(win, TL);
  printWallElement(win, HZ, 4);
  printWallElement(win, BR);
  printWallElement(win, VT);
  printFood(win);
  printWallElement(win, VT, 2);
  printSpace(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 6);
  printWallElement(win, TR);
  printSpace(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, VT);
  printWallElement(win, BL);
  printWallElement(win, HZ, 4);
  printWallElement(win, TR);

  // row 20
  printWallElement(win, VT);
  printWallElement(win, TL);
  printWallElement(win, HZ, 4);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, BR);
  printSpace(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printWallElement(win, BR);
  printSpace(win);
  printWallElement(win, BL);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 4);
  printWallElement(win, TR);
  printWallElement(win, VT);

  // row 21
  printWallElement(win, VT, 2);
  printFood(win, 12);
  printWallElement(win, VT, 2);
  printFood(win, 12);
  printWallElement(win, VT, 2);

  // row 22
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 3);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 3);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, VT, 2);

  // row 23
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ);
  printWallElement(win, TR);
  printWallElement(win, VT);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 3);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 3);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, VT);
  printWallElement(win, TL);
  printWallElement(win, HZ);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, VT, 2);

  // row 24
  printWallElement(win, VT, 2);
  printFood(win, 1, Yellow);
  printFood(win, 2);
  printWallElement(win, VT, 2);
  printFood(win, 16);
  printWallElement(win, VT, 2);
  printFood(win, 2);
  printFood(win, 1, Yellow);
  printWallElement(win, VT, 2);

  // row 25
  printWallElement(win, VT);
  printWallElement(win, BL);
  printWallElement(win, HZ);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 6);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ);
  printWallElement(win, BR);
  printWallElement(win, VT);

  // row 26
  printWallElement(win, VT);
  printWallElement(win, TL);
  printWallElement(win, HZ);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ);
  printWallElement(win, TR);
  printWallElement(win, VT);

  // row 27
  printWallElement(win, VT, 2);
  printFood(win, 6);
  printWallElement(win, VT, 2);
  printFood(win, 4);
  printWallElement(win, VT, 2);
  printFood(win, 4);
  printWallElement(win, VT, 2);
  printFood(win, 6);
  printWallElement(win, VT, 2);

  // row 28
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 4);
  printWallElement(win, BR);
  printWallElement(win, BL);
  printWallElement(win, HZ, 2);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, TL);
  printWallElement(win, HZ, 2);
  printWallElement(win, BR);
  printWallElement(win, BL);
  printWallElement(win, HZ, 4);
  printWallElement(win, TR);
  printFood(win);
  printWallElement(win, VT, 2);

  // row 29
  printWallElement(win, VT, 2);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 8);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, BL);
  printWallElement(win, HZ, 8);
  printWallElement(win, BR);
  printFood(win);
  printWallElement(win, VT, 2);

  // row 30
  printWallElement(win, VT, 2);
  printFood(win, 26);
  printWallElement(win, VT, 2);

  // row 31
  printWallElement(win, VT);
  printWallElement(win, BL);
  printWallElement(win, HZ, 26);
  printWallElement(win, BR);
  printWallElement(win, VT);

  // row 32
  printWallElement(win, BL);
  printWallElement(win, HZ, 28);
  printWallElement(win, BR);
}

void startGame() {

  const int start_x = term_width / 2 - GAME_WIDTH / 2;
  const int start_y = term_height / 2 - GAME_HEIGHT / 2;
  WINDOW *game_scr = newwin(GAME_HEIGHT, GAME_WIDTH, start_y, start_x);
  keypad(game_scr, true);
  refresh();

  buildWalls(game_scr);
  const Position pacmanStart = { 3, 2 };
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
