#include <stdio.h>
#include <pthread.h>
#include <gd.h>

#define OLD_PHOTO_DIR "old_photo_PIPELINE"

typedef enum {
    READ, WRITE
} mode;

typedef enum {
    contrast, smooth, texture, sepia
} stage;

typedef struct {
    char *filename_full_path;                  // has the directory included
    char *image_name;                          // just the image names

    char *processed_image_filename_full_path;  // output image name
} image_filename_info;

typedef struct {
    gdImagePtr image;
    image_filename_info name_info;

    struct timespec processing_start;
    struct timespec processing_end;
} img_info;

typedef struct {
    int pipe_read;
    int pipe_write;

    char *generic;    // output_dir or texture_filepath
} thread_args;

typedef struct thread_output {
	struct timespec time;
	int n_images_processed;
} thread_output;

image_filename_info *get_filenames(char *dataset_dir, int *count, char *out_dir);
#ifdef DEBUG
void print_filenames(image_filename_info*, int count);
#endif
int is_jpeg(char *image_name);

void free_image_filenames(image_filename_info *images, int count);
void free_image_infos(img_info *images, int count);

void *thread_process_images(void *arg);

void *thread_contrast(void *arg);
void *thread_smooth(void *arg);
void *thread_texture(void *arg);
void *thread_sepia(void *arg);

char *create_out_directory(char *directory);
void write_timings(struct timespec start_time, struct timespec end_time, img_info *images, int n_images, char *dataset_dir);
void write_to_csv(struct timespec total_time, int n_threads, char *filepath);
