#include <bits/time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <gd.h>

#include "old-photo-paralelo-B.h"
#include "image-lib.h"

int check_extension(const char *filename, const char *extension){
    const int filenameLength = strlen(filename);
    const int extensionLength = strlen(extension);

    const int length_diff = filenameLength - extensionLength;

    for (int i = length_diff; i < filenameLength; i++){
        if (filename[i] != extension[i - filenameLength + extensionLength]) {
#ifdef DEBUG
            printf("[INFO] file %s is NOT a %s\n", filename, extension);
#endif
            return 0;
        }
    }
#ifdef DEBUG
            printf("[INFO] file %s is a %s\n", filename, extension);
#endif
    return 1;
}

int is_jpeg(const char *image_name){
    if (!image_name) return 0;
    return (check_extension(image_name, ".jpeg") || check_extension(image_name, ".jpg"));
}


int image_index = 0;


void *thread_process_images(void *arg) {

	struct timespec start_time, end_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);

    int images_processed;

	gdImagePtr paper_texture;

	thread_output *ret = malloc(sizeof(thread_output));
    if (!ret) {
        fprintf(stderr, "[ERROR] Couldn't alloc return value\n");
        return NULL;
    }

	thread_args *args = (thread_args *) arg;
    int n_images = args->count;

	images_processed = 0;

	paper_texture = args->texture;

	gdImagePtr image;
	gdImagePtr image_tmp;

    image_filename_info image_name;

    ret->image_times = malloc (n_images * sizeof(*ret->image_times));

    struct timespec image_start, image_end;
	for (int i = 0; read(args->pipe_read, &image_name, sizeof(image_name)) > 0 && i < n_images; i++){

        clock_gettime(CLOCK_MONOTONIC, &image_start);

		image = read_jpeg_file(image_name.image_path);
        if (!image) {
            free(ret->image_times);
            free(ret);
            return NULL;
        }

		gdImageContrast(image, -20);
		gdImageSmooth(image, 20);

        image_tmp = image;
		image = texture_image(image_tmp, paper_texture);
        gdImageDestroy(image_tmp);

        gdImageColor(image, 100, 60, 0, 0);

		if (write_jpeg_file(image, image_name.processed_image_path) == 0){
            fprintf(stderr, "[ERROR] Couldn't save image!\n");
        } gdImageDestroy(image);	

        clock_gettime(CLOCK_MONOTONIC, &image_end);

        ret->image_times[i].time_to_process = diff_timespec(&image_end, &image_start);
        ret->image_times[i].image_name = image_name.image_name;
		images_processed++;
	}

	clock_gettime(CLOCK_MONOTONIC, &end_time);
	
	ret->thread_time = diff_timespec(&end_time, &start_time);
	ret->n_images_processed = images_processed;

	return ret;
}
