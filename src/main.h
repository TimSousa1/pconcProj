#include <stdio.h>
#include <gd.h>

#define OLD_PHOTO_DIR "old_photo_PAR_A"

typedef struct image_filenames{
    char *out_directory;
    char **filenames_directory; // has the directory included
    char **filenames;           // just the image names
    int count;
} image_filenames;

image_filenames *get_filenames(char *filepath);
int is_jpeg(char *image_name);

void print_filenames(image_filenames*);
void thread_open_images(image_filenames *image_names, int low, int high, char *out_filepath);
char *create_out_directory(char *directory);
