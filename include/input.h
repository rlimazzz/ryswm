#ifndef INPUT_H
#define INPUT_H

#include <X11/keysym.h>

/* Keyboard input handling */
void grab_key_with_mods(KeySym ks, unsigned int mods);
void setup_hotkeys(void);

#endif
