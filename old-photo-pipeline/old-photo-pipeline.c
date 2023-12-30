#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "old-photo-pipeline.h"
#include "image-lib.h"

#define N_THREADS 4

int main(int argc, char **argv){

	if (argc != 2) return 1;
	
	struct timespec start_time_total, end_time_total;
	clock_gettime(CLOCK_MONOTONIC, &start_time_total);

    int count;
    char *dataset_dir = argv[1];
    char *out_dir = NULL;

	out_dir = create_out_directory(dataset_dir);
    if (!out_dir) {
		return 1;
	}

    image_filename_info *image_names; // array of images
    image_names = get_filenames(dataset_dir, &count, out_dir);

    if (!image_names || count < 1) {
        free(image_names);
        free(out_dir);
        return 1;
    }

#ifdef DEBUG
    printf("[INFO] got images: ");
    print_filenames(image_names, count);
#endif
	

	char texture_name[] = "paper-texture.png";
    char *texture_filepath = NULL;

	pthread_t thread_id[N_THREADS];

    thread_args args[N_THREADS]; 
    int pipes[N_THREADS][2];

	for (int i = 0; i < N_THREADS; i++){
		pipe(pipes[i]);
	}

    for (int i = 0; i < N_THREADS; i++){
        args[i].pipe_read = pipes[i][READ];
        args[i].generic = NULL;
        if (i+1 >= N_THREADS) break;
        args[i].pipe_write = pipes[i+1][WRITE];
    }

    char *texture_filename = texture_name;

    if (access(texture_name, F_OK) == -1) {
        int n_chars;

        n_chars = strlen(texture_name) + strlen(dataset_dir) +2;
        texture_filepath = malloc(n_chars * sizeof(char));

        snprintf(texture_filepath, n_chars, "%s/%s", dataset_dir, texture_name);

        if (access(texture_filepath, F_OK) == -1){
			fprintf(stderr, "[ERROR] Missing texture image!\n");
            free(texture_filepath);
            free(out_dir);
			free_image_filenames(image_names, count);	
			return 1;
        }

        texture_filename = texture_filepath;
    }

    args[contrast].generic = (void *) out_dir;
    pthread_create(&thread_id[contrast], NULL, thread_contrast, &args[contrast]);

    pthread_create(&thread_id[smooth], NULL, thread_smooth, &args[smooth]);

    args[texture].generic = (void *) texture_filename;
    pthread_create(&thread_id[texture], NULL, thread_texture, &args[texture]);

    args[sepia].generic = (void*) &count;
    pthread_create(&thread_id[sepia], NULL, thread_sepia, &args[sepia]);

    for (int i = 0; i < count; i++){
        write(pipes[contrast][WRITE], &image_names[i], sizeof(*image_names));
    }

    close(pipes[contrast][WRITE]);


	for (int i = 0; i < N_THREADS-1; i++) {
		pthread_join(thread_id[i], NULL);
	}

	void *ret;
	img_info *output;
    pthread_join(thread_id[sepia], &ret);
    output = (img_info *) ret;

    free(out_dir);
    free(texture_filepath);
	
    clock_gettime(CLOCK_MONOTONIC, &end_time_total);

    write_timings(start_time_total, end_time_total, output, count, dataset_dir);
    write_to_csv(start_time_total, end_time_total, dataset_dir);

	free_image_infos(output, count);	
    free(image_names);
    return 0;
}
