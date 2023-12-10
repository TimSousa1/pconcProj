#include <stdio.h>
#include <time.h>
#include <string.h>

#include "image-lib.h"

int count_digits(int number){
	
	if (number == 0) return 1;

	int digits;
	for (digits = 0; number != 0; number /= 10) digits++;

	return digits;

}

void write_timings(struct timespec total_time, struct timespec *threads_time, int n_threads, int *images_per_thread, char *filepath, int n_images) {
	
	char *file_name;
	int len = strlen("/timing_.txt") + count_digits(n_threads) + strlen(filepath) + 1;
	file_name = malloc(len * sizeof(char));

	snprintf(file_name, sizeof(char) * len, "%s/timing_%d.txt", filepath, n_threads);
	
	FILE *fp = fopen(file_name, "w");	
	if (!fp) {
#ifdef DEBUG
		fprintf(stderr, "[ERROR] Can't create %s!\n", file_name);
#endif
		return;
	}
	
	fprintf(fp, "total %d %li.%li\n", n_images, total_time.tv_sec, total_time.tv_nsec);

	for (int i = 0; i < n_threads; i++) { 
		fprintf(fp, "Thread_%d: %d %li.%li\n", i, images_per_thread[i], threads_time[i].tv_sec, threads_time[i].tv_nsec);
	}
	free(file_name);	
    fclose(fp);
}

void write_to_csv(struct timespec total_time, struct timespec *threads_time, int n_threads, int *images_per_thread, char *filepath, int n_images) {
	
	char *file_name;
	int len = strlen("/timing_.csv") + count_digits(n_threads) + strlen(filepath) + 1;
	file_name = malloc(len*sizeof(char));

	snprintf(file_name, sizeof(char) * len, "%s/timing_%d.csv", filepath, n_threads);
	
	FILE *fp = fopen(file_name, "w");	
	if (!fp){
#ifdef DEBUG
		fprintf(stderr, "[ERROR] Can't create %s!\n", file_name);
#endif
		return;
	}
	
    fprintf(fp, "threads,n_images,time\n");
	fprintf(fp, "%d,%d,%li.%li\n",n_threads, n_images, total_time.tv_sec, total_time.tv_nsec);
    return;

	for (int i = 0; i < n_threads; i++) { 
		fprintf(fp, "thread_%d: %d %li.%li\n", i, images_per_thread[i], threads_time[i].tv_sec, threads_time[i].tv_nsec);
	}
	free(file_name);	
    fclose(fp);
}
