#include <curses.h>

int main() {
  initscr();
  move(20, 20);
  printw("Hello");
  refresh();
  move(0, 0);
  int c = getch();
  getch();
  endwin();
  return 0;
}
