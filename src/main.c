#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>

#include "main.h"
#include "image-lib.h"


int main(int argc, char **argv){
    int n_threads;
    char *filepath;
    char *out_filepath;

    image_filenames *image_names;

    if (argc != 3) return 1;

    n_threads = atoi(argv[2]);
    filepath = argv[1];

    if (filepath[strlen(filepath) -1] == '/') filepath[strlen(filepath) -1] = '\0';

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
                                 //
    thread_open_images(image_names, 0, 0, out_filepath);
    return 0;
}
