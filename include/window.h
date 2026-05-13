#ifndef WINDOW_H
#define WINDOW_H

#include <X11/Xlib.h>

/* Window list management */
void add_win(Window w);
void remove_win(Window w);

/* Layout and focus */
void tile(void);
void tile_tree(void);
void apply_layout(void);
void focus_window(int idx);

#endif
