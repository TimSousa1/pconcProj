#include <stdio.h>
#include "main.h"

int main(int argc, char **argv){
    if (argc != 3) return 1;

    image_filenames *image_names;
    image_names = get_filenames(argv[1]);

    if (!image_names) {
#ifdef DEBUG
        fprintf(stderr, "[ERROR] no image-list.txt found!\n");
#endif
        return 1;
    }

#ifdef DEBUG
    printf("[INFO] got images: ");
    print_filenames(image_names);
#endif
}
