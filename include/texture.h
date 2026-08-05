#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>


struct Texel {
    float u;
    float v;
};

extern int texture_width;
extern int texture_height;

extern const uint8_t REDBRICK_TEXTURE[];

extern uint32_t* mesh_texture;

#endif // TEXTURE_H
