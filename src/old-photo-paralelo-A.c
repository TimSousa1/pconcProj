#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "old-photo-paralelo-A.h"
#include "image-lib.h"


pthread_mutex_t lock;

int main(int argc, char **argv){
    
	if (argc != 3) return 1;
	
	struct timespec start_time_total, end_time_total;

	clock_gettime(CLOCK_MONOTONIC, &start_time_total);

	int n_threads;
    int n_images;
    char *filepath;

    image_filenames *image_names;

    n_threads = atoi(argv[2]);
	if (n_threads <= 0) return 1;

    filepath = argv[1];
    image_names = get_filenames(filepath);

    if (!image_names) return 1;

#ifdef DEBUG
    printf("[INFO] got images: ");
    print_filenames(image_names);
#endif
	
    n_images = image_names->count;

	image_names->out_directory = create_out_directory(filepath);
    if (!image_names->out_directory) {
		free_image_filenames(image_names);
		return 1;
	}

	char texture_name[] = "paper-texture.png";
    char *texture_filepath;

	pthread_t thread_id[n_threads];
	thread_args args;

	args.images = image_names;

	args.texture = read_png_file(texture_name);
    if (!args.texture) {
        int n_chars;

        n_chars = strlen(texture_name) + strlen(filepath) +2;
        texture_filepath = malloc(n_chars * sizeof(char));

        snprintf(texture_filepath, n_chars, "%s/%s", filepath, texture_name);
        args.texture = read_png_file(texture_filepath);

        free(texture_filepath);
    }

    if (!args.texture){
			fprintf(stderr, "[ERROR] Missing texture image!\n");
			free_image_filenames(image_names);	
			return 1;
    }

    if (pthread_mutex_init(&lock, NULL) != 0){
        fprintf(stderr, "[ERROR] Couldn't create mutex\n");

        free(image_names);
        gdImageDestroy(args.texture);
        return 1;
    }
    
    args.lock = &lock;
	for (int i = 0; i < n_threads; i++) {
		pthread_create(&thread_id[i], NULL, &thread_process_images, &args);
	}

	void *ret;
	thread_output *output;

	struct timespec thread_time[n_threads];

	int images_per_thread[n_threads];
	for (int i = 0; i < n_threads; i++) {
		pthread_join(thread_id[i], &ret);
		output = (thread_output *) ret;

		thread_time[i] = output->time;
		images_per_thread[i] = output->n_images_processed;
		free(output);
	}

	free_image_filenames(image_names);	
	gdImageDestroy(args.texture);
	
	clock_gettime(CLOCK_MONOTONIC, &end_time_total);
	struct timespec total_time = diff_timespec(&end_time_total, &start_time_total);
	
	write_timings(total_time, thread_time, n_threads, images_per_thread, filepath, n_images);

    return 0;
}
