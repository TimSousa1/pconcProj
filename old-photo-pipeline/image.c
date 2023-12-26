#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <gd.h>

#include "old-photo-pipeline.h"
#include "image-lib.h"

// move to header file for times

int is_jpeg(char *image_name){
    if (!image_name) return 0;

    int extensionJPEG_size;
    int extensionJPG_size;

    char extensionJPEG[] = ".jpeg";
    char extensionJPG[] = ".jpg";

    extensionJPEG_size = strlen(extensionJPEG);
    extensionJPG_size = strlen(extensionJPG);

    int jpegTrue = 1;

#ifdef DEBUG
    printf("[INFO] got file %s\n", image_name);
#endif

    for (long unsigned int i = strlen(image_name) - extensionJPEG_size; i < strlen(image_name); i++){
#ifdef DEBUG
        printf("[INFO] checking %c against %c\n", image_name[i], extensionJPEG[i - strlen(image_name) + extensionJPEG_size]);
#endif
        if (image_name[i] != extensionJPEG[i - strlen(image_name) + extensionJPEG_size]) {
            jpegTrue = 0;
            break;
        }
    }

    if (jpegTrue) return 1;

    for (long unsigned int i = strlen(image_name) - extensionJPG_size; i < strlen(image_name); i++){
#ifdef DEBUG
        printf("[INFO] checking %c against %c\n", image_name[i], extensionJPG[i - strlen(image_name) + extensionJPG_size]);
#endif
        if (image_name[i] != extensionJPG[i - strlen(image_name) + extensionJPG_size]) return 0;
    }

    return 1;
}

void *thread_contrast(void *arg){
    thread_args *args = (thread_args*) arg;
    int pipe_in = args->pipe_read;
    int pipe_out = args->pipe_write;

    char *out_dir = (char *)args->generic;
    char *out_file = NULL; 

    image_filename_info image_name;
    gdImagePtr image;
    img_info info;

    int filename_chars;
    int outdir_chars;
    outdir_chars = strlen(out_dir);

    while (read(pipe_in, &image_name, sizeof(image_name)) > 0){
        clock_gettime(CLOCK_MONOTONIC, &info.processing_start);

        filename_chars = strlen(image_name.image_name);
        out_file = malloc(outdir_chars + filename_chars +2);
        if (!out_file){
            break; // solve later :)
        }
        snprintf(out_file, outdir_chars + filename_chars +2, "%s/%s", out_dir, image_name.image_name);

		if (access(out_file, F_OK) == 0) continue;

		image = read_jpeg_file(image_name.filename_full_path);
        if (!image) break;

		gdImageContrast(image, -20);
        info.image = image;
        info.name_info.image_name = image_name.image_name;
        info.name_info.filename_full_path = out_file;


        write(pipe_out, &info, sizeof(img_info));
#ifdef DEBUG
        printf("[CONTRAST] %s processed\n", info.name_info.image_name);
#endif
    }

    close(pipe_out);
    return NULL;
}


void *thread_smooth(void *arg){
    thread_args *args = (thread_args*) arg;
    int pipe_in = args->pipe_read;
    int pipe_out = args->pipe_write;

    img_info info;

    while (read(pipe_in, &info, sizeof(img_info)) > 0){
		gdImageSmooth(info.image, 20);
        write(pipe_out, &info, sizeof(info));
#ifdef DEBUG
        printf("[SMOOTH] %s processed\n", info.name_info.image_name);
#endif
    }

    close(pipe_out);
    return NULL;
}


void *thread_texture(void *arg){
    thread_args *args = (thread_args*) arg;
    int pipe_in = args->pipe_read;
    int pipe_out = args->pipe_write;

    char *texture_filename = (char *) args->generic;

    gdImagePtr image_tmp, texture;
    img_info info;

    texture = read_png_file(texture_filename);
    if (!texture) return NULL; // check later heh ;)
    while (read(pipe_in, &info, sizeof(info)) > 0){


        image_tmp = info.image;
		info.image = texture_image(image_tmp, texture);
        gdImageDestroy(image_tmp);

        write(pipe_out, &info, sizeof(info));
#ifdef DEBUG
        printf("[TEXTURE] %s processed\n", info.name_info.image_name);
#endif
    }
    gdImageDestroy(texture);
    close(pipe_out);
    return NULL;
}


void *thread_sepia(void *arg){
    thread_args *args = (thread_args*) arg;
    int pipe_in = args->pipe_read;

    int n_images = *(int *) args->generic;

    img_info *info = malloc (n_images * sizeof(*info));

    for (int i = 0; read(pipe_in, &info[i], sizeof(*info)) > 0 && i < n_images; i++){
        gdImageColor(info[i].image, 100, 60, 0, 0);
		if (write_jpeg_file(info[i].image, info[i].name_info.filename_full_path) == 0){
            fprintf(stderr, "[ERROR] Couldn't save %s!\n", info[i].name_info.image_name);
        }
        clock_gettime(CLOCK_MONOTONIC, &info[i].processing_end);
#ifdef DEBUG
        printf("[SEPIA] %s processed\n", info[i].name_info.filename_full_path);
#endif
    }

    return (void *) info;
}
