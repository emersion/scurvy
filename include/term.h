#ifndef _SCURVY_TERM_H
#define _SCURVY_TERM_H
#include <vterm.h>
#include <cairo/cairo.h>

struct scurvy_vterm {
	VTerm *vterm;
	VTermState *vtstate;
	VTermScreen *vtscreen;
};

struct scurvy_vterm *term_init();
void term_render(struct scurvy_vterm *term, cairo_t *cairo);
void set_term_size(struct scurvy_vterm *term, cairo_t *cairo, int width, int height);

#endif
