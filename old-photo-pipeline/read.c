#include <gd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "old-photo-pipeline.h"
#include "image-lib.h"


image_filename_info *get_filenames(char *dataset_dir, int *count, char *out_dir){
    if (!dataset_dir || !out_dir) return NULL; // if out_dir is NULL just disable the image_already_processed check
    char image_list[strlen("/image-list.txt") + strlen(dataset_dir) +1];

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
        n_files ++;
    }

#ifdef DEBUG
    printf("[INFO] found %i files!\n", n_files);
#endif

    images = calloc(n_files, sizeof(*images));
    if (!images) return NULL;

    rewind(fp);
    int filename_chars;
    int outdir_chars = strlen(out_dir);

    char *out_file;

    int n_chars_filename_full_path, n_chars_img;
    for (int i = 0; fgets(line, sizeof(line), fp); i++){
        
        n_chars_filename_full_path = strlen(line) + strlen(dataset_dir) +2;
        images[i].image_path = malloc(n_chars_filename_full_path * sizeof(char));

        if (!images[i].image_path) {
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
            free(images[i].image_name);
            free(images[i].image_path);
            i--;
            n_files--;
            continue;
        }


        snprintf(images[i].image_path, n_chars_filename_full_path, "%s/%s", dataset_dir, line);
        strcpy(images[i].image_name , line);

        filename_chars = strlen(images[i].image_name);
        out_file = malloc(outdir_chars + filename_chars +2);
        if (!out_file){
            free_image_filenames(images, n_files);
            fclose(fp);
            return NULL;
        }

        snprintf(out_file, outdir_chars + filename_chars +2, "%s/%s", out_dir, images[i].image_name);

        if (access(out_file, F_OK) == 0) {
#ifdef DEBUG
            fprintf(stdout, "[INFO] file %s already exists.. skipping..\n", out_file);
#endif
            free(out_file);
            free(images[i].image_name);
            free(images[i].image_path);
            i--;
            n_files--;
            continue;
        }
        images[i].processed_image_path = out_file;
    }

    *count = n_files;
#ifdef DEBUG
    printf("[INFO] %d files for processing\n", n_files);
#endif
    fclose(fp);
    return images;
}

// returns NULL on fail
char *create_out_directory(char *directory){
    if (!directory){
        return NULL;
    }
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
        printf("%s ", image_names[i].image_path);
    }
    printf("\n");
}
#endif

void free_image_infos(img_info *images, int count) {
	if (!images) return;

    for (int i = 0; i < count; i++){
        free(images[i].name_info.image_name);
        free(images[i].name_info.image_path);
        free(images[i].name_info.processed_image_path);
        gdImageDestroy(images[i].image);
    }
	free(images);
}

void free_image_filenames(image_filename_info *images, int count) {
	if (!images) return;

    for (int i = 0; i < count; i++){
        free(images[i].image_name);
        free(images[i].image_path);
    }

	free(images);
}
