#ifndef CONFIGURATIONS_HPP
#define CONFIGURATIONS_HPP


namespace C2Renderer {

enum class CullMethod {
    CULL_NONE,
    CULL_BACKFACE
};

enum class RenderMethod {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE
};

}

#endif // CONFIGURATIONS_HPP