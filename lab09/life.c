// declare feature test macros to enable certain functions (sigaction, usleep)
#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <signal.h>

typedef struct {
	int cell_start;
	int cell_end;

	char **foreground;
	char **background;
} thread_args;

void signal_handler(int signo) {};

void* thread_func(void *args){
	thread_args *t_args = (thread_args *)args;

	while (true) {
		pause();

		for (int i = t_args->cell_start; i < t_args->cell_end; ++i) {
			int row = i / grid_height;
			int col = i % grid_width;

			(*t_args->background)[i] = is_alive(row, col, *t_args->foreground);
		}
	}
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("Usage: %s <threads number>\n", argv[0]);
		return 1;
	}

	int threads = atoi(argv[1]);

	pthread_t thread[threads];
	thread_args args[threads];
	
	int cells_per_thread = (int)(grid_width * grid_height / threads);

	struct sigaction sa;
	sa.sa_handler = signal_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGUSR1, &sa, NULL);


	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	for (int i = 0; i < threads; i++) {
		args[i].cell_start = i * cells_per_thread;
		args[i].cell_end = (i == threads - 1) ? grid_width * grid_height : (i + 1) * cells_per_thread;

		args[i].foreground = &foreground;
		args[i].background = &background;

		pthread_create(&thread[i], NULL, thread_func, &args[i]);
	}

	init_grid(foreground);

	while (true)
	{
		draw_grid(foreground);
		
		for (int i = 0; i < threads; i++) {
      pthread_kill(thread[i], SIGUSR1);
    }
		
		usleep(500 * 1000);

		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
