#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>
#include <unistd.h>
#include <time.h>

#include <main.h>
#include <image-lib.h>


int main(int argc, char **argv){
    
	if (argc != 3) return 1;
	
	struct timespec start_time_total, end_time_total;

	clock_gettime(CLOCK_MONOTONIC, &start_time_total);

	int n_threads;
    char *filepath;

    image_filenames *image_names;

    n_threads = atoi(argv[2]);
    filepath = argv[1];

    image_names = get_filenames(filepath);

    if (!image_names) {
        fprintf(stderr, "[ERROR] no image-list.txt found!\n"); // should be printed in get_filenames()
        return 1; // MEM LEAK!
    }

#ifdef DEBUG
    printf("[INFO] got images: ");
    print_filenames(image_names);
#endif
	
	image_names->out_directory = create_out_directory(filepath);
    if (!image_names->out_directory) return 1; // MEM LEAK!

	char texture_name[] = "/paper-texture.png";

	char *texture_filepath = malloc((strlen(texture_name)+strlen(filepath)+1)*sizeof(char));

	strcpy(texture_filepath, filepath);
	strcat(texture_filepath, texture_name);
	if (access(texture_filepath, F_OK) == -1) {
		strcpy(texture_filepath, texture_name);
		if (access(texture_filepath, F_OK) == -1) {
			fprintf(stderr, "[ERROR] missing texture image!\n");
			return 1;
		}
	}

    thread_open_images(image_names, 0, 0, texture_filepath);

	clock_gettime(CLOCK_MONOTONIC, &end_time_total);

	//write_timings(diff_timespec(&end_time_total, &start_time_total), NULL, NULL, n_threads, NULL);

    return 0;
}
