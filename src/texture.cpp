#include "texture.h"


extern "C" {
    #include "upng.h"
}

int texture_width = 64;
int texture_height = 64;

upng_t* png_texture = nullptr;
uint32_t* mesh_texture = nullptr;

Texture load_png_texture_data(const char* filename) {
    Texture texture;
    upng_t* png_texture = upng_new_from_file(filename);
    if (png_texture != nullptr) {
        upng_decode(png_texture);
        if (upng_get_error(png_texture) == UPNG_EOK) {
            texture.pixels = (uint32_t*)upng_get_buffer(png_texture);
            texture.width = upng_get_width(png_texture);
            texture.height = upng_get_height(png_texture);
        }
    }
    upng_free(png_texture);
    return texture;
}
