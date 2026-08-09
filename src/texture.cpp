#include "texture.h"
#include <cstring>


extern "C" {
    #include "upng.h"
}

Texture load_png_texture_data(const char* filename) {
    Texture texture;
    upng_t* png_texture = upng_new_from_file(filename);
    if (png_texture != nullptr) {
        upng_decode(png_texture);
        if (upng_get_error(png_texture) == UPNG_EOK) {
            texture.width = upng_get_width(png_texture);
            texture.height = upng_get_height(png_texture);
        
            int texture_size = upng_get_size(png_texture);
            texture.pixels = new uint32_t[texture_size / sizeof(uint32_t)];
            std::memcpy(texture.pixels, upng_get_buffer(png_texture), texture_size);
        }
    }
    upng_free(png_texture);
    return texture;
}
