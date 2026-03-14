#include <ncurses.h>
#ifndef BUILDMAP_H_INCLUDED
#define BUILDMAP_H_INCLUDED

enum WallElement { TL, HZ, TR, VT, BR, BL };
enum Colors { Yellow = 1, White, Blue, Cyan, Red, Green, Magenta };
const wchar_t WallEle[7] = { L'\u250c', L'\u2500', L'\u2510',
                             L'\u2502', L'\u2518', L'\u2514' };

void printSpace(WINDOW *win, const int n = 1) {
  for (int i = 0; i < n; i++)
    wprintw(win, " ");
}

void printFood(WINDOW *win, const int NumFood = 1, Colors color = White) {
  wattron(win, COLOR_PAIR(color));
  for (int i = 0; i < NumFood; i++)
    wprintw(win, "\u2022");
  wattroff(win, COLOR_PAIR(color));
}

/* prints wele[WALL] at current cursor position, n number of times. in case no
 n is provided, it prints only once */
void printWallElement(WINDOW *win, WallElement WALL, const int n = 1) {
  wattron(win, COLOR_PAIR(Blue)); // 3 is blue' color pair
  for (int i = 0; i < n; i++)
    wprintw(win, "%lc", WallEle[WALL]);
  wattroff(win, COLOR_PAIR(Blue));
}

// build all of the walls
void buildMap(WINDOW *win) {
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
  printWallElement(win, HZ, 2);
  printSpace(win, 2);
  printWallElement(win, HZ, 2);
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
#endif
