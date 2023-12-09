#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include <main.h>
#include <image-lib.h>


int main(int argc, char **argv){
    
	if (argc != 3) return 1;
	
	struct timespec start_time_total, end_time_total;

	clock_gettime(CLOCK_MONOTONIC, &start_time_total);

	int n_threads;
    char *filepath;
    int n_images;

    image_filenames *image_names;

    n_threads = atoi(argv[2]);
	if (n_threads <= 0) return 1;

    filepath = argv[1];
    if (filepath[strlen(filepath)-1] == '/'){
        filepath[strlen(filepath)-1] = '\0';
    }

    image_names = get_filenames(filepath);

    if (!image_names) {
        fprintf(stderr, "[ERROR] no image-list.txt found!\n"); // should be printed in get_filenames()
        return 1;
    }

#ifdef DEBUG
    printf("[INFO] got images: ");
    print_filenames(image_names);
#endif
	
    n_images = image_names->count;

    // if (n_threads > n_images) n_threads = n_images;

	image_names->out_directory = create_out_directory(filepath);
    if (!image_names->out_directory) {
		free_image_filenames(image_names);
		return 1;
	}
	char texture_name[] = "paper-texture.png";

	char *texture_filepath = malloc((strlen(texture_name) + strlen(filepath) +2) * sizeof(char));

	strcpy(texture_filepath, filepath);
    strcat(texture_filepath, "/");
	strcat(texture_filepath, texture_name);

	if (access(texture_filepath, F_OK) == -1) {
		strcpy(texture_filepath, texture_name);

		if (access(texture_filepath, F_OK) == -1) {
			fprintf(stderr, "[ERROR] missing texture image!\n");
			free_image_filenames(image_names);	
			return 1;
		}
	}

	pthread_t thread_id[n_threads];
	thread_resources resources;

	resources.images = image_names;
	resources.texture = read_png_file(texture_filepath);

	for (int i = 0; i < n_threads; i++) {
		pthread_create(&thread_id[i], NULL, &thread_process_images, &resources);
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
	free(texture_filepath);
	gdImageDestroy(resources.texture);
	
	clock_gettime(CLOCK_MONOTONIC, &end_time_total);
	struct timespec total_time = diff_timespec(&end_time_total, &start_time_total);
	
	write_timings(total_time, thread_time, n_threads, images_per_thread, filepath, n_images);

    return 0;
}
