#ifndef _SCURVY_TERM_H
#define _SCURVY_TERM_H
#include <vterm.h>
#include <cairo/cairo.h>

extern VTerm *vterm;
extern VTermState *vtstate;
extern VTermScreen *vtscreen;

void term_init();
void term_render();
void set_term_size(cairo_t *cairo, int width, int height);

#endif
