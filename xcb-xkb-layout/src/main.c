#include <stdlib.h>
#include <xcb/xcb.h>
#include <xcb/xkb.h>
#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>

int
main ()
{
	int ret;
	int core_kbd_device_id;

	struct xkb_context *ctx;
	struct xkb_keymap *keymap;

	xcb_connection_t *conn;

	conn = xcb_connect(NULL, NULL);
	ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

	if (!conn || xcb_connection_has_error(conn))
	{
		fprintf(
				stderr,
				"Couldn't connect to X server: error code %d"
				"\n",
				conn ? xcb_connection_has_error(conn) : -1);
		return EXIT_FAILURE;
	}

	ret = xkb_x11_setup_xkb_extension(
			conn,
			XKB_X11_MIN_MAJOR_XKB_VERSION,
			XKB_X11_MIN_MINOR_XKB_VERSION,
			XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS,
			NULL,
			NULL,
			NULL,
			NULL);
	if (!ret)
	{
		fprintf(stderr, "Couldn't setup XKB extension\n");
		return EXIT_FAILURE;
	}

	core_kbd_device_id = xkb_x11_get_core_keyboard_device_id(conn);
	if (core_kbd_device_id == -1)
	{
		fprintf(stderr, "Couldn't find core keyboard device\n");
		return EXIT_FAILURE;
	}

	keymap = xkb_x11_keymap_new_from_device(
			ctx,
			conn,
			core_kbd_device_id,
			XKB_KEYMAP_COMPILE_NO_FLAGS);

	if (!keymap)
	{
		fprintf(stderr, "Couldn't get keymap from device\n");
		return EXIT_FAILURE;
	}

	printf(
			"%s\n",
			xkb_keymap_layout_get_name(keymap, 0));

	xkb_keymap_unref(keymap);
	xkb_context_unref(ctx);
	xcb_disconnect(conn);

	return EXIT_SUCCESS;
}
