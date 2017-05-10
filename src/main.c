#include <stdlib.h>
#include <stdint.h>
#include "config.h"
#include "log.h"
#include "term.h"
#include "child.h"

int wayland_main(struct scurvy_child *);

int main(int argc, char **argv) {
	init_log(L_DEBUG);
	vterm = vterm_new(80, 24);
	vterm_set_utf8(vterm, 1);
	if (!load_scurvy_config(NULL)) {
		return 1;
	}
	char *_argv[] = { "/usr/bin/bash", NULL };
	struct scurvy_child *child = child_spawn(_argv);
	int status = wayland_main(child);
	child_free(child);
	vterm_free(vterm);
	return status;
}
