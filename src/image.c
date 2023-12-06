#include <stdio.h>
#include <stdlib.h>
#include <gd.h>
#include <string.h>
#include <unistd.h>
#include "main.h"
#include "image-lib.h"

int is_jpeg(char *filename){
    if (!filename) return 0;

    int extension_size;
    char extension[] = ".jpeg";
    extension_size = strlen(extension);

#ifdef DEBUG
    printf("[INFO] got file %s\n", filename);
#endif

    for (int i = strlen(filename) - extension_size; i < strlen(filename); i++){
#ifdef DEBUG
        printf("[INFO] checking %c against %c\n", filename[i], extension[i - strlen(filename) + extension_size]);
#endif
        if (filename[i] != extension[i - strlen(filename) + extension_size]) return 0;
    }
    return 1;
}

gdImagePtr *open_images(image_filenames *image_names, int low, int high);

void thread_open_images(image_filenames *image_names, int low, int high, char *out_filepath){
    gdImagePtr *images;
    int n_images;
    char out_file[256];

    n_images = image_names->count;
    images = open_images(image_names, low, high);
    for (int i = 0; i < n_images; i++){
        gdImageContrast(images[i], -20);
    }

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
        write_jpeg_file(images[i], out_file);
    }
}

gdImagePtr *open_images(image_filenames *image_names, int low, int high){
    int n_images;
    gdImagePtr *images, tmp;

    n_images = image_names->count;
    images = malloc (n_images * sizeof(gdImagePtr));
    if (!images) return NULL;

    for (int i = 0; i < n_images; i++){
        images[i] = read_jpeg_file(image_names->filenames_directory[i]);
    }
    return images;
}
