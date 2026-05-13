#include "wm.h"
#include "display.h"
#include "input.h"
#include <stdlib.h>
#include <X11/Xlib.h>

void init_display(void) {
	display = XOpenDisplay(NULL);
	if (!display) die("cannot open display");

	XSetErrorHandler(on_xerror);

	root = DefaultRootWindow(display);
	screen_w = DisplayWidth(display, DefaultScreen(display));
	screen_h = DisplayHeight(display, DefaultScreen(display));
	
	wm_protocols = XInternAtom(display, "WM_PROTOCOLS", False);
	wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);

	XSelectInput(display, root, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask | FocusChangeMask);

	setup_hotkeys();
	
	XSync(display, False);
}

void cleanup_display(void) {
	if (wins) free(wins);
	XCloseDisplay(display);
}
