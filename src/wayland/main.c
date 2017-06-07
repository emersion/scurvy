#define _POSIX_C_SOURCE 200809L
#include <xkbcommon/xkbcommon.h>
#include <wayland-client.h>
#include <cairo/cairo.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include "wayland/window.h"
#include "wayland/registry.h"
#include "config.h"
#include "child.h"
#include "term.h"
#include "log.h"

static int term_damage(VTermRect rect, void *data) {
	return 0;
}

static int term_moverect(VTermRect dest, VTermRect src, void *data) {
	return 0;
}

static int term_movecursor(VTermPos new, VTermPos old, int visible, void *data) {
	return 0;
}

static int term_settermprop(VTermProp prop, VTermValue *val, void *data) {
	return 0;
}

static int term_bell(void *data) {
	return 0;
}

static int term_sb_push(int cols, const VTermScreenCell *cells, void *data) {
	return 0;
}

static int term_sb_pop(int cols, VTermScreenCell *cells, void *data) {
	return 0;
}

static int term_resize(int rows, int cols, void *data) {
	if (rows + cols) {
		struct scurvy_child *child = data;
		struct winsize ws = { .ws_row = rows, .ws_col = cols };
		ioctl(child->fd, TIOCSWINSZ, &ws);
	}
	return 1;
}

static VTermScreenCallbacks vterm_screen_callbacks = {
	.damage = term_damage,
	.moverect = term_moverect,
	.movecursor = term_movecursor,
	.settermprop = term_settermprop,
	.bell = term_bell,
	.sb_pushline = term_sb_push,
	.sb_popline  = term_sb_pop,
	.resize = term_resize
};

static void window_resize(struct window *win) {
	set_term_size(win->term, win->cairo, win->width, win->height);
}

static void keyboard_event(enum wl_keyboard_key_state state,
		xkb_keysym_t sym, uint32_t code, uint32_t codepoint) {
	// TODO
}

int wayland_main(struct scurvy_vterm *term, struct scurvy_child *child) {
	vterm_screen_set_callbacks(term->vtscreen, &vterm_screen_callbacks, child);
	struct registry *registry = registry_poll();
	registry->input->notify = keyboard_event;
	struct window *win = window_setup(registry, 640, 480, 1, true);
	win->term = term;
	win->notify_resize = window_resize;
	while (wl_display_dispatch(registry->display) != -1) {
		struct buffer *old_buffer = win->buffer;
		if (window_prerender(win) && win->cairo) {
			if (old_buffer != win->buffer) {
				vterm_screen_flush_damage(term->vtscreen);
			}
			child_read_pty(child);
			child_write_pty(child);
			term_render(term, win->cairo);
			window_render(win);
			wl_display_flush(registry->display);
		}
		struct timespec spec = { 0, .5e+8 };
		nanosleep(&spec, NULL);
	}
	window_teardown(win);
	registry_teardown(registry);
	return 0;
}
