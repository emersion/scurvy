#ifndef _SCURVY_CONFIG_H
#define _SCURVY_CONFIG_H
#include <stdint.h>
#include <stdbool.h>

struct scurvy_config {
	char *font;
	char *url;
	char *term;
};

extern struct scurvy_config *config;

bool load_scurvy_config(const char *file);

#endif
