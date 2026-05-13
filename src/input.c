#include "wm.h"
#include "input.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>

void grab_key_with_mods(KeySym ks, unsigned int mods) {
	KeyCode kc = XKeysymToKeycode(display, ks);
	unsigned int extra_masks[] = { 0, LockMask, Mod2Mask, LockMask | Mod2Mask };
	for (size_t i = 0; i < sizeof(extra_masks)/sizeof(extra_masks[0]); ++i) {
		XGrabKey(display, kc, mods | extra_masks[i], root, False, GrabModeAsync, GrabModeAsync);
	}
}

void setup_hotkeys(void) {
	grab_key_with_mods(XK_Return, Mod1Mask); // Spawn terminal
	grab_key_with_mods(XK_q, Mod1Mask); // Quit
	grab_key_with_mods(XK_j, Mod1Mask); // Focus prev
	grab_key_with_mods(XK_k, Mod1Mask); // Focus next
	grab_key_with_mods(XK_c, Mod1Mask | ShiftMask); // Close focused
    grab_key_with_mods(XK_Up, Mod1Mask); // Move focused up
    grab_key_with_mods(XK_Down, Mod1Mask); // Move focused down
    grab_key_with_mods(XK_Left, Mod1Mask); // Promote focused to master
}
