#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

#include <ncurses-playground.h>
#include "../common.h"

#define CONTROL_WINDOW_HEIGHT 12
#define CONTROL_WINDOW_WIDTH  40
#define MAIN_WINDOW_MIN_HEIGHT 2
#define MAIN_WINDOW_MIN_WIDTH  2
#define FPS_SMOOTHING 0.9

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
 * @brief Calculate the smooth average FPS.
 *
 * @param [in]  start Time measured before rendering.
 * @param [in]  end   Time measured after rendering.
 * @param [out] fps   Variable to write new fps value to.
 */
void
calculate_fps (clock_t start, clock_t end, double *fps)
{
	double current;

	current = (double) (end - start) / CLOCKS_PER_SEC;
	*fps = (*fps * FPS_SMOOTHING) + (current * (1 - FPS_SMOOTHING));
}

/**
 * @brief Clear the given window.
 *
 * @param [in] win    Window to clear.
 * @param [in] height Height of window.
 * @param [in] width  Width of window.
 */
void
clear_background (WINDOW *win, int height, int width)
{
	for (int h = 0; h < height; h++)
	{
		mvwprintw(win, h, 0, "%*s", width, "");
	}
	wrefresh(win);
}

/**
 * @brief Display the state of the main window in the control window.
 *
 * @param [in] win           Ncurses window to display state into.
 * @param [in] custom_window CustomWindow state to display.
 * @param [in] fps           FPS to display in corner.
 */
void
render_control_window (WINDOW *win, CustomWindow *custom_window, double *fps)
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

	// display fps in top-right corner
	// NOTES:
	// - `CONTROL_WINDOW_WIDTH` to start from right border
	// - `-12` = 10 (format string width), 2 (padding)
	// - "%6.1f" formats the fps snugly w/ a resolution of 4 digits
	//   and 1 decimal point
	// - `1 / *fps` to display actual fps and not time per frame
	mvwprintw(
			win,
			1,
			CONTROL_WINDOW_WIDTH - 12,
			"FPS %6.1f",
			1 / *fps);
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
	double *fps;
	clock_t start;
	clock_t end;

	WINDOW *background;
	WINDOW *control_win;
	CustomWindow *main_window;

	// initialize ncurses
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(CURSOR_INVISIBLE);

	// initialize color mode
	start_color();
	init_color(COLOR_GREEN, 50, 250, 50);
	init_pair(1, COLOR_WHITE, COLOR_GREEN);

	// create a new control window
	getmaxyx(stdscr, height, width);
	control_win = newwin(
			CONTROL_WINDOW_HEIGHT,
			CONTROL_WINDOW_WIDTH,
			0,
			0);

	// define background to be the container for the main window
	// also enable the first color pair for it
	background = newwin(height, width/2, 0, width/2);
	wattron(background, COLOR_PAIR(1));

	// create and initialize main window state
	main_window = malloc (sizeof (CustomWindow));
	main_window->height = 2;
	main_window->width = 2;
	main_window->x = width / 2 + 1;
	main_window->y = 2;
	main_window->win = newwin(
			main_window->height,
			main_window->width,
			main_window->y,
			main_window->x);

	fps = malloc(sizeof (double));

	// start main loop
	is_end = false;
	*fps = 1.0;
	while (!is_end)
	{
		// measure time before render
		start = clock();

		refresh();
		getmaxyx(stdscr, height, width);

		// clear the render area (container + main_window)
		clear_background(background, height, width / 2);

		// render main and control windows
		// in that order to bypass artefacts on the control window
		render_main_window(main_window);
		render_control_window(control_win, main_window, fps);

		// measure time after render and calculate fps
		end = clock();
		calculate_fps(start, end, fps);

		// handle a newly input character
		ch = getch();
		is_end = handle_input(ch, main_window, height, width);
	}

	// free resources
	free(main_window);
	free(fps);

	// de-initialize ncurses
	curs_set(CURSOR_VISIBLE);
	endwin();

	// exit successfully
	return 0;
}
