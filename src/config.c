#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wordexp.h>
#include <vterm.h>
#include "log.h"
#include "ini.h"
#include "colors.h"
#include "config.h"
#include "term.h"

struct scurvy_config *config = NULL;

static bool file_exists(const char *path) {
	return access(path, R_OK) != -1;
}

static char *get_config_path(const char **config_paths, int len) {
	if (!getenv("XDG_CONFIG_HOME")) {
		char *home = getenv("HOME");
		char *config_home = malloc(strlen(home) + strlen("/.config") + 1);
		strcpy(config_home, home);
		strcat(config_home, "/.config");
		setenv("XDG_CONFIG_HOME", config_home, 1);
		free(config_home);
	}

	wordexp_t p;
	char *path;

	int i;
	for (i = 0; i < len; ++i) {
		if (wordexp(config_paths[i], &p, 0) == 0) {
			path = strdup(p.we_wordv[0]);
			wordfree(&p);
			if (file_exists(path)) {
				return path;
			}
		}
	}

	return NULL; // Not reached
}

static bool open_config(const char *path, FILE **f) {
	struct stat sb;
	if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
		return false;
	}

	if (path == NULL) {
		scurvy_log(L_ERROR, "Unable to find a config file!");
		return false;
	}

	*f = fopen(path, "r");
	return *f != NULL;
}

static void set_color(struct scurvy_config *config,
		const char *key, const char *value) {
	VTermColor fg, bg;
	vterm_state_get_default_colors(config->vterm->vtstate, &fg, &bg);
	uint32_t color;
	if (!color_parse(value, &color)) {
		scurvy_log(L_ERROR, "Invalid color specification '%s'", value);
		return;
	}
	VTermColor col = {
		color >> 8 & 0xFF,
		color >> 16 & 0xFF,
		color >> 24 & 0xFF,
	};
	if (strncmp(key, "color", 5) == 0) {
		char *end;
		int index = strtol(key + 5, &end, 10);
		if (index < 0 || index > 255) {
			scurvy_log(L_ERROR, "Invalid color configuration '%s'", key);
			return;
		}
		vterm_state_set_palette_color(config->vterm->vtstate, index, &col);
		scurvy_log(L_DEBUG, "Set color%d to %08X", index, color);
	} else if (strcmp(key, "foreground") == 0) {
		vterm_state_set_default_colors(config->vterm->vtstate, &col, &bg);
		scurvy_log(L_DEBUG, "Set foreground to %08X", color);
	} else if (strcmp(key, "background") == 0) {
		vterm_state_set_default_colors(config->vterm->vtstate, &fg, &col);
		config->background = color;
		scurvy_log(L_DEBUG, "Set background to %08X", color);
	} else {
		scurvy_log(L_ERROR, "Invalid color configuration '%s'", key);
	}
}

int handle_config_option(void *_config, const char *section,
		const char *key, const char *value) {
	struct scurvy_config *config = _config;
	struct { const char *section; const char *key; char **string; } strings[] = {
		{ "scurvy", "font", &config->font },
		{ "scurvy", "url", &config->url },
		{ "scurvy", "term", &config->term },
	};

	if (strcmp(section, "colors") == 0) {
		set_color(config, key, value);
		return 1;
	}

	for (size_t i = 0; i < sizeof(strings) / (sizeof(void *) * 3); ++i) {
		if (strcmp(strings[i].section, section) == 0
				&& strcmp(strings[i].key, key) == 0) {
			if(*strings[i].string) free(*strings[i].string);
			*strings[i].string = strdup(value);
			return 1;
		}
	}

	scurvy_log(L_ERROR, "Unknown config option [%s]%s", section, key);
	return 1;
}

static bool load_config(const char *path, struct scurvy_vterm *term,
		struct scurvy_config *config) {
	FILE *f;
	if (!open_config(path, &f)) {
		scurvy_log(L_ERROR, "Unable to open %s for reading", path);
		return false;
	}

	scurvy_log(L_DEBUG, "Loading config from %s", path);
	config->vterm = term;

	int ini = ini_parse_file(f, handle_config_option, config);
	if (ini != 0) {
		scurvy_log(L_ERROR, "Configuration parsing error on line %d", ini);
	}

	fclose(f);
	return ini == 0;
}

static void config_defaults() {
	config->font = strdup("monospace 10");
	config->url = strdup("xdg-open");
	config->term = strdup("xterm");
}

void free_config(struct scurvy_config *config) {
	free(config->font);
	free(config->url);
	free(config->term);
}

bool load_scurvy_config(struct scurvy_vterm *term, const char *file) {
	static const char *config_paths[] = {
		"$XDG_CONFIG_HOME/scurvy.conf",
	};

	char *path;
	if (file != NULL) {
		path = strdup(file);
	} else {
		path = get_config_path(config_paths, 3);
	}

	struct scurvy_config *old_config = config;
	config = calloc(1, sizeof(struct scurvy_config));

	config_defaults(config);

	bool success = load_config(path, term, config);

	if (old_config) {
		free_config(old_config);
	}

	free(path);
	return success;
}
