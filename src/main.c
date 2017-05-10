#include <stdlib.h>
#include <stdint.h>
#include "config.h"
#include "log.h"
#include "term.h"

struct VTerm *vterm;

int wayland_main();

int main(int argc, char **argv) {
	init_log(L_DEBUG);
	vterm = vterm_new(80, 24);
	if (!load_scurvy_config(NULL)) {
		return 1;
	}
	int status = wayland_main();
	vterm_free(vterm);
	return status;
}
