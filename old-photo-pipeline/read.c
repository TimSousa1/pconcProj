#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "old-photo-pipeline.h"
#include "image-lib.h"

void free_names(char **names, int n_names);


image_filename_info *get_filenames(char *dataset_dir, int *count){
    if (!dataset_dir) return NULL;
    char image_list[strlen("/image-list.txt") + strlen(dataset_dir) +1];

#ifdef DEBUG
    printf("[INFO] checking char %c\n", dataset_dir[strlen(dataset_dir) -1]);
#endif

    strcpy(image_list, dataset_dir);
    strcat(image_list, "/image-list.txt");

#ifdef DEBUG
    printf("[INFO] file list location: %s\n", image_list);
#endif

    FILE *fp = fopen(image_list, "r");
    if (!fp) {
        fprintf(stderr, "[ERROR] no image-list.txt found!\n");
        return NULL;
    }

    char line[256];
    
    image_filename_info *images = NULL;

    int n_files = 0;
    while(fgets(line, sizeof(line), fp)) {
        if (line[strlen(line)-1] == '\n'){
            line[strlen(line)-1] = '\0';
        }
        n_files += is_jpeg(line);
    }

    *count = n_files;

#ifdef DEBUG
    printf("[INFO] got %i files!\n", n_files);
#endif

    images = calloc(n_files, sizeof(*images));
    if (!images) return NULL;

    rewind(fp);

    int n_chars_dir, n_chars_img;
    for (int i = 0; fgets(line, sizeof(line), fp); i++){
        
        n_chars_dir = strlen(line) + strlen(dataset_dir) +2;
        images[i].filename_full_path = malloc(n_chars_dir * sizeof(char));

        if (!images[i].filename_full_path) {
            free_image_filenames(images, n_files);
            fclose(fp);
            return NULL;
        }
        n_chars_img = strlen(line) +1;
        images[i].image_name = malloc(n_chars_img * sizeof(char));
        if (!images[i].image_name) {
            free_image_filenames(images, n_files);
            fclose(fp);
            return NULL;
        }

        if (line[strlen(line)-1] == '\n'){
            line[strlen(line)-1] = '\0';
        }

        if (!is_jpeg(line)){
            i--;
            continue;
        }

        snprintf(images[i].filename_full_path, n_chars_dir, "%s/%s", dataset_dir, line);
        strcpy(images[i].image_name , line);

    }

    fclose(fp);
    return images;
}

// returns NULL on fail
char *create_out_directory(char *directory){
    char *out_filepath;
    int n_chars;

    n_chars = strlen(directory) + strlen(OLD_PHOTO_DIR) +2;
    out_filepath = malloc(n_chars * sizeof(char));
    if (!out_filepath) return NULL;

    snprintf(out_filepath, n_chars, "%s/%s", directory, OLD_PHOTO_DIR);
    if (create_directory(out_filepath) == 0){
        fprintf(stderr, "[ERROR] Couldn't create output directory\n");
        free(out_filepath);
        out_filepath = NULL;
    }
    return out_filepath;
}

#ifdef DEBUG
void print_filenames(image_filename_info *image_names, int count){
    if (!image_names) return;
    for (int i = 0; i < count; i++){
        printf("%s ", image_names[i].filename_full_path);
    }
    printf("\n");
}
#endif


void free_image_filenames(image_filename_info *images, int count) {
	if (!images) return;

    for (int i = 0; i < count; i++){
        free(images[i].image_name);
        free(images[i].filename_full_path);
    }

	free(images);
}
