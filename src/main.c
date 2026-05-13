// Minimal X11 window manager
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "wm.h"
#include "window.h"
#include "client.h"
#include "display.h"

/* Global state definitions */
Display *dpy = NULL;
Window root = 0;
int screen_w = 0, screen_h = 0;
Atom wm_protocols, wm_delete;

Window *wins = NULL;
int nwins = 0;
int focused_idx = -1;

int running = 1;

void die(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

int on_xerror(Display *d, XErrorEvent *e) { (void)d; (void)e; return 0; }

int main(void) {
	signal(SIGCHLD, SIG_IGN);

	init_display();

	while (running) {
		XEvent ev;
		XNextEvent(dpy, &ev);
		switch (ev.type) {
			case MapRequest:
				add_win(ev.xmaprequest.window);
				XMapWindow(dpy, ev.xmaprequest.window);
				focus_window(nwins - 1);
				apply_layout();
				break;
			case DestroyNotify:
				remove_win(ev.xdestroywindow.window);
				if (focused_idx >= nwins) focused_idx = nwins - 1;
				if (focused_idx >= 0) focus_window(focused_idx);
				apply_layout();
				break;
			case UnmapNotify:
				remove_win(ev.xunmap.window);
				if (focused_idx >= nwins) focused_idx = nwins - 1;
				if (focused_idx >= 0) focus_window(focused_idx);
				apply_layout();
				break;
			case ConfigureRequest: {
				XWindowChanges wc;
				wc.x = ev.xconfigurerequest.x;
				wc.y = ev.xconfigurerequest.y;
				wc.width = ev.xconfigurerequest.width;
				wc.height = ev.xconfigurerequest.height;
				wc.border_width = ev.xconfigurerequest.border_width;
				wc.sibling = ev.xconfigurerequest.above;
				wc.stack_mode = ev.xconfigurerequest.detail;
				XConfigureWindow(dpy, ev.xconfigurerequest.window, ev.xconfigurerequest.value_mask, &wc);
				break;
			}
			case KeyPress: {
				KeySym ks = XLookupKeysym(&ev.xkey, 0);
				int shift = ev.xkey.state & ShiftMask;
				int alt = ev.xkey.state & Mod1Mask;
				if (ks == XK_Return && alt) {
					spawn_terminal();
				} else if (ks == XK_q && alt) {
					running = 0;
				} else if (ks == XK_j && alt && nwins > 0) {
					focus_window((focused_idx + 1) % nwins);
				} else if (ks == XK_k && alt && nwins > 0) {
					focus_window((focused_idx - 1 + nwins) % nwins);
				} else if (ks == XK_c && alt && shift) {
					close_focused();
				}
				break;
			}
			default:
				break;
		}
	}

	cleanup_display();
	return 0;
}
