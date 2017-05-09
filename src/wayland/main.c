#include <wayland-client.h>
#include <cairo/cairo.h>
#include <stdbool.h>
#include <stdint.h>
#include "wayland/window.h"
#include "wayland/registry.h"

void cairo_set_source_u32(cairo_t *cairo, uint32_t color) {
	cairo_set_source_rgba(cairo,
			(color >> (3*8) & 0xFF) / 255.0,
			(color >> (2*8) & 0xFF) / 255.0,
			(color >> (1*8) & 0xFF) / 255.0,
			(color >> (0*8) & 0xFF) / 255.0);
}

struct registry *registry;
struct window *win;

int wayland_main(void) {
	registry = registry_poll();
	win = window_setup(registry, 640, 480, 1, true);
	while (wl_display_dispatch(registry->display) != -1) {
		if (window_prerender(win) && win->cairo) {
			cairo_set_source_u32(win->cairo, 0xFF0000FF);
			cairo_paint(win->cairo);
			window_render(win);
			wl_display_flush(registry->display);
		}
	}
	window_teardown(win);
	registry_teardown(registry);
	return 0;
}
