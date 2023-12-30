#include <stdio.h>
#include <time.h>
#include <string.h>

#include "image-lib.h"
#include "old-photo-paralelo-B.h"

int count_digits(int number){
	
	if (number == 0) return 1;

	int digits;
	for (digits = 0; number != 0; number /= 10) digits++;

	return digits;

}

void write_timings(char *dataset_dir, struct timespec total_time, thread_output **thread_profiles, int n_images, int n_threads) {
	
	char *file_name;
	int len = strlen("/timing_.txt") + count_digits(n_threads) + strlen(dataset_dir) + 1;
	file_name = malloc(len * sizeof(char));

	snprintf(file_name, sizeof(char) * len, "%s/timing_%d.txt", dataset_dir, n_threads);
	
	FILE *fp = fopen(file_name, "w");	
	if (!fp) {
#ifdef DEBUG
		fprintf(stderr, "[ERROR] Can't create %s!\n", file_name);
#endif
		return;
	}
	

	for (int i = 0; i < n_threads; i++) { 
        for (int j = 0; j < thread_profiles[i]->n_images_processed; j++){
            image_time tmp = thread_profiles[i]->image_times[j];
            fprintf(fp, "%s %li.%li\n", tmp.image_name, tmp.time_to_process.tv_sec, tmp.time_to_process.tv_nsec);
        }
	}
    for (int i = 0; i < n_threads; i++){
        fprintf(fp, "thread_%d %d %li.%li\n", i, thread_profiles[i]->n_images_processed, 
                thread_profiles[i]->thread_time.tv_sec, thread_profiles[i]->thread_time.tv_nsec);
    }
	fprintf(fp, "total %d %li.%li\n", n_images, total_time.tv_sec, total_time.tv_nsec);
	free(file_name);	
    fclose(fp);
}


void write_to_csv(char *dataset_dir, struct timespec total_time, int n_threads) {
	char *file_name;
	int len = strlen("/timing_.csv") + count_digits(n_threads) + strlen(dataset_dir) + 1;
	file_name = malloc(len * sizeof(char));

	snprintf(file_name, sizeof(char) * len, "%s/timing_%d.csv", dataset_dir, n_threads);
	
	FILE *fp = fopen(file_name, "w");	
	if (!fp) {
#ifdef DEBUG
		fprintf(stderr, "[ERROR] Can't create %s!\n", file_name);
#endif
		return;
	}
    fprintf(fp, "n_threads,time\n");
	fprintf(fp, "%d,%li.%li\n", n_threads, total_time.tv_sec, total_time.tv_nsec);

	free(file_name);	
    fclose(fp);
}
