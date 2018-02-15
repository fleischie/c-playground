#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses-playground.h>

#define CURSOR_INVISIBLE 0
#define CURSOR_VISIBLE 1

#define MENU_WIDTH 256

struct Attribs {
	int standout;
	int underline;
	int reverse;
	int blink;
	int dim;
	int bold;
	int protect;
	int invis;
	int altcharset;
	int chartext;
};

void
initialize_attrs (struct Attribs *attrs)
{
	attrs->standout = 0;
	attrs->underline = 0;
	attrs->reverse = 0;
	attrs->blink = 0;
	attrs->dim = 0;
	attrs->bold = 0;
	attrs->protect = 0;
	attrs->invis = 0;
	attrs->altcharset = 0;
	attrs->chartext = 0;
}

void
manipulate_attrs (char ch, struct Attribs *attrs)
{
	switch (ch)
	{
		case 'n':
		case 'N':
			initialize_attrs(attrs);
			break;
		case 's':
		case 'S':
			attrs->standout = attrs->standout == 0
				? A_STANDOUT
				: 0;
			break;
		case 'u':
		case 'U':
			attrs->underline = attrs->underline == 0
				? A_UNDERLINE
				: 0;
			break;
		case 'r':
		case 'R':
			attrs->reverse = attrs->reverse == 0
				? A_REVERSE
				: 0;
			break;
		case 'l':
		case 'L':
			attrs->blink = attrs->blink == 0
				? A_BLINK
				: 0;
			break;
		case 'd':
		case 'D':
			attrs->dim = attrs->dim == 0
				? A_DIM
				: 0;
			break;
		case 'b':
		case 'B':
			attrs->bold = attrs->bold == 0
				? A_BOLD
				: 0;
			break;
		case 'p':
		case 'P':
			attrs->protect = attrs->protect == 0
				? A_PROTECT
				: 0;
			break;
		case 'i':
		case 'I':
			attrs->invis = attrs->invis == 0
				? A_INVIS
				: 0;
			break;
		case 'a':
		case 'A':
			attrs->altcharset = attrs->altcharset == 0
				? A_ALTCHARSET
				: 0;
			break;
		case 'c':
		case 'C':
			attrs->chartext = attrs->chartext == 0
				? A_CHARTEXT
				: 0;
			break;
	}

}

int
handle_input (char *ch, struct Attribs *attrs)
{
	switch (*ch)
	{
		case 4:
		case 10:
		case 27:
		case 'q':
		case 'Q':
			return true;
		default:
			manipulate_attrs(*ch, attrs);
	}

	return false;
}

int
generate_bitmask (struct Attribs *attrs)
{
	return
		attrs->standout |
		attrs->underline |
		attrs->reverse |
		attrs->blink |
		attrs->dim |
		attrs->bold |
		attrs->protect |
		attrs->invis |
		attrs->altcharset |
		attrs->chartext;
}

void
render_menu (int height, struct Attribs *attrs)
{
	char *output;
	output = malloc (MENU_WIDTH * sizeof (char));
	sprintf(
			output,
			"[S]out (%d) | [U]ine (%d) | [R]ev (%d) | "
			"B[l]ink (%d) | [D]im (%d) | [B]old (%d) | "
			"[P]rot (%d) | [I]nvis (%d) | [A]ltch (%d) | "
			"[C]htxt (%d) | [N]ormal | [Q]uit",
			attrs->standout > 0,
			attrs->underline > 0,
			attrs->reverse > 0,
			attrs->blink > 0,
			attrs->dim > 0,
			attrs->bold > 0,
			attrs->protect > 0,
			attrs->invis > 0,
			attrs->altcharset > 0,
			attrs->chartext > 0
		);
	mvprintw(height, 0, output);
	free(output);
}

int
main ()
{
	int bitmask;
	int width;
	int height;
	char ch;
	bool is_end;

	struct Attribs main_attrs;

	const char *demo_text = "DEMO TEXT";

	initscr();
	cbreak();
	keypad(stdscr, true);
	noecho();
	curs_set(CURSOR_INVISIBLE);

	getmaxyx(stdscr, height, width);
	initialize_attrs(&main_attrs);

	bitmask = A_NORMAL;
	is_end = false;
	ch = '\0';
	do
	{
		is_end = handle_input (&ch, &main_attrs);

		bitmask = generate_bitmask(&main_attrs);
		attron(bitmask);
		mvprintw(
				height / 2,
				(width / 2) - (strlen(demo_text) / 2),
				demo_text
			);

		standend();
		render_menu(height - 2, &main_attrs);
	}
	while (!is_end && (ch = getch()) != ERR);

	curs_set(CURSOR_VISIBLE);
	refresh();
	endwin();

	return 0;
}
