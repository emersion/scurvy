#ifndef _SCURVY_CHILD_H
#define _SCURVY_CHILD_H
#include <sys/types.h>
#include "term.h"

struct scurvy_child {
	pid_t pid;
	int fd;
	struct scurvy_vterm *term;
};

struct scurvy_child *child_spawn(struct scurvy_vterm *term, char **argv);
void child_free(struct scurvy_child *child);
bool child_read_pty(struct scurvy_child *child);
void child_write_pty(struct scurvy_child *child);

#endif
