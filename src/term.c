#include <cairo/cairo.h>
#include <vterm.h>
#include <string.h>
#include "term.h"
#include "pango.h"
#include "config.h"
#include "child.h"
#include "log.h"

struct VTerm *vterm;
struct VTermState *vtstate;
struct VTermScreen *vtscreen;

void cairo_set_source_u32(cairo_t *cairo, uint32_t color) {
	cairo_set_source_rgba(cairo,
			(color >> (3*8) & 0xFF) / 255.0,
			(color >> (2*8) & 0xFF) / 255.0,
			(color >> (1*8) & 0xFF) / 255.0,
			(color >> (0*8) & 0xFF) / 255.0);
}

void term_render(cairo_t *cairo) {
	cairo_set_source_u32(cairo, config->background);
	cairo_paint(cairo);
	int rows, cols;
	vterm_get_size(vterm, &rows, &cols);
	VTermScreenCell cell;
	char buf[VTERM_MAX_CHARS_PER_CELL + 1];
	int tw, th, _;
	get_text_size(cairo, config->font, &tw, &th, 1, false, "Hello world");
	for (int row = 0; row < rows; ++row) {
		for (int col = 0, x = 0; col < cols; ++col) {
			VTermPos pos = { row, col };
			vterm_screen_get_cell(vtscreen, pos, &cell);
			memcpy(buf, cell.chars, VTERM_MAX_CHARS_PER_CELL);
			buf[(int)cell.width] = '\0';

			get_text_size(cairo, config->font, &tw, &_, 1, false, "%s", buf);
			cairo_set_source_rgb(cairo,
					cell.bg.red / 256.0,
					cell.bg.green / 256.0,
					cell.bg.blue / 256.0);
			cairo_rectangle(cairo, x, row * th, tw, _);

			cairo_move_to(cairo, x, row * th);
			cairo_set_source_rgb(cairo,
					cell.fg.red / 256.0,
					cell.fg.green / 256.0,
					cell.fg.blue / 256.0);
			pango_printf(cairo, config->font, 1, false, "%s", buf);
			x += tw;
		}
	}
}

void term_init() {
	vterm = vterm_new(24, 80);
	vtstate = vterm_obtain_state(vterm);
	vtscreen = vterm_obtain_screen(vterm);

	vterm_set_utf8(vterm, 1);
	vterm_screen_set_damage_merge(vtscreen, VTERM_DAMAGE_CELL);
	vterm_screen_reset(vtscreen, 1);
	vterm_state_set_bold_highbright(vtstate, true);
}

void set_term_size(cairo_t *cairo, int width, int height) {
	int rows, cols;
	get_text_size(cairo, config->font, &cols, &rows, 1, false, "H");
	rows = height / rows;
	cols = width / cols;
	int old_rows, old_cols;
	vterm_get_size(vterm, &old_rows, &old_cols);
	if ((rows && cols) && (old_rows != rows || old_cols != cols)) {
		scurvy_log(L_DEBUG, "Setting size to %d rows %d cols", rows, cols);
		vterm_set_size(vterm, rows, cols);
		vterm_screen_flush_damage(vtscreen);
	}
}
