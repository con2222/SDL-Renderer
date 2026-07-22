#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

namespace C2::Color {
    // === Basic Colors ===
    constexpr uint32_t Black       = 0xFF000000;
    constexpr uint32_t White       = 0xFFFFFFFF;
    constexpr uint32_t Red         = 0xFFFF0000;
    constexpr uint32_t Green       = 0xFF00FF00;
    constexpr uint32_t Blue        = 0xFF0000FF;
    constexpr uint32_t Yellow      = 0xFFFFFF00;
    constexpr uint32_t Cyan        = 0xFF00FFFF; // Light blue
    constexpr uint32_t Magenta     = 0xFFFF00FF; // Magenta

    // === Grayscale ===
    constexpr uint32_t Silver      = 0xFFC0C0C0;
    constexpr uint32_t LightGray   = 0xFFD3D3D3;
    constexpr uint32_t Gray        = 0xFF808080;
    constexpr uint32_t DarkGray    = 0xFFA9A9A9;
    constexpr uint32_t Charcoal    = 0xFF36454F; // Dark gray (charcoal)

    // === UI / Pleasant Colors (Palette) ===
    constexpr uint32_t Orange      = 0xFFFFA500;
    constexpr uint32_t Coral       = 0xFFFF7F50;
    constexpr uint32_t Gold        = 0xFFFFD700;
    constexpr uint32_t Pink        = 0xFFFFC0CB;
    constexpr uint32_t DeepPink    = 0xFFFF1493;
    constexpr uint32_t Purple      = 0xFF800080;
    constexpr uint32_t Indigo      = 0xFF4B0082;
    constexpr uint32_t Brown       = 0xFFA52A2A;
    constexpr uint32_t Maroon      = 0xFF800000; // Burgundy/Maroon

    // === Shades of Green (for landscapes) ===
    constexpr uint32_t Lime        = 0xFF00FF00;
    constexpr uint32_t ForestGreen = 0xFF228B22;
    constexpr uint32_t Olive       = 0xFF808000;
    constexpr uint32_t Teal        = 0xFF008080; // Blue-green

    // === Shades of Blue (for sky/water) ===
    constexpr uint32_t Navy        = 0xFF000080; // Dark blue
    constexpr uint32_t SkyBlue     = 0xFF87CEEB;
    constexpr uint32_t RoyalBlue   = 0xFF4169E1;

    // === Transparent (if the renderer supports alpha blending) ===
    constexpr uint32_t Transparent = 0x00000000; 
}

#endif // COLOR_H