all: apply_contrast apply_sepia apply_smooth apply_texture

apply_contrast: apply_contrast.c image-lib.c image-lib.h
	gcc apply_contrast.c image-lib.c -g -o apply_contrast.out -lgd

apply_sepia: apply_sepia.c image-lib.c image-lib.h
	gcc apply_sepia.c image-lib.c -g -o apply_sepia.out -lgd

apply_smooth: apply_smooth.c image-lib.c image-lib.h
	gcc apply_smooth.c image-lib.c -g -o apply_smooth.out -lgd

apply_texture: apply_texture.c image-lib.c image-lib.h
	gcc apply_texture.c image-lib.c -g -o apply_texture.out -lgd
