#include <xkbcommon/xkbcommon.h>
#include <wayland-client.h>
#include <cairo/cairo.h>
#include <stdbool.h>
#include <stdint.h>
#include "wayland/window.h"
#include "wayland/registry.h"
#include "config.h"
#include "child.h"
#include "term.h"
#include "log.h"

struct registry *registry;
struct window *win;

static int term_damage(VTermRect rect, void *data) {
	scurvy_log(L_DEBUG, "damage");
	return 0;
}

static int term_moverect(VTermRect dest, VTermRect src, void *data) {
	scurvy_log(L_DEBUG, "moverect");
	return 0;
}

static int term_movecursor(VTermPos new, VTermPos old, int visible, void *data) {
	scurvy_log(L_DEBUG, "movecursor");
	return 0;
}

static int term_settermprop(VTermProp prop, VTermValue *val, void *data) {
	scurvy_log(L_DEBUG, "prop");
	return 0;
}

static int term_bell(void *data) {
	scurvy_log(L_DEBUG, "bell");
	return 0;
}

static int term_sb_push(int cols, const VTermScreenCell *cells, void *data) {
	scurvy_log(L_DEBUG, "sb_push");
	return 0;
}

static int term_sb_pop(int cols, VTermScreenCell *cells, void *data) {
	scurvy_log(L_DEBUG, "sb_pop");
	return 0;
}

static VTermScreenCallbacks vterm_screen_callbacks = {
	.damage      = term_damage,
	.moverect    = term_moverect,
	.movecursor  = term_movecursor,
	.settermprop = term_settermprop,
	.bell        = term_bell,
	.sb_pushline = term_sb_push,
	.sb_popline  = term_sb_pop,
};

static void window_resize(struct window *_win) {
	set_term_size(win->cairo, win->width, win->height);
}

static void keyboard_event(enum wl_keyboard_key_state state,
		xkb_keysym_t sym, uint32_t code, uint32_t codepoint) {
	// TODO
}

int wayland_main(struct scurvy_child *child) {
	vterm_screen_set_callbacks(vtscreen, &vterm_screen_callbacks, NULL);
	registry = registry_poll();
	registry->input->notify = keyboard_event;
	win = window_setup(registry, 640, 480, 1, true);
	win->notify_resize = window_resize;
	while (wl_display_dispatch(registry->display) != -1) {
		if (window_prerender(win) && win->cairo) {
			child_read_pty(child);
			child_write_pty(child);
			term_render(win->cairo);
			window_render(win);
			wl_display_flush(registry->display);
		}
	}
	window_teardown(win);
	registry_teardown(registry);
	return 0;
}
