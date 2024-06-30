#ifndef DISTANCEMEASURES_HMT_IMAGE_LOADER_H
#define DISTANCEMEASURES_HMT_IMAGE_LOADER_H

#include <GL/glew.h>
#include <stb_image.h>

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char *filename, GLuint *out_texture, int *out_width, int *out_height);

#endif //DISTANCEMEASURES_HMT_IMAGE_LOADER_H
