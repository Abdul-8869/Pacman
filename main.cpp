#include "buildMap.h"
#include <cstdlib>
#include <ctime>
#include <curses.h>
#include <iostream>
#include <ncurses.h>
#define GAME_HEIGHT 35
#define GAME_WIDTH 30
#define MAX_FOOD 246
#define TIMEOUT 20
#define ESC 27
#define ENTER 10

using namespace std;

// enumerations
enum StartMenu { Start, Exit };
enum Direction { Left, Up, Right, Down };

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

  // constructors
  Pacman() : MouthOpen(false), dir(Right), speed(10) {}
  Pacman(Position p) : pos(p), MouthOpen(false), dir(Right), speed(10) {}
};

struct Enemy {
  static const char shape = '@';
  Position pos;
  Direction dir;
  unsigned int speed;
  Colors color;
  bool isEaten;  // turns to true when pacman eats enemy
  bool hasEaten; // turns to true when enemy eats food

  // constructors
  Enemy() : speed(10), isEaten(false), hasEaten(false) {}

  Enemy(Position p) : pos(p), speed(10), isEaten(false), hasEaten(false) {}
};

// function prototypes
void initCurses();
void initColors();
void printPacman(WINDOW *, Pacman);
void printEnemy(WINDOW *, Enemy);
void printEnemies(WINDOW *, Enemy[], const int = 4);
bool detectWall(WINDOW *, Position);
bool detectFood(WINDOW *, const Position);
bool detectEnemies(WINDOW *, const Position);
void incScore(WINDOW *, Position);
void printScore(WINDOW *);
void printLives(WINDOW *, const unsigned int);
void updatePosition(const Direction, Position &);
void tickPacman(WINDOW *, Pacman &);
void tickEnemy(WINDOW *, Enemy &);
void menuInput(short &, const int, const int);
StartMenu displayStartMenu();
void pacmanInput(Direction &, const int);
void initPos(WINDOW *, Position &, Position[], bool);
void startGame();

// global variables
// term is short for terminal
int term_width, term_height, menu_width, menu_height;
bool gameStarted = false;
const wchar_t pacmanStates[8] = { L'\u0254', L'u', L'c', L'n',
                                  L'\u0186', L'U', L'C', L'\u2229' };

unsigned short tick = 0;
unsigned int score = 0;
bool pacmanDead = false;
unsigned short lives = 3;
unsigned int foodEatenCount = 0;

int main() {
  initCurses();
  initColors();
  srand(time(0));
  if (displayStartMenu() == Exit) {
    endwin();
    return 0;
  }

  gameStarted = true;
  startGame();
  endwin();
  cout << "Game Over" << endl;
  cout << "Score: " << score << endl;
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

  menu_width = term_width * 2 / 3;
  menu_height = term_height / 2;
}

void initColors() {

  // check if colors are supported
  if (!has_colors()) {
    endwin();
    cout << "Error: Terminal does not support colors.\n";
    exit(3);
  }

  // start colors
  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  init_pair(2, COLOR_WHITE, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);
  init_pair(5, COLOR_RED, COLOR_BLACK);
  init_pair(6, COLOR_GREEN, COLOR_BLACK);
  init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
}

void printPacman(WINDOW *win, Pacman p) {
  wattron(win, COLOR_PAIR(Yellow));
  mvwprintw(win, p.pos.y, p.pos.x, "%lc",
            pacmanStates[p.dir + 4 * p.MouthOpen]);
  wattroff(win, COLOR_PAIR(Yellow));
}

void printEnemy(WINDOW *win, Enemy e) {
  wattron(win, COLOR_PAIR(e.color));
  mvwprintw(win, e.pos.y, e.pos.x, "%c", e.shape);
  wattroff(win, COLOR_PAIR(e.color));
}

void printEnemies(WINDOW *win, Enemy enem[], const int SIZE) {
  for (int i = 0; i < SIZE; i++) {
    printEnemy(win, enem[i]);
  }
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

bool detectFood(WINDOW *win, const Position p) {
  cchar_t ch_at_p;
  mvwin_wch(win, p.y, p.x, &ch_at_p);
  if (ch_at_p.chars[0] == L'\u2022')
    return true;
  else
    return false;
}

bool detectEnemy(WINDOW *win, const Position p) {
  cchar_t ch_at_p;
  mvwin_wch(win, p.y, p.x, &ch_at_p);
  if (ch_at_p.chars[0] == '@')
    return true;
  else
    return false;
}

bool detectPacman(WINDOW *win, const Position p) {
  // store wide character at position p in ch_at_p
  cchar_t ch_at_p;
  mvwin_wch(win, p.y, p.x, &ch_at_p);
  // calculate the number of elements in pacmanStates[]
  const int SIZE = sizeof(pacmanStates) / sizeof(pacmanStates[0]);
  for (int i = 0; i < SIZE; i++)
    if (ch_at_p.chars[0] == pacmanStates[i])
      return true;
  return false;
}

void incScore(WINDOW *win, Position p) {
  if (detectFood(win, p)) {
    score += 10;
    foodEatenCount++;
  }
}
void printScore(WINDOW *win) {
  wmove(win, GAME_HEIGHT - 1, 0);
  wprintw(win, "Score: %d", score);
}

void printLives(WINDOW *win, const unsigned int lives) {
  wmove(win, GAME_HEIGHT - 1, GAME_WIDTH - 9);
  wprintw(win, "Lives: %d", lives);
}

void updatePosition(const Direction dir, Position &p) {
  switch (dir) {
  case Left:
    --p.x;
    break;
  case Up:
    --p.y;
    break;
  case Right:
    ++p.x;
    break;
  case Down:
    ++p.y;
  }
}

void tickPacman(WINDOW *win, Pacman &p) {
  // invert mouth every 5 ticks
  if (tick % 5 == 0)
    p.MouthOpen = !p.MouthOpen;

  // update position of pacman every 10 ticks
  if (tick % p.speed == 0) {

    // store old position
    Position oldPos = p.pos;

    // remove pacman from previous position
    mvwprintw(win, p.pos.y, p.pos.x, " ");

    // update position and revert if there is a wall at the new position
    updatePosition(p.dir, p.pos);
    if (detectWall(win, p.pos))
      p.pos = oldPos;
    else if (detectEnemy(win, p.pos)) {
      pacmanDead = true;
      napms(1000);
      lives--;
      return;
    }

    incScore(win, p.pos);
  }

  // wrap pacman in the tunnel
  if (p.pos.x < 0 && p.pos.y == 15) {
    mvwprintw(win, 15, 0, " ");
    p.pos.x = GAME_WIDTH - 1;
  } else if (p.pos.x > GAME_WIDTH - 1 && p.pos.y == 15) {
    mvwprintw(win, 15, GAME_WIDTH - 1, " ");
    p.pos.x = 0;
  }

  // draw pacman at its current position and refresh screen
  printPacman(win, p);
}
void tickEnemy(WINDOW *win, Enemy &e) {

  // update enemy position based on their speed
  if (tick % e.speed == 0) {

    // print food at current position if hasEaten is true, otherwise print space
    wmove(win, e.pos.y, e.pos.x);
    if (e.hasEaten) {
      printFood(win);
      e.hasEaten = false;
    } else
      wprintw(win, " ");

    const Position oldPos = e.pos;
    updatePosition(e.dir, e.pos);
    if (detectWall(win, e.pos) || detectEnemy(win, e.pos)) {
      e.dir = static_cast<Direction>(rand() % 4);
      e.pos = oldPos;
    }

    // if there is a food at the new position, then set hasEaten to true.
    if (detectFood(win, e.pos))
      e.hasEaten = true;
    else if (detectPacman(win, e.pos)) {
      pacmanDead = true;
      napms(1000);
      lives--;
      return;
    }

    // handle tunnel/wrap-around
    if (e.pos.x < 0 && e.pos.y == 15) {
      mvwprintw(win, 15, 0, " ");
      e.pos.x = GAME_WIDTH - 1;
    } else if (e.pos.x > GAME_WIDTH - 1 && e.pos.y == 15) {
      mvwprintw(win, 15, GAME_WIDTH - 1, " ");
      e.pos.x = 0;
    }

    // print enemy
    printEnemy(win, e);
  }
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

  // set wgetch timeout to TIMEOUT milliseconds
  wtimeout(menu_scr, TIMEOUT);
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
    tick++;
    tickPacman(menu_scr, pac);

    // start pacman at initial position again after eating all food
    if (pac.pos.x > pacStart.x + foodNum)
      pac.dir = Left;
    else if (pac.pos.x == pacStart.x)
      pac.dir = Right;

    // get input
    keyPressed = wgetch(menu_scr);

    // handel input
    menuInput(highlight, keyPressed, SIZE);
  } while (keyPressed != ENTER);

  // exit loop if Enter key is pressed
  napms(150);       // pause for 150 milli seconds
  delwin(menu_scr); // free memory of menu's window
  clear();

  return static_cast<StartMenu>(highlight);
}

void pacmanInput(Direction &dir, const int key) {
  switch (key) {
  case KEY_LEFT:
  case static_cast<int>('a'):
  case static_cast<int>('A'):
    dir = Left;
    break;
  case KEY_UP:
  case static_cast<int>('w'):
  case static_cast<int>('W'):
    dir = Up;
    break;
  case KEY_RIGHT:
  case static_cast<int>('d'):
  case static_cast<int>('D'):
    dir = Right;
    break;
  case KEY_DOWN:
  case static_cast<int>('s'):
  case static_cast<int>('S'):
    dir = Down;
    break;
  }
}

void initPos(WINDOW *win, Pacman &p, Enemy e[4], const Position initialPos[5],
             bool eraseFromPrevPosFlag) {
  if (eraseFromPrevPosFlag)
    mvwprintw(win, p.pos.y, p.pos.x, " ");
  p.pos = initialPos[0];
  for (int i = 0; i < 4; i++) {

    if (eraseFromPrevPosFlag)
      mvwprintw(win, e[i].pos.y, e[i].pos.x, " ");

    e[i].pos = initialPos[i + 1];
    if (i < 2)
      e[i].dir = Up;
    else
      e[i].dir = Down;

    e[i].color =
        static_cast<Colors>(i + 4); // Assign different colors from index 4 to 7
  }
}

void startGame() {

  const int start_x = term_width / 2 - GAME_WIDTH / 2;
  const int start_y = term_height / 2 - GAME_HEIGHT / 2;
  WINDOW *game_scr = newwin(GAME_HEIGHT, GAME_WIDTH, start_y, start_x);
  keypad(game_scr, true);
  refresh();

  buildMap(game_scr);
  const Position initialPos[5] = {
    { 15, 24 }, // pacman
    { 14, 14 }, // Enemy 1
    { 15, 14 }, // Enemy 2
    { 14, 16 }, // Enemy 3
    { 15, 16 }  // Enemy 4
  };

  Pacman pac;
  Enemy enemies[4];
  initPos(game_scr, pac, enemies, initialPos, false);
  printPacman(game_scr, pac);
  printEnemies(game_scr, enemies);

  tick = 0;
  score = 0;
  wtimeout(game_scr, TIMEOUT);
  int key;
  do {

    // increment tick
    tick++;

    // print and update pacman every tick
    tickPacman(game_scr, pac);

    // print score, lives
    printScore(game_scr);
    printLives(game_scr, lives);

    key = wgetch(game_scr);
    pacmanInput(pac.dir, key);

    for (int i = 0; i < 4; i++) {
      tickEnemy(game_scr, enemies[i]);
    }
    // restart map if pacman is dead
    if (pacmanDead) {
      napms(1000);
      initPos(game_scr, pac, enemies, initialPos, true);
      pacmanDead = false;
    }

    // restart if all of the food is eaten
    else if (foodEatenCount == MAX_FOOD) {
      napms(1000);
      werase(game_scr);
      buildMap(game_scr);
      initPos(game_scr, pac, enemies, initialPos, true);
      foodEatenCount = 0;
      enemies[0].speed--; // increase speed
      wrefresh(game_scr);
    }

    // reset tick back to 0 to avoid variable overflow
    if (tick == 60)
      tick = 0;
  } while (lives > 0 && key != ESC);
  napms(150);
  delwin(game_scr);
  clear();
}
