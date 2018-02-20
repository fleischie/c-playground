#include <stdlib.h>
#include <ncurses.h>

#include <ncurses-playground.h>
#include "../common.h"

#define CONTROL_WINDOW_HEIGHT 12
#define CONTROL_WINDOW_WIDTH  40
#define MAIN_WINDOW_MIN_HEIGHT 2
#define MAIN_WINDOW_MIN_WIDTH  2

/**
 * @struct CustomWindow
 * @brief Main window's state.
 */
typedef struct window_t {
	// window reference
	WINDOW *win;

	// geometry
	int height;
	int width;
	int x;
	int y;
} CustomWindow;

/**
 * @brief Display the state of the main window in the control window.
 *
 * @param [in] win           Ncurses window to display state into.
 * @param [in] custom_window CustomWindow state to display.
 */
void
render_control_window (WINDOW *win, CustomWindow *custom_window)
{
	box(win, 0, 0);
	mvwprintw(
			win,
			2,
			3,
			"Press buttons!");
	mvwprintw(
			win,
			4,
			3,
			"Arrow keys to move, WASD to resize.");
	mvwprintw(
			win,
			6,
			3,
			"- Position  (%4i, %4i)",
			custom_window->x,
			custom_window->y);
	mvwprintw(
			win,
			7,
			3,
			"- Dimension (%4i, %4i)",
			custom_window->width - MAIN_WINDOW_MIN_WIDTH,
			custom_window->height - MAIN_WINDOW_MIN_HEIGHT);
	mvwprintw(
			win,
			9,
			3,
			"[Q]uit");
	wrefresh(win);
}

/**
 * @brief Display the main window.
 *
 * @param [in] win CustomWindow object to display.
 */
void
render_main_window (CustomWindow *win)
{
	// clear and remove currently displayed window
	wborder(win->win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(win->win);
	delwin(win->win);

	// create new window instance with the updated state
	win->win = newwin(win->height, win->width, win->y, win->x);
	box(win->win, 0, 0);
	wrefresh(win->win);
}

/**
 * @brief Handle input character and update state of the CustomWindow.
 *
 * @param [in] ch         Character to update state with.
 * @param [in] win        Window instance to update state on.
 * @param [in] max_height Maximum height of the container area.
 * @param [in] max_width  Maximum width of the container area.
 */
bool
handle_input (char ch, CustomWindow *win, int max_height, int max_width)
{
	// calculate left-most border to restrict main window to
	int min_x = max_width / 2;

	switch (ch)
	{
		// left-arrow
		case 4:
			// decrease left value
			win->x -= win->x > min_x ? 1 : 0;
			break;
		// right-arrow
		case 5:
			// increase left value
			win->x += win->x + win->width < max_width ? 1 : 0;
			break;
		// up-arrow
		case 3:
			// decrease top value
			win->y -= win->y > 0 ? 1 : 0;
			break;
		// down-arrow
		case 2:
			// increase top value
			win->y += win->y + win->height < max_height ? 1 : 0;
			break;

		case 'w':
			// decrease height
			win->height -= win->height > MAIN_WINDOW_MIN_HEIGHT
				? 1
				: 0;
			break;
		case 's':
			// increase height
			win->height += win->y + win->height < max_height
				? 1
				: 0;
			break;
		case 'a':
			// decrease width
			win->width -= win->width > MAIN_WINDOW_MIN_WIDTH
				? 1
				: 0;
			break;
		case 'd':
			// increase width
			win->width += win->x + win->width < max_width ? 1 : 0;
			break;

		case 'q':
		case 'Q':
			return true;
	}

	return false;
}

int
main ()
{
	bool is_end;
	char ch;
	int height;
	int width;

	WINDOW *control_win;
	CustomWindow *main_window;

	// initialize ncurses
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(CURSOR_INVISIBLE);

	// create a new control window
	getmaxyx(stdscr, height, width);
	control_win = newwin(
			CONTROL_WINDOW_HEIGHT,
			CONTROL_WINDOW_WIDTH,
			0,
			0);

	// create and initialize main window state
	main_window = malloc (sizeof (CustomWindow));
	main_window->height = 2;
	main_window->width = 2;
	main_window->x = width / 2 + 1;
	main_window->y = 2;
	main_window->win = newwin(
			main_window->y,
			main_window->x,
			main_window->height,
			main_window->width);

	// start main loop
	is_end = false;
	while (!is_end)
	{
		refresh();
		getmaxyx(stdscr, height, width);

		// render main and control windows
		// in that order to bypass artefacts on the control window
		render_main_window(main_window);
		render_control_window(control_win, main_window);

		// handle a newly input character
		ch = getch();
		is_end = handle_input(ch, main_window, height, width);
	}

	// de-initialize ncurses and main window
	curs_set(CURSOR_VISIBLE);
	refresh();
	free(main_window);
	endwin();

	// exit successfully
	return 0;
}
