#include <stdio.h>
#include <time.h>
#include <string.h>

#include "image-lib.h"
#include "old-photo-pipeline.h"

int count_digits(int number){
	
	if (number == 0) return 1;

	int digits;
	for (digits = 0; number != 0; number /= 10) digits++;

	return digits;

}

void write_timings(struct timespec start_time, struct timespec end_time, img_info *images, int n_images, char *dataset_dir) {
	
	char *timings_filename_full_path;
	int len = strlen("/timing_pipeline.txt") + strlen(dataset_dir) + 1;

	timings_filename_full_path = malloc(len * sizeof(char));
	snprintf(timings_filename_full_path, sizeof(char) * len, "%s/timing_pipeline.txt", dataset_dir);
	
	FILE *fp = fopen(timings_filename_full_path, "w");
	if (!fp) {
#ifdef DEBUG
		fprintf(stderr, "[ERROR] Can't create %s!\n", timings_filename_full_path);
#endif
		return;
	}
	
    for (int i = 0; i < n_images; i++) { 
        struct timespec start = diff_timespec(&images[i].processing_start, &start_time);
        struct timespec end = diff_timespec(&images[i].processing_end, &start_time);

        fprintf(fp, "%s start %li.%li\n", images[i].name_info.image_name, start.tv_sec, start.tv_nsec);
        fprintf(fp, "%s end %li.%li\n", images[i].name_info.image_name, end.tv_sec, end.tv_nsec);
    }
    struct timespec total = diff_timespec(&end_time, &start_time);
	fprintf(fp, "total %li.%li\n", total.tv_sec, total.tv_nsec);
	free(timings_filename_full_path);	
    fclose(fp);
}
