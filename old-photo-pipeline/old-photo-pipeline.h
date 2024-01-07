#ifndef OLD_PHOTO
#define OLD_PHOTO
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
    char *image_path;
    char *image_name;

    char *processed_image_path;
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

    void *generic;    // output_dir or texture_filepath
} thread_args;

image_filename_info *get_filenames(char *dataset_dir, int *count, char *out_dir);
#ifdef DEBUG
void print_filenames(image_filename_info*, int count);
#endif

void free_image_filenames(image_filename_info *images, int count);
void free_image_infos(img_info *images, int count);

void *thread_contrast(void *arg);
void *thread_smooth  (void *arg);
void *thread_texture (void *arg);
void *thread_sepia   (void *arg);

char *create_out_directory(char *directory);

void write_timings(struct timespec start_time, struct timespec end_time, img_info *images, int n_images, char *dataset_dir);
void write_to_csv(struct timespec start_time, struct timespec end_time, char *dataset_dir);
#endif
