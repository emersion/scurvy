#include <cairo/cairo.h>
#include <vterm.h>
#include "term.h"
#include "pango.h"
#include "config.h"

struct VTerm *vterm;

void set_term_size(cairo_t *cairo, int width, int height) {
	int w, h;
	get_text_size(cairo, config->font, &w, &h, 1, false, "Hello world");
	vterm_set_size(vterm, height / h, width / w);
}
