#ifndef WM_H
#define WM_H

#include <X11/Xlib.h>

/* Global window manager state */
extern Display *dpy;
extern Window root;
extern int screen_w, screen_h;
extern Atom wm_protocols;
extern Atom wm_delete;

/* Window list and focus state */
extern Window *wins;
extern int nwins;
extern int focused_idx;

/* Runtime state */
extern int running;

/* Utility functions */
void die(const char *msg);
int on_xerror(Display *d, XErrorEvent *e);

#endif
