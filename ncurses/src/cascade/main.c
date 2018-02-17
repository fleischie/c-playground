#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ncurses-playground.h>

int
main ()
{
	const char *msg = "...oooOOO000OOOooo...\n";
	struct timespec *ts;
	int width;
	int height;
	int width_margin;
	int w;

	initscr();
	cbreak(); // raw();
	keypad(stdscr, TRUE);
	noecho();

	// determine size of window
	width_margin = strlen(msg) + 5;
	getmaxyx(stdscr, height, width);

	// create timespec struct for delaying next iteration
	ts = malloc(sizeof(struct timespec));
	ts->tv_sec = 0;
	ts->tv_nsec = 1.5e7;

	// cascade something along the window
	w = (int) (width - width_margin) / height;
	for (int h = 0; h < 250; h++) {
		mvprintw(h % height, (int) (w * h) % width, msg);
		refresh();
		nanosleep(ts, NULL);
	}

	move (height / 2 - 5, 0);
	for (int i = 0; i < width; i++) {
		printw("#");
	}

	for (int i = 0; i < 10; i++) {
		printw("\n");
	}

	mvprintw(
			height / 2,
			(width - width_margin) / 2,
			"%s",
			"Press enter to exit..."
		);

	move (height / 2 + 5, 0);
	for (int i = 0; i < width; i++) {
		printw("#");
	}

	mvprintw(
			height - 2,
			(width - width_margin) / 2,
			"This screen has %d rows and %d cols\n",
			height,
			width
		);

	refresh();
	getch();
	endwin();

	free(ts);

	return 0;
}
