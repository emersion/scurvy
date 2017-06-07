#include <stdlib.h>
#include <stdint.h>
#include "config.h"
#include "log.h"
#include "term.h"
#include "child.h"

int wayland_main(struct scurvy_vterm *term, struct scurvy_child *);

int main(int argc, char **argv) {
	init_log(L_DEBUG);
	struct scurvy_vterm *term = term_init();
	if (!load_scurvy_config(term, NULL)) {
		return 1;
	}
	char *_argv[] = { "/usr/bin/sh", "-c", "htop", NULL };
	struct scurvy_child *child = child_spawn(term, _argv);
	int status = wayland_main(term, child);
	child_free(child);
	vterm_free(term->vterm);
	return status;
}
