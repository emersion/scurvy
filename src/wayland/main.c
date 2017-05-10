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

void cairo_set_source_u32(cairo_t *cairo, uint32_t color) {
	cairo_set_source_rgba(cairo,
			(color >> (3*8) & 0xFF) / 255.0,
			(color >> (2*8) & 0xFF) / 255.0,
			(color >> (1*8) & 0xFF) / 255.0,
			(color >> (0*8) & 0xFF) / 255.0);
}

struct registry *registry;
struct window *win;

static void window_resize(struct window *_win) {
	set_term_size(win->cairo, win->width, win->height);
}

static void keyboard_event(enum wl_keyboard_key_state state,
		xkb_keysym_t sym, uint32_t code, uint32_t codepoint) {
	// TODO
}

int wayland_main(struct scurvy_child *child) {
	registry = registry_poll();
	registry->input->notify = keyboard_event;
	win = window_setup(registry, 640, 480, 1, true);
	win->notify_resize = window_resize;
	while (wl_display_dispatch(registry->display) != -1) {
		if (window_prerender(win) && win->cairo) {
			cairo_set_source_u32(win->cairo, config->background);
			cairo_paint(win->cairo);
			child_read_pty(child);
			window_render(win);
			wl_display_flush(registry->display);
		}
	}
	window_teardown(win);
	registry_teardown(registry);
	return 0;
}
