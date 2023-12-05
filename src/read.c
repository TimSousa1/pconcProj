#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"

void print_filenames(image_filenames *image_names){
    for (int i = 0; i < image_names->count; i++){
        printf("%s ", image_names->filenames[i]);
    }
    printf("\n");
}

void free_names(char **names, int n_names){
    for (int i = 0; i < n_names; i++){
        free(names[i]);
    }
    free(names);
}

image_filenames *get_filenames(char *filepath){
    char filename[strlen("/image-list.txt") + strlen(filepath) + 1];

#ifdef DEBUG
    printf("[INFO] checking char %c\n", filepath[strlen(filepath) -1]);
#endif

    if (filepath[strlen(filepath) -1] == '/') filepath[strlen(filepath) -1] = '\0';

    strcpy(filename, filepath);
    strcat(filename, "/image-list.txt");

#ifdef DEBUG
    printf("[INFO] file list location: %s\n", filename);
#endif

    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;

    char line[256];
    char **files = NULL;

    int n_files = 0;
    while(fgets(line, sizeof(line), fp)) n_files++;

    files = malloc(n_files * sizeof(char*));
    if (!files) return NULL;

    rewind(fp);

    for (int i = 0; fgets(line, sizeof(line), fp); i++){
        files[i] = calloc((strlen(line) +1), sizeof(char));
        if (!files[i]) {
            free_names(files, i);
            return NULL;
        }
        strcpy(files[i], line);

        if (files[i][strlen(files[i])-1] == '\n'){
            files[i][strlen(files[i])-1] = '\0';
        }
    }

    image_filenames *images = malloc (sizeof(*images));
    if (!images){
        free_names(files, n_files);
        return NULL;
    }

    images->filenames = files;
    images->count = n_files;

    fclose(fp);
    return images;
}
