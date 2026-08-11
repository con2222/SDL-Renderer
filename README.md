# 3D Software Rasterizer

A custom 3D software rasterizer built from scratch in C++. This project renders 3D meshes without relying on any hardware acceleration or modern graphics APIs (like OpenGL or Vulkan), drawing everything pixel by pixel using the CPU.

### Features

* **Custom Math Library:** Built-in vector and matrix math implementations (`vec2`, `vec3`, `mat4`).
* **Triangle Rasterization:** Uses the Pineda edge equation algorithm (barycentric coordinates).
* **Subpixel Accuracy:** Evaluates exact pixel centers to prevent vertex jittering and geometry distortion.
* **Perspective-Correct Texturing:** Accurately maps 2D textures onto 3D triangles using $1/W$ interpolation.
* **3D Pipeline:** Supports OBJ file loading, 3D camera movement, backface culling, and Z-buffering.

### Dependencies

* [SDL2](https://www.libsdl.org/) - Used exclusively for creating the window, handling user input, and displaying the final color buffer.

### Credits

This project was heavily inspired by and built following the excellent Computer Graphics courses by Gustavo Pezzi at [Pikuma.com](https://pikuma.com/).