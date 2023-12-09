#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <gd.h>

#include <main.h>
#include <image-lib.h>

int is_jpeg(char *filename){
    if (!filename) return 0;

    int extensionJPEG_size;
    int extensionJPG_size;

    char extensionJPEG[] = ".jpeg";
    char extensionJPG[] = ".jpg";

    extensionJPEG_size = strlen(extensionJPEG);
    extensionJPG_size = strlen(extensionJPG);

    int jpegTrue = 1;

#ifdef DEBUG
    printf("[INFO] got file %s\n", filename);
#endif

    for (long unsigned int i = strlen(filename) - extensionJPEG_size; i < strlen(filename); i++){
#ifdef DEBUG
        printf("[INFO] checking %c against %c\n", filename[i], extension[i - strlen(filename) + extension_size]);
#endif
        if (filename[i] != extensionJPEG[i - strlen(filename) + extensionJPEG_size]) {
            jpegTrue = 0;
            break;
        }
    }

    if (jpegTrue) return 1;

    for (long unsigned int i = strlen(filename) - extensionJPG_size; i < strlen(filename); i++){
#ifdef DEBUG
        printf("[INFO] checking %c against %c\n", filename[i], extension[i - strlen(filename) + extension_size]);
#endif
        if (filename[i] != extensionJPG[i - strlen(filename) + extensionJPG_size]) return 0;
    }

    return 1;
}


int image_index = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void *thread_process_images(void *arg) {

	struct timespec start_time, end_time;

	clock_gettime(CLOCK_MONOTONIC, &start_time);

	thread_resources *resources = (thread_resources *) arg;

	thread_output *ret = malloc(sizeof(thread_output));

	int n_images = resources->images->count;
	int i, images_processed = 0;
	char out_file[256];
	gdImagePtr paper_texture = resources->texture;

	gdImagePtr image;
	gdImagePtr image_tmp;

	do {
		pthread_mutex_lock(&mutex);
		i = image_index++;
		pthread_mutex_unlock(&mutex);

		if (i >= n_images) break;

		strcpy(out_file, resources->images->out_directory);
        strcat(out_file, "/");
        strcat(out_file, resources->images->filenames[i]);

		if (access(out_file, F_OK) == 0) continue;

		image = read_jpeg_file(resources->images->filenames_directory[i]);

		gdImageContrast(image, -20);
		gdImageSmooth(image, 20);

        image_tmp = image;
		image = texture_image(image_tmp, paper_texture);
        gdImageDestroy(image_tmp);

        gdImageColor(image, 100, 60, 0, 0);

		write_jpeg_file(image, out_file);
		gdImageDestroy(image);	

		images_processed++;

	} while (i < n_images);

	clock_gettime(CLOCK_MONOTONIC, &end_time);
	
	ret->time = diff_timespec(&end_time, &start_time);
	ret->n_images_processed = images_processed;

	return ret;
}
