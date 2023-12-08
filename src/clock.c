#include <stdio.h>
#include <time.h>
#include <string.h>

#include "image-lib.h"

int count_digits(int number){
	
	if (number == 0) return 1;
	int digits;
	for (digits = 0; number != 0; number /= 10, digits++);
	return digits;

}

void write_timings(struct timespec total_time, struct timespec *threads_time,
		int n_threads, int *images_per_thread, char *filepath) {
	
	char *file_name;
	int len = strlen("/timings_.txt") + count_digits(n_threads) + strlen(filepath) + 1;
	file_name = malloc(len*sizeof(char));

	snprintf(file_name, sizeof(char) * len, "%s/timings_%d.txt", filepath, n_threads);
	
	FILE *fp = fopen(file_name, "w");	
	if (!fp) {
#ifdef DEBUG
		fprintf(stderr, "[ERROR] Cant create %s!\n", file_name);
#endif
		return;
	}
	
	fprintf(fp, "Total Time: %d %li.%li\n", n_threads, total_time.tv_sec, total_time.tv_nsec);

	for (int i = 0; i < n_threads; i++) { 
		fprintf(fp, "Thread_%d: %d %li.%li\n", i, images_per_thread[i], threads_time[i].tv_sec, threads_time[i].tv_nsec);
	}
	free(file_name);	
}
