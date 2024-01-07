#ifndef OLD_PHOTO
#define OLD_PHOTO
#include <stdio.h>
#include <pthread.h>
#include <gd.h>

#define OLD_PHOTO_DIR "old_photo_PAR_B"

typedef struct {
    char *image_path;                    // has the directory included
    char *image_name;                    // just the image names

    char *processed_image_path;          // path of the outputted image
} image_filename_info;

typedef struct {
    int pipe_read;
    char *out_directory;
    int count;

	gdImagePtr texture;
} thread_args;

typedef struct {
    char *image_name;
    struct timespec time_to_process;
} image_time;

typedef struct thread_output {
    image_time *image_times;

	struct timespec thread_time;
	int n_images_processed;
} thread_output;

image_filename_info *get_filenames(char *dataset_dir, int *count, char *out_dir);

#ifdef DEBUG
void print_filenames(image_filename_info*, int count);
#endif

void free_image_filenames(image_filename_info *images, int count);

void *thread_process_images(void *arg);

char *create_out_directory(char *dataset_dir); // creates output folder inside the dataset folder
void write_timings(char *dataset_dir, struct timespec total_time, thread_output **all_timings, int n_images, int n_threads);
void write_to_csv(char *dataset_dir, struct timespec total_time, int n_threads);
#endif
