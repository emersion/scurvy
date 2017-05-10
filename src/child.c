#define _XOPEN_SOURCE 600
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <libtsm.h>
#include <termbox.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>
#include <wordexp.h>
#include <termios.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <pty.h>
#include <vterm.h>
#include "term.h"
#include "log.h"
#include "child.h"

static void init_child_pty(struct scurvy_child *child) {
	if (grantpt(child->fd) < 0) {
		close(child->fd);
		goto fail;
	}
	if (unlockpt(child->fd) < 0) {
		close(child->fd);
		goto fail;
	}
	char *slave_name = ptsname(child->fd);
	if (!slave_name) {
		close(child->fd);
		goto fail;
	}
	int slave = open(slave_name, O_RDWR | O_CLOEXEC | O_NOCTTY);
	if (slave < 0) {
		close(child->fd);
		goto fail;
	}
	pid_t pid = setsid();
	if (pid < 0) {
		close(child->fd);
		goto fail_slave;
	}
	close(child->fd);
	struct termios attr;
	struct winsize ws;
	if (tcgetattr(slave, &attr) < 0) {
		goto fail_slave;
	}
	attr.c_cc[VERASE] = 010;
	if (tcsetattr(slave, TCSANOW, &attr) < 0) {
		goto fail_slave;
	}
	memset(&ws, 0, sizeof(ws));
	ws.ws_col = 80;
	ws.ws_row = 24;
	if (ioctl(slave, TIOCSWINSZ, &ws) < 0) {
		goto fail_slave;
	}
	dup2(slave, STDIN_FILENO);
	dup2(slave, STDOUT_FILENO);
	dup2(slave, STDERR_FILENO);
	ioctl(slave, TIOCSCTTY, NULL);
	close(slave);
	setenv("TERM", "xterm-256color", 1);
	return;
fail_slave:
	close(slave);
fail:
	// TODO: bubble up error
	exit(1);
}

static void init_child(struct scurvy_child *child, char **argv) {
	// Clear signals
	sigset_t sigset;
	sigemptyset(&sigset);
	if (sigprocmask(SIG_SETMASK, &sigset, NULL) < 0) {
		exit(1);
	}
	for (int i = 0; i < SIGUNUSED; ++i) {
		signal(i, SIG_DFL);
	}
	init_child_pty(child);
	execvp(argv[0], argv);
	exit(1);
}

struct scurvy_child *child_spawn(char **argv) {
	struct scurvy_child *child = calloc(sizeof(struct scurvy_child), 1);
	if (!child) {
		return NULL;
	}
	child->fd = posix_openpt(O_RDWR | O_NOCTTY | O_CLOEXEC | O_NONBLOCK);
	child->pid = fork();
	if (child->pid < 0) {
		scurvy_log(L_ERROR, "fork() failed (%d)", errno);
	} else if (!child->pid) {
		init_child(child, argv);
	}
	return child;
}

bool child_read_pty(struct scurvy_child *child) {
	static char buf[4096];
	int r = read(child->fd, &buf, sizeof(buf));
	if (r == -1) {
		if (errno != EAGAIN) {
			scurvy_log(L_DEBUG, "pty read error %d", errno);
			// TODO: Abort
			return false;
		}
	}
	if (r > 0) {
		//vterm_input_write(vterm, buf, r);
		return true;
	}
	return false;
}

void child_free(struct scurvy_child *child) {
	if (!child) {
		return;
	}
	close(child->fd);
	free(child);
}
