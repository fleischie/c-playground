#include <curses.h>
#include <unistd.h>

#include <ncurses-playground.h>

int
main ()
{
	int ch;

	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();

	printw("Type characters to see in bold!\n");
	ch = getch();

	if (ch == KEY_F(1))
	{
		printw("Character is F1");
	}
	else
	{
		printw("The pressed key is ");
		attron(A_BOLD);
		printw("%c (%d)", ch, ch);
		attroff(A_BOLD);
	}

	getch();

	printw("\nWill exit in 3 seconds...");
	refresh();

	sleep(3);
	endwin();

	return 0;
}
