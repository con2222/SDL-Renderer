#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>

namespace C2 {

struct Texture {
    int width = 0;
    int height = 0;
    std::uint32_t* pixels = nullptr;
};

Texture load_png_texture_data(const char* filename); 

} // C2

#endif // TEXTURE_H
