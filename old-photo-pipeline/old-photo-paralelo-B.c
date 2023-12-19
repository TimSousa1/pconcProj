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
    int n_images;
    int count;
    char *filepath = NULL;
    char *out_directory = NULL;

    image_filenames *image_names; // array of images

    n_threads = atoi(argv[2]);
	if (n_threads <= 0) return 1;

    filepath = argv[1];
    image_names = get_filenames(filepath, &count);

    if (!image_names) return 1;

#ifdef DEBUG
    printf("[INFO] got images: ");
    print_filenames(image_names, count);
#endif
	
    n_images = count;

	out_directory = create_out_directory(filepath);
    if (!out_directory) {
		free_image_filenames(image_names, count);
		return 1;
	}

	char texture_name[] = "paper-texture.png";
    char *texture_filepath = NULL;

	pthread_t thread_id[n_threads];

    thread_args args[n_threads]; 
    int pipes[n_threads][2];

	for (int i = 0; i < n_threads; i++){
		pipe(pipes[i]);
	}

    for (int i = 0; i < n_threads; i++){
        args[i].pipe_read = pipes[i][READ];
        args[i].generic = NULL;
        if (i+1 >= n_threads) break;
        args[i].pipe_write = pipes[i+1][WRITE];
    }

    char *texture_filename = texture_name;

    if (access(texture_name, F_OK) == -1) {
        int n_chars;

        n_chars = strlen(texture_name) + strlen(filepath) +2;
        texture_filepath = malloc(n_chars * sizeof(char));

        snprintf(texture_filepath, n_chars, "%s/%s", filepath, texture_name);

        if (access(texture_filepath, F_OK) == -1){
			fprintf(stderr, "[ERROR] Missing texture image!\n");
            free(texture_filepath);
			free_image_filenames(image_names, count);	
			return 1;
        }

        texture_filename = texture_filepath;
    }

    args[contrast].generic = out_directory;
    pthread_create(&thread_id[contrast], NULL, thread_contrast, &args[contrast]);

    pthread_create(&thread_id[smooth], NULL, thread_smooth, &args[smooth]);

    args[texture].generic = texture_filename;
    pthread_create(&thread_id[texture], NULL, thread_texture, &args[texture]);

    args[sepia].generic = out_directory;
    pthread_create(&thread_id[sepia], NULL, thread_sepia, &args[sepia]);

    for (int i = 0; i < count; i++){
        write(pipes[contrast][WRITE], &image_names[i], sizeof(*image_names));
    }

    close(pipes[contrast][WRITE]);

	void *ret;
	thread_output *output;

	struct timespec thread_time[n_threads];

	int images_per_thread[n_threads];
	for (int i = 0; i < n_threads; i++) {
		pthread_join(thread_id[i], &ret);
		output = (thread_output *) ret;

//		thread_time[i] = output->time;
//		images_per_thread[i] = output->n_images_processed;
		free(output);
	}

	free_image_filenames(image_names, count);	
    free(texture_filepath);
    free(out_directory);
	
//	clock_gettime(CLOCK_MONOTONIC, &end_time_total);
//	struct timespec total_time = diff_timespec(&end_time_total, &start_time_total);
	
//	write_timings(total_time, thread_time, n_threads, images_per_thread, filepath, n_images);

    return 0;
}
