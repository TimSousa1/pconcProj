#include <stdio.h>

typedef struct image_filenames{
    char **filenames;
    int count;
} image_filenames;

image_filenames *get_filenames(char *filepath);
void print_filenames(image_filenames*);
