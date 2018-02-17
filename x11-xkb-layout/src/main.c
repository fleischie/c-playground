#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/XKBrules.h>

static Display *dpy;

/**
 * @brief Display xkb display errors.
 *
 * Use the given parameters to determine cause of error and print an
 * appropriate message.
 *
 * @param [in] reason Integer value representing the error cause.
 * @param [in] major  Integer major version value of server.
 * @param [in] minor  Integer minor version value of server.
 * @param [in] name   String representing the display name.
 */
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
			break;
		// unknown error
		default:
			fprintf(
					stderr,
					"Unknown error (%d).\n",
					reason);
			break;
	}
}

/**
 * @brief Retrieve the current keyboard layout.
 *
 * Retrieve the currently used keyboard layout from the displays
 * definitions. Store the layout in the given char pointer reference.
 * This function does not alter the reference, if an error was
 * encountered.
 *
 * @param [out] layout Char pointer reference to write the layout into.
 */
void
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
	}

	// retrieve keyboard layout from server and store in variable
	if (XkbRF_GetNamesProp(dpy, NULL, &defs))
	{
		*layout = defs.layout;
	}
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

	// retrieve the layout if possible
	layout = NULL;
	get_current_keyboard_layout(&layout);

	// if the layout couldn't be determined the program failed
	if (!layout)
	{
		return EXIT_FAILURE;
	}

	// display the successfully retrieved layout and exit
	// successfully
	puts(layout);
	return EXIT_SUCCESS;
}
