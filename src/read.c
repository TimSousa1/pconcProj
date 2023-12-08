#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <main.h>
#include <image-lib.h>

char *create_out_directory(char *directory){
    char *out_filepath;
    out_filepath = malloc((strlen(directory) + strlen(OLD_PHOTO_DIR) +2) * sizeof(char));
    if (!out_filepath) return NULL;

    strcpy(out_filepath, directory);
    strcat(out_filepath, "/");
    strcat(out_filepath, OLD_PHOTO_DIR);

    create_directory(out_filepath);
    
    return out_filepath;
}

void print_filenames(image_filenames *image_names){
    if (!image_names) return;
    for (int i = 0; i < image_names->count; i++){
        printf("%s ", image_names->filenames_directory[i]);
    }
    printf("\n");
}

void free_names(char **names, int n_names){
    if (!names) return;
    for (int i = 0; i < n_names; i++){
        free(names[i]);
    }
    free(names);
}

void free_image_filenames(image_filenames *images) {
	if (!images) return;
	free_names(images->filenames, images->count);
	free_names(images->filenames_directory, images->count);
	free(images->out_directory);
	free(images);
}

// filepath without final '/'
image_filenames *get_filenames(char *filepath){
    if (!filepath) return NULL;
    char filename[strlen("/image-list.txt") + strlen(filepath) + 1];

#ifdef DEBUG
    printf("[INFO] checking char %c\n", filepath[strlen(filepath) -1]);
#endif

    strcpy(filename, filepath);
    strcat(filename, "/image-list.txt");

#ifdef DEBUG
    printf("[INFO] file list location: %s\n", filename);
#endif

    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;

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
    if (!image_names) return NULL;

    rewind(fp);

    for (int i = 0; fgets(line, sizeof(line), fp); i++){
        files_directory[i] = malloc((strlen(line)+strlen(filepath)+2)*sizeof(char));
        if (!files_directory[i]) {
            free_names(files_directory, i-1);
            free_names(image_names, i-1);
            return NULL;
        }
        image_names[i] = malloc((strlen(line)+1)*sizeof(char));
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

        strcpy(files_directory[i], filepath);
        strcat(files_directory[i], "/");
        strcat(files_directory[i], line);

        strcpy(image_names[i], line);
    }

    image_filenames *images = malloc (sizeof(*images));
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

