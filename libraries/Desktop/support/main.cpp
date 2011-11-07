#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sched.h>
#include <wiring.h>
#include <getopt.h>
#include "desktop.h"

void setup(void);
void loop(void);

// the state of the desktop simulation
struct desktop_state desktop_state;

static void usage(void)
{
	printf("Options:\n");
	printf("\t-s          enable CLI slider switch\n");
}

int main(int argc, char * const argv[])
{
	int opt;

	// default state
	desktop_state.slider = false;
	gettimeofday(&desktop_state.sketch_start_time, NULL);

	while ((opt = getopt(argc, argv, "sh")) != -1) {
		switch (opt) {
		case 's':
			desktop_state.slider = true;
			break;
		default:
			usage();
			break;
		}
	}

	// run main setup() function from sketch
	setup();

	while (true) {
		struct timeval tv;
		fd_set fds;
		int fd_high = 0;

		FD_ZERO(&fds);
		loop();

		desktop_serial_select_setup(&fds, &fd_high);
		tv.tv_sec = 0;
		tv.tv_usec = 100;

		select(fd_high+1, &fds, NULL, NULL, &tv);
	}
	return 0;
}
