# Week 2 – OpenGL Basics

Topics covered:
- GLFW
- GLM
- OpenGL Context Setup
- Vertex Shaders
- Fragment Shaders
- Rendering a Triangle
- RenderDoc Introduction

---

## Resources Used

- LearnOpenGL (https://learnopengl.com)
- GLFW Documentation (https://www.glfw.org/documentation.html)
- GLM Documentation (https://github.com/g-truc/glm)
- RenderDoc Documentation (https://renderdoc.org)

---

## Notes

### GLFW

GLFW is used for:

- Creating windows
- Creating OpenGL contexts
- Handling input
- Managing the render loop

Example:

```cpp
GLFWwindow* window =
    glfwCreateWindow(800, 600,
                     "OpenGL", nullptr, nullptr);
```

### GLM

GLM is a mathematics library designed for graphics applications.

Example:

```cpp
glm::vec3 position(1.0f, 2.0f, 3.0f);
```

Common types:

- glm::vec2
- glm::vec3
- glm::vec4
- glm::mat4

### Vertex Shader

The vertex shader processes vertex data.

Example:

```glsl
#version 330 core

layout(location = 0)
in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}
```

### Fragment Shader

The fragment shader determines pixel color.

Example:

```glsl
#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(
        1.0,
        0.5,
        0.2,
        1.0
    );
}
```

### RenderDoc

RenderDoc is a graphics debugger.

Used for:

- Frame capture
- Inspecting draw calls
- Viewing shaders
- Inspecting buffers and textures

Important takeaway:

RenderDoc helps visualize what the GPU is doing during rendering.

---

## Mini Project: Render a Triangle

To reinforce all concepts learned this week, I created a simple OpenGL application that renders a triangle.

### Features

- GLFW window creation
- OpenGL context setup
- Vertex shader compilation
- Fragment shader compilation
- Vertex buffer creation
- Triangle rendering

### Concepts Practiced

| Concept | How it was Used |
|----------|----------------|
| GLFW | Window and context creation |
| GLM | Math library setup |
| Vertex Shader | Vertex processing |
| Fragment Shader | Pixel coloring |
| VBO | Vertex storage |
| VAO | Vertex configuration |
| OpenGL Pipeline | Rendering |
| RenderDoc | Frame inspection |

---

## Build Instructions

### Build with CMake

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
cd ..
./build/triangle
```

### Build with Make

```bash
make
./triangle
```

> Note: The application loads `vshader.glsl` and `fshader.glsl` from the current working directory, so run the binary from the project folder or copy the shader files into the build directory.

---

## Completion Status

- [x] GLFW
- [x] GLM
- [x] OpenGL Setup
- [x] Vertex Shader
- [x] Fragment Shader
- [x] Triangle Rendering
- [x] RenderDoc Introduction

Week 2 completed.