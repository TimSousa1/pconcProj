#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "old-photo-paralelo-B.h"
#include "image-lib.h"


int main(int argc, char **argv){
    
	if (argc != 3) return 1;
	
	struct timespec start_time_total, end_time_total;

	clock_gettime(CLOCK_MONOTONIC, &start_time_total);

	int n_threads;
    int count;
    char *dataset_dir = NULL;
    char *out_dir = NULL;

    image_filename_info *image_names; // array of images

    n_threads = atoi(argv[2]);
	if (n_threads <= 0) return 1;

    dataset_dir = argv[1];

	out_dir = create_out_directory(dataset_dir);
    if (!out_dir) {
		return 1;
	}

    image_names = get_filenames(dataset_dir, &count, out_dir);

    if (!image_names || count < 1) {
#ifdef DEBUG
        printf("[INFO] all images already processed.. exiting..\n");
#endif
        free(out_dir);
        return 1;
    }

#ifdef DEBUG
    printf("[INFO] got images: ");
    print_filenames(image_names, count);
#endif
	

	char texture_name[] = "paper-texture.png";
    char *texture_filepath;

	pthread_t thread_id[n_threads];
	thread_args args;

    args.out_directory = out_dir;
    args.count = count;

	args.texture = read_png_file(texture_name);
    if (!args.texture) {
        int n_chars;

        n_chars = strlen(texture_name) + strlen(dataset_dir) +2;
        texture_filepath = malloc(n_chars * sizeof(char));

        snprintf(texture_filepath, n_chars, "%s/%s", dataset_dir, texture_name);
        args.texture = read_png_file(texture_filepath);

        free(texture_filepath);
    }

    if (!args.texture){
			fprintf(stderr, "[ERROR] Missing texture image!\n");
			free_image_filenames(image_names, count);	
            free(out_dir);
			return 1;
    }

    int pipe_fd[2];
    if (pipe(pipe_fd) != 0){
        free_image_filenames(image_names, count);
        free(out_dir);
        exit(1);
    }

    args.pipe_read = pipe_fd[0];

	for (int i = 0; i < n_threads; i++) {
		pthread_create(&thread_id[i], NULL, &thread_process_images, &args);
	}

    for (int i = 0; i < count; i++){
        write(pipe_fd[1], &image_names[i], sizeof(*image_names));
    }
    close(pipe_fd[1]);

	void *ret;
	thread_output **outputs = malloc(n_threads * sizeof(*outputs)); // avoiding VLA

	for (int i = 0; i < n_threads; i++) {
		pthread_join(thread_id[i], &ret);
		outputs[i] = (thread_output *) ret;
	}

    free(out_dir);
	gdImageDestroy(args.texture);
	
	clock_gettime(CLOCK_MONOTONIC, &end_time_total);
	struct timespec total_time = diff_timespec(&end_time_total, &start_time_total);
	
	write_timings(dataset_dir, total_time, outputs, count, n_threads);

	free_image_filenames(image_names, count);	

    for (int i = 0; i < n_threads; i++){
        free(outputs[i]->image_times);
        free(outputs[i]);
    }
    free(outputs);
    return 0;
}
