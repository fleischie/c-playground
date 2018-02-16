#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/XKBrules.h>

static Display *dpy;

enum return_codes {
	RETURN_OK = 0,
	RETURN_ERROR
};

void
display_xkb_display_error (int reason, int major, int minor, const char *name)
{
	switch (reason)
	{
		// version mismatch of library or server/client
		case XkbOD_BadLibraryVersion:
		case XkbOD_BadServerVersion:
			fprintf(
					stderr,
					"Incompatible versions between server "
					"(%d.%d) and client (%d.%d).\n",
					major,
					minor,
					XkbMajorVersion,
					XkbMinorVersion);
			break;
		// connection to the given display was refused
		case XkbOD_ConnectionRefused:
			fprintf(
					stderr,
					"Connection to (%s) refused by server."
					"\n",
					name);
			break;
		// X server does not support keyboard extensions
		case XkbOD_NonXkbServer:
			fprintf(
					stderr,
					"(%s) does not support the XKB"
					"extension.\n",
					name);
		// unknown error
		default:
			fprintf(
					stderr,
					"Unknown error (%d).\n",
					reason);
			break;
	}
}

int
get_current_keyboard_layout (char **layout)
{
	int major;
	int minor;
	int reason;
	char *display_name;
	XkbRF_VarDefsRec defs;

	// setup parameters to get display with
	display_name = getenv("DISPLAY");
	display_name = display_name == NULL
		? ""
		: display_name;
	major = XkbMajorVersion;
	minor = XkbMinorVersion;

	// retrieve display
	dpy = XkbOpenDisplay(
			display_name,
			NULL,
			NULL,
			&major,
			&minor,
			&reason);

	// handle missing display and exit the program
	if (!dpy)
	{
		// give unset display_name a sensible content for debug
		// message
		display_name = display_name == NULL
			? "default display"
			: display_name;

		display_xkb_display_error(
				reason,
				major,
				minor,
				display_name);

		return EXIT_FAILURE;
	}

	// store retrieved layout in variable
	*layout = defs.layout;

	return RETURN_OK;
}

/**
 * @brief Print ISO639 keyboard layout code.
 *
 * This program uses the X11 xkb extension to determine the currently
 * active keyboard layout.
 *
 * @return Exit status.
 */
int
main ()
{
	char *layout;

	if (get_current_keyboard_layout(&layout) != RETURN_OK)
	{
		layout = "";
	}

	puts(layout);

	return EXIT_SUCCESS;
}
