#include "wm.h"
#include "display.h"
#include "input.h"
#include <stdlib.h>
#include <X11/Xlib.h>

void init_display(void) {
	dpy = XOpenDisplay(NULL);
	if (!dpy) die("cannot open display");

	XSetErrorHandler(on_xerror);

	root = DefaultRootWindow(dpy);
	screen_w = DisplayWidth(dpy, DefaultScreen(dpy));
	screen_h = DisplayHeight(dpy, DefaultScreen(dpy));
	
	wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
	wm_delete = XInternAtom(dpy, "WM_DELETE_WINDOW", False);

	XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask | FocusChangeMask);

	setup_hotkeys();
	
	XSync(dpy, False);
}

void cleanup_display(void) {
	if (wins) free(wins);
	XCloseDisplay(dpy);
}
