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
typedef struct {
    gdImagePtr image;
    char *filename;

    // time taken?
} img_info;

int is_jpeg(char *filename){
    if (!filename) return 0;

    int extensionJPEG_size;
    int extensionJPG_size;

    char extensionJPEG[] = ".jpeg";
    char extensionJPG[] = ".jpg";

    extensionJPEG_size = strlen(extensionJPEG);
    extensionJPG_size = strlen(extensionJPG);

    int jpegTrue = 1;

#ifdef DEBUG
    printf("[INFO] got file %s\n", filename);
#endif

    for (long unsigned int i = strlen(filename) - extensionJPEG_size; i < strlen(filename); i++){
#ifdef DEBUG
        printf("[INFO] checking %c against %c\n", filename[i], extensionJPEG[i - strlen(filename) + extensionJPEG_size]);
#endif
        if (filename[i] != extensionJPEG[i - strlen(filename) + extensionJPEG_size]) {
            jpegTrue = 0;
            break;
        }
    }

    if (jpegTrue) return 1;

    for (long unsigned int i = strlen(filename) - extensionJPG_size; i < strlen(filename); i++){
#ifdef DEBUG
        printf("[INFO] checking %c against %c\n", filename[i], extensionJPG[i - strlen(filename) + extensionJPG_size]);
#endif
        if (filename[i] != extensionJPG[i - strlen(filename) + extensionJPG_size]) return 0;
    }

    return 1;
}

void *thread_contrast(void *arg){
    thread_args *args = (thread_args*) arg;
    int pipe_in = args->pipe_read;
    int pipe_out = args->pipe_write;

    char *out_dir = args->generic;
    char *out_file = NULL; 

    image_filenames image_name;
    gdImagePtr image;
    img_info info;

    int filename_chars;
    int outdir_chars;
    outdir_chars = strlen(out_dir);

    while (read(pipe_in, &image_name, sizeof(image_name)) > 0){
        filename_chars = strlen(image_name.filenames);
        out_file = malloc(outdir_chars + filename_chars +2);
        if (!out_file){
            break; // solve later :)
        }
        snprintf(out_file, outdir_chars + filename_chars +2, "%s/%s", out_dir, image_name.filenames);

		if (access(out_file, F_OK) == 0) continue;

		image = read_jpeg_file(image_name.filenames_directory);
        if (!image) break;

		gdImageContrast(image, -20);
        info.image = image;
        info.filename = out_file;

        write(pipe_out, &info, sizeof(img_info));
#ifdef DEBUG
        printf("[CONTRAST] %s processed\n", info.filename);
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
        printf("[SMOOTH] %s processed\n", info.filename);
#endif
    }

    close(pipe_out);
    return NULL;
}


void *thread_texture(void *arg){
    thread_args *args = (thread_args*) arg;
    int pipe_in = args->pipe_read;
    int pipe_out = args->pipe_write;

    char *texture_filename = args->generic;

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
        printf("[TEXTURE] %s processed\n", info.filename);
#endif
    }
    gdImageDestroy(texture);
    close(pipe_out);
    return NULL;
}


void *thread_sepia(void *arg){
    thread_args *args = (thread_args*) arg;
    int pipe_in = args->pipe_read;

    img_info info;

    while (read(pipe_in, &info, sizeof(info)) > 0){
        gdImageColor(info.image, 100, 60, 0, 0);
		if (write_jpeg_file(info.image, info.filename) == 0){
            fprintf(stderr, "[ERROR] Couldn't save image!\n");
        }
		gdImageDestroy(info.image);	
        free(info.filename);
#ifdef DEBUG
        printf("[SEPIA] %s processed\n", info.filename);
#endif
    }

    return NULL;
}
 

#if 0
void *thread_process_images(void *arg) {

	struct timespec start_time, end_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);

    int images_processed;
	char out_file[256];

    int outdir_chars;
    int filename_chars;

	gdImagePtr paper_texture;

	thread_output *ret = malloc(sizeof(thread_output));
    if (!ret) {
        fprintf(stderr, "[ERROR] Couldn't alloc return value\n");
        return NULL;
    }

	thread_args *args = (thread_args *) arg;

	images_processed = 0;

	paper_texture = args->texture;

	gdImagePtr image;
	gdImagePtr image_tmp;

    image_filenames image_name;

    outdir_chars = strlen(args->out_directory);
	while(read(args->pipe_read, &image_name, sizeof(image_name)) > 0){

        filename_chars = strlen(image_name.filenames);
        snprintf(out_file, outdir_chars + filename_chars +2, "%s/%s", args->out_directory, image_name.filenames);

		if (access(out_file, F_OK) == 0) continue;

		image = read_jpeg_file(image_name.filenames_directory);
        if (!image) return NULL;

		gdImageContrast(image, -20);
		gdImageSmooth(image, 20);

        image_tmp = image;
		image = texture_image(image_tmp, paper_texture);
        gdImageDestroy(image_tmp);

        gdImageColor(image, 100, 60, 0, 0);

		if (write_jpeg_file(image, out_file) == 0){
            fprintf(stderr, "[ERROR] Couldn't save image!\n");
        }
		gdImageDestroy(image);	

		images_processed++;
	}

	clock_gettime(CLOCK_MONOTONIC, &end_time);
	
	ret->time = diff_timespec(&end_time, &start_time);
	ret->n_images_processed = images_processed;

	return ret;

}
#endif
