#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <main.h>
#include <image-lib.h>

void free_names(char **names, int n_names);

// filepath without final '/'
image_filenames *get_filenames(char *filepath){
    if (!filepath) return NULL;
    char filename[strlen("/image-list.txt") + strlen(filepath) +1];

#ifdef DEBUG
    printf("[INFO] checking char %c\n", filepath[strlen(filepath) -1]);
#endif

    strcpy(filename, filepath);
    strcat(filename, "/image-list.txt");

#ifdef DEBUG
    printf("[INFO] file list location: %s\n", filename);
#endif

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "[ERROR] no image-list.txt found!\n");
        return NULL;
    }

    char line[256];
    char **files_directory = NULL;
    char **image_names = NULL;

    int n_files = 0;
    while(fgets(line, sizeof(line), fp)) {
        if (line[strlen(line)-1] == '\n'){
            line[strlen(line)-1] = '\0';
        }
        n_files += is_jpeg(line);
    }

#ifdef DEBUG
    printf("[INFO] got %i files!\n", n_files);
#endif

    files_directory = malloc(n_files * sizeof(char*));
    if (!files_directory) return NULL;

    image_names = malloc(n_files * sizeof(char*));
    if (!image_names) {
        free(files_directory);
        return NULL;
    }

    rewind(fp);

    int n_chars_dir, n_chars_img;
    for (int i = 0; fgets(line, sizeof(line), fp); i++){
        
        n_chars_dir = strlen(line) + strlen(filepath) +2;
        files_directory[i] = malloc(n_chars_dir * sizeof(char));

        if (!files_directory[i]) {
            free_names(files_directory, i-1);
            free_names(image_names, i-1);
            return NULL;
        }
        n_chars_img = strlen(line) +1;
        image_names[i] = malloc(n_chars_img * sizeof(char));
        if (!image_names[i]) {
            free_names(image_names, i-1);
            free_names(files_directory, i);
            return NULL;
        }

        if (line[strlen(line)-1] == '\n'){
            line[strlen(line)-1] = '\0';
        }

        if (!is_jpeg(line)){
            i--;
            continue;
        }

        snprintf(files_directory[i], n_chars_dir, "%s/%s", filepath, line);
        strcpy(image_names[i], line);
    }

    image_filenames *images;
    images = malloc (sizeof(*images));

    if (!images){
        free_names(files_directory, n_files);
        return NULL;
    }

    images->out_directory = NULL;
    images->filenames_directory = files_directory;
    images->filenames = image_names;
    images->count = n_files;

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
void print_filenames(image_filenames *image_names){
    if (!image_names) return;
    for (int i = 0; i < image_names->count; i++){
        printf("%s ", image_names->filenames_directory[i]);
    }
    printf("\n");
}
#endif

// deletes all members of array till n_names (inclusive)
void free_names(char **names, int n_names){
    if (!names) return;

    for (int i = 0; i <= n_names; i++){
        free(names[i]);
    }
    free(names);
}

void free_image_filenames(image_filenames *images) {
	if (!images) return;

	free_names(images->filenames, images->count-1);
	free_names(images->filenames_directory, images->count-1);

	free(images->out_directory);
	free(images);
}

