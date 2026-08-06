#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>

extern "C" {
    #include "upng.h"
}

struct Texel {
    float u;
    float v;
};

extern int texture_width;
extern int texture_height;

extern const uint8_t REDBRICK_TEXTURE[];
extern upng_t* png_texture;
extern uint32_t* mesh_texture;

void load_png_texture_data(const char* filename); 

#endif // TEXTURE_H
