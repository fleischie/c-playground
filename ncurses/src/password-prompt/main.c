#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses-playground.h>
#include "../common.h"

#define PASSWORD_LENGTH 128

void
render ()
{
	int height;
	int width;
	char *output;

	// get dimension of current window
	getmaxyx(stdscr, height, width);

	// fill output with arbitrary characters
	output = malloc(width * sizeof (char));
	for (int i = 0; i < width; i++) {
		output[i] = rand() > RAND_MAX / 2 ? '#' : ' ';
	}

	mvprintw(height / 2 + 1, 0, "%s", output);
	refresh();

	free(output);
}

int
main ()
{
	bool is_input;
	bool is_end;
	int height;
	int ch;
	int length;
	char *input;

	initscr();
	cbreak();
	keypad(stdscr, true);
	noecho();
	curs_set(CURSOR_INVISIBLE);

	// notify user of intent
	printw("Enter password:");

	// start input loop
	is_input = true;
	is_end = false;
	length = 0;
	input = malloc(PASSWORD_LENGTH * sizeof (char));
	while (is_input && !is_end && (ch = getch()) != ERR)
	{
		switch (ch)
		{
			// input was ESC -> discard input
			case 27:
				is_input = false;
				break;
			// input was Enter, Ctrl-D -> finalize input
			case 4:
			case 10:
				is_end = true;
				break;
			// input was Ctrl-U -> clear password
			case 21:
				length = 0;
				memset(input, '\0', PASSWORD_LENGTH);
				render();
				break;
			// input was backspace
			// -> clear last char in password
			case 127:
				input[--length] = '\0';
				render();
				break;
			// on ascii char -> append character to password
			default:
				input[length++] = ch;
				render();
				break;
		}

		// clamp length above 0
		if (length < 0) {
			length = 0;
		}
		// if there is no more space left in password
		// -> break
		if (length >= PASSWORD_LENGTH - 1) {
			break;
		}
	}

	// terminate last character
	input[PASSWORD_LENGTH - 1] = '\0';

	// show cursor again
	curs_set(CURSOR_VISIBLE);

	// get dimension of current window
	height = getmaxy(stdscr);
	mvprintw(height - 1, 0, "Your password is: %s\n", input);

	getch();
	refresh();
	endwin();

	return 0;
}
