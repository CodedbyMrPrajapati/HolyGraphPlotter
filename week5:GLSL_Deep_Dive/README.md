# Week 5 — GLSL Deep Dive

Topics covered:

- Phong lighting model (ambient, diffuse, specular)
- Per-vertex normals (sent from the CPU)
- Uniform structs (`Light` and `Material`) in GLSL

Files:

- `main.cpp` — example program rendering a lit cube using `gl_framework` and the shaders.
- `vshader.glsl` — vertex shader that passes world-space position and normal.
- `fshader.glsl` — fragment shader that implements Phong lighting using uniform structs.
- `CMakeLists.txt` — basic cmake target for the example.

Build & run (from repository root):

```
mkdir -p build && cd build
cmake ..
cmake --build . --target week5
./week5
```

Notes:

- This example expects the `gl_framework.hpp` and `shader_util.hpp` files available in the repository root (same pattern as previous weeks).
- Adjust link flags in `CMakeLists.txt` if your platform requires different OpenGL/GLFW/GLEW link names.
