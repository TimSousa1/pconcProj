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

    int extension_size;
    char extension[] = ".jpeg";
    extension_size = strlen(extension);

#ifdef DEBUG
    printf("[INFO] got file %s\n", filename);
#endif

    for (long unsigned int i = strlen(filename) - extension_size; i < strlen(filename); i++){
#ifdef DEBUG
        printf("[INFO] checking %c against %c\n", filename[i], extension[i - strlen(filename) + extension_size]);
#endif
        if (filename[i] != extension[i - strlen(filename) + extension_size]) return 0;
    }
    return 1;
}

static int image_index = 0;

gdImagePtr *open_images(image_filenames *image_names, int low, int high);

void thread_open_images(image_filenames *image_names, int low, int high, char *texture_filepath){

	gdImagePtr *images, paper_texture;
    int n_images;
    char out_file[256];

    n_images = image_names->count;
    images = open_images(image_names, low, high);
	paper_texture = read_png_file(texture_filepath);

	for (int i = 0; i < n_images; i++){
        strcpy(out_file, image_names->out_directory);
        strcat(out_file, "/");
        strcat(out_file, image_names->filenames[i]);

        if (access(out_file, F_OK) == 0){
#ifdef DEBUG
            printf("file %s already exists!\n", out_file);
#endif
            continue;
        }

		gdImageContrast(images[i], -20);
		gdImageSmooth(images[i], 20);
		images[i] = texture_image(images[i], paper_texture);
		gdImageColor(images[i], 100, 60, 0, 0);

		write_jpeg_file(images[i], out_file);
		gdImageDestroy(images[i]);	
    }
}

gdImagePtr *open_images(image_filenames *image_names, int low, int high){
    int n_images;
    gdImagePtr *images;

    n_images = image_names->count;
    images = malloc (n_images * sizeof(gdImagePtr));
    if (!images) return NULL;

    for (int i = 0; i < n_images; i++){
        images[i] = read_jpeg_file(image_names->filenames_directory[i]);
    }
    return images;
}

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

	do {

		i = image_index++;
		if (i >= n_images) break;
	
		strcpy(out_file, resources->images->out_directory);
        strcat(out_file, "/");
        strcat(out_file, resources->images->filenames[i]);
		if (access(out_file, F_OK) == 0) continue;

		image = read_jpeg_file(resources->images->filenames_directory[i]);
		gdImageContrast(image, -20);
		gdImageSmooth(image, 20);
		image = texture_image(image, paper_texture);
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
