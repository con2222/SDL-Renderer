#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

namespace C2::Color {
    // === Basic Colors ===
    constexpr uint32_t Black       = 0x000000FF;
    constexpr uint32_t White       = 0xFFFFFFFF;
    constexpr uint32_t Red         = 0xFF0000FF;
    constexpr uint32_t Green       = 0x00FF00FF;
    constexpr uint32_t Blue        = 0x0000FFFF;
    constexpr uint32_t Yellow      = 0xFFFF00FF;
    constexpr uint32_t Cyan        = 0x00FFFFFF; // Light blue
    constexpr uint32_t Magenta     = 0xFF00FFFF; // Magenta

    // === Grayscale ===
    constexpr uint32_t Silver      = 0xC0C0C0FF;
    constexpr uint32_t LightGray   = 0xD3D3D3FF;
    constexpr uint32_t Gray        = 0x808080FF;
    constexpr uint32_t DarkGray    = 0xA9A9A9FF;
    constexpr uint32_t Charcoal    = 0x36454FFF; // Dark gray (charcoal)

    // === UI / Pleasant Colors (Palette) ===
    constexpr uint32_t Orange      = 0xFFA500FF;
    constexpr uint32_t Coral       = 0xFF7F50FF;
    constexpr uint32_t Gold        = 0xFFD700FF;
    constexpr uint32_t Pink        = 0xFFC0CBFF;
    constexpr uint32_t DeepPink    = 0xFF1493FF;
    constexpr uint32_t Purple      = 0x800080FF;
    constexpr uint32_t Indigo      = 0x4B0082FF;
    constexpr uint32_t Brown       = 0xA52A2AFF;
    constexpr uint32_t Maroon      = 0x800000FF; // Burgundy/Maroon

    // === Shades of Green (for landscapes) ===
    constexpr uint32_t Lime        = 0x00FF00FF;
    constexpr uint32_t ForestGreen = 0x228B22FF;
    constexpr uint32_t Olive       = 0x808000FF;
    constexpr uint32_t Teal        = 0x008080FF; // Blue-green

    // === Shades of Blue (for sky/water) ===
    constexpr uint32_t Navy        = 0x000080FF; // Dark blue
    constexpr uint32_t SkyBlue     = 0x87CEEBFF;
    constexpr uint32_t RoyalBlue   = 0x4169E1FF;

    // === Transparent (if the renderer supports alpha blending) ===
    constexpr uint32_t Transparent = 0x00000000; 
}

namespace C2 {

inline uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor) {
    uint32_t a = (original_color & 0xFF000000);
    uint32_t r = (original_color & 0x00FF0000) * percentage_factor;
    uint32_t g = (original_color & 0x0000FF00) * percentage_factor;
    uint32_t b = (original_color & 0x000000FF) * percentage_factor;
    uint32_t new_color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
    return new_color;
}

} // C2

#endif // COLOR_H
