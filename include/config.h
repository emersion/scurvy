#ifndef _SCURVY_CONFIG_H
#define _SCURVY_CONFIG_H
#include <stdint.h>
#include <stdbool.h>

struct scurvy_config {
	struct scurvy_vterm *vterm;
	uint32_t background;
	char *font;
	char *url;
	char *term;
	int font_width, font_height;
};

extern struct scurvy_config *config;

bool load_scurvy_config(struct scurvy_vterm *term, const char *file);

#endif
