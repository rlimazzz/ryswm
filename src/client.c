#include "../include/wm.h"
#include "../include/client.h"
#include <unistd.h>
#include <sys/types.h>
#include <X11/Xlib.h>

static void close_window(Window w) {
	Atom *protocols = NULL;
	int nprotocols = 0;
	int supports_delete = 0;

	if (XGetWMProtocols(display, w, &protocols, &nprotocols)) {
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
		XSendEvent(display, w, False, NoEventMask, &ev);
	} else {
		XKillClient(display, w);
	}
}

void close_all(void) {
	for (int i = 0; i < nwins; ++i) {
		close_window(wins[i]);
	}
}

void close_focused(void) {
	if (focused_idx < 0 || focused_idx >= nwins) return;
	close_window(wins[focused_idx]);
}

void spawn_terminal(void) {
	if (fork() == 0) {
		setsid();
		execlp("xterm", "xterm", NULL);
		_exit(1);
	}
}
