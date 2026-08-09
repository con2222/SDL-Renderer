#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>

extern "C" {
    #include "upng.h"
}

struct Texture {
    int width = 0;
    int height = 0;
    std::uint32_t* pixels = nullptr;
};

Texture load_png_texture_data(const char* filename); 

#endif // TEXTURE_H
