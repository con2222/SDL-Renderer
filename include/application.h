#pragma once
#include "configurations.h"
#include <cstdint>

struct ApplicationState {
    bool is_running = false;
    float delta_time = 0.0f;
    uint64_t previous_frame_time = 0;
    
    C2Renderer::RenderMethod render_method = C2Renderer::RenderMethod::RENDER_WIRE;
    C2Renderer::CullMethod cull_method = C2Renderer::CullMethod::CULL_BACKFACE;
};