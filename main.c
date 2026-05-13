// Minimal X11 window manager
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>

static Display *dpy;
static Window root;
static int screen_w, screen_h;
static Atom wm_protocols;
static Atom wm_delete;

static Window *wins = NULL;
static int nwins = 0;
static int focused_idx = -1;

static int running = 1;

static void die(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

static void add_win(Window w) {
	for (int i = 0; i < nwins; ++i) if (wins[i] == w) return;
	wins = realloc(wins, sizeof(Window) * (nwins + 1));
	wins[nwins++] = w;
}

static void remove_win(Window w) {
	int j = 0;
	for (int i = 0; i < nwins; ++i) {
		if (wins[i] == w) continue;
		wins[j++] = wins[i];
	}
	nwins = j;
	if (nwins == 0) { free(wins); wins = NULL; }
	else wins = realloc(wins, sizeof(Window) * nwins);
}

static void tile(void) {
	if (nwins == 0) return;
	int gap = 8, border = 2;
	int usable_w = screen_w - gap*2;
	int usable_h = screen_h - gap*2;
	int h = usable_h / nwins;
	for (int i = 0; i < nwins; ++i) {
		int y = gap + i * h;
		int hh = (i == nwins-1) ? (usable_h - h*(nwins-1)) : h;
		XMoveResizeWindow(dpy, wins[i], gap, y, usable_w - border*2, hh - border*2);
		XMapWindow(dpy, wins[i]);
	}
	XFlush(dpy);
}

static void focus_window(int idx) {
	if (idx < 0 || idx >= nwins) return;
	focused_idx = idx;
	XSetInputFocus(dpy, wins[idx], RevertToPointerRoot, CurrentTime);
	XRaiseWindow(dpy, wins[idx]);
}

static void close_focused(void) {
	if (focused_idx < 0 || focused_idx >= nwins) return;
	Window w = wins[focused_idx];
	Atom *protocols = NULL;
	int nprotocols = 0;
	int supports_delete = 0;
	
	if (XGetWMProtocols(dpy, w, &protocols, &nprotocols)) {
		for (int i = 0; i < nprotocols; ++i) {
			if (protocols[i] == wm_delete) {
				supports_delete = 1;
				break;
			}
		}
		if (protocols) XFree(protocols);
	}
	
	if (supports_delete) {
		XEvent ev = {0};
		ev.xclient.type = ClientMessage;
		ev.xclient.window = w;
		ev.xclient.message_type = wm_protocols;
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = wm_delete;
		ev.xclient.data.l[1] = CurrentTime;
		XSendEvent(dpy, w, False, NoEventMask, &ev);
	} else {
		XKillClient(dpy, w);
	}
}

static void spawn_terminal(void) {
	if (fork() == 0) {
		setsid();
		execlp("xterm", "xterm", NULL);
		_exit(1);
	}
}

static int on_xerror(Display *d, XErrorEvent *e) { (void)d; (void)e; return 0; }

int main(void) {
	signal(SIGCHLD, SIG_IGN);

	dpy = XOpenDisplay(NULL);
	if (!dpy) die("cannot open display");

	XSetErrorHandler(on_xerror);

	root = DefaultRootWindow(dpy);
	screen_w = DisplayWidth(dpy, DefaultScreen(dpy));
	screen_h = DisplayHeight(dpy, DefaultScreen(dpy));
	
	wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
	wm_delete = XInternAtom(dpy, "WM_DELETE_WINDOW", False);

	XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask | FocusChangeMask);

    // Hotkeys
    XGrabKey(dpy, XKeysymToKeycode(dpy, XK_Return), Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XK_q), Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XK_j), Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XK_k), Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, XKeysymToKeycode(dpy, XK_c), Mod1Mask | ShiftMask, root, True, GrabModeAsync, GrabModeAsync);
	
	XSync(dpy, False);

	while (running) {
		XEvent ev;
		XNextEvent(dpy, &ev);
		switch (ev.type) {
			case MapRequest:
				add_win(ev.xmaprequest.window);
				XMapWindow(dpy, ev.xmaprequest.window);
				focus_window(nwins - 1);
				tile();
				break;
			case DestroyNotify:
				remove_win(ev.xdestroywindow.window);
				if (focused_idx >= nwins) focused_idx = nwins - 1;
				if (focused_idx >= 0) focus_window(focused_idx);
				tile();
				break;
			case UnmapNotify:
				remove_win(ev.xunmap.window);
				if (focused_idx >= nwins) focused_idx = nwins - 1;
				if (focused_idx >= 0) focus_window(focused_idx);
				tile();
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

	if (wins) free(wins);
	XCloseDisplay(dpy);
	return 0;
}

