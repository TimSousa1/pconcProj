#include <stdio.h>
#include <gd.h>

#define OLD_PHOTO_DIR "old_photo_PAR_A"

typedef struct image_filenames{
    char *out_directory;
    char **filenames_directory; // has the directory included
    char **filenames;           // just the image names
    int count;
} image_filenames;

typedef struct thread_resources {
	image_filenames *images;
	gdImagePtr texture;
} thread_resources;

typedef struct thread_output {
	struct timespec time;
	int n_images_processed;
} thread_output;

image_filenames *get_filenames(char *filepath);
void print_filenames(image_filenames*);
int is_jpeg(char *image_name);

void free_image_filenames(image_filenames *images);

void *thread_process_images(void *arg);

char *create_out_directory(char *directory);
void write_timings(struct timespec total_time, struct timespec *threads_time, int n_threads, int *images_per_thread, char *filepath, int n_images);
void write_to_csv(struct timespec total_time, struct timespec *threads_time, int n_threads, int *images_per_thread, char *filepath, int n_images);
