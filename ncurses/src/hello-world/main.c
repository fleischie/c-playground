#include <curses.h>

#include <ncurses-playground.h>

int
main ()
{
	initscr();
	printw("hello, world!\n");
	refresh();
	getch();
	endwin();

	return 0;
}
