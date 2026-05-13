#include "../include/wm.h"
#include "../include/window.h"
#include <stdlib.h>
#include <X11/Xlib.h>

void add_win(Window w) {
	for (int i = 0; i < nwins; ++i) if (wins[i] == w) return;
	wins = realloc(wins, sizeof(Window) * (nwins + 1));
	wins[nwins++] = w;
}

void remove_win(Window w) {
	int j = 0;
	for (int i = 0; i < nwins; ++i) {
		if (wins[i] == w) continue;
		wins[j++] = wins[i];
	}
	nwins = j;
	if (nwins == 0) { free(wins); wins = NULL; }
	else wins = realloc(wins, sizeof(Window) * nwins);
}

void tile(void) {
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

void tile_tree(void) {
	if (nwins == 0) return;
	int gap = 8, border = 2;
	int usable_w = screen_w - gap*2;
	int usable_h = screen_h - gap*2;
	
	int master_w = (usable_w / 2);
	XMoveResizeWindow(dpy, wins[0], gap, gap, master_w - border*2, usable_h - border*2);
	XMapWindow(dpy, wins[0]);
	
	int stack_w = usable_w - master_w;
	int stack_h = (nwins > 1) ? (usable_h / (nwins - 1)) : usable_h;
	for (int i = 1; i < nwins; ++i) {
		int y = gap + (i - 1) * stack_h;
		int hh = (i == nwins - 1) ? (usable_h - stack_h * (nwins - 2)) : stack_h;
		XMoveResizeWindow(dpy, wins[i], gap + master_w, y, stack_w - border*2, hh - border*2);
		XMapWindow(dpy, wins[i]);
	}
	XFlush(dpy);
}

void apply_layout(void) {
	tile_tree();
}

void focus_window(int idx) {
	if (idx < 0 || idx >= nwins) return;
	focused_idx = idx;
	XSetInputFocus(dpy, wins[idx], RevertToPointerRoot, CurrentTime);
	XRaiseWindow(dpy, wins[idx]);
}
