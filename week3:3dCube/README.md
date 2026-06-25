# Week 3 - 3D Cube Transforms

Topics covered:
- Model Matrix
- View Matrix
- Projection Matrix
- Orthographic Projection
- GLM Transformations
- Depth Testing
- Rendering a 3D Cube
- Keyboard-Based Rotation

---

## Resources Used

- LearnOpenGL - Coordinate Systems (https://learnopengl.com/Getting-started/Coordinate-Systems)
- LearnOpenGL - Camera (https://learnopengl.com/Getting-started/Camera)
- GLFW Documentation (https://www.glfw.org/documentation.html)
- GLM Documentation (https://github.com/g-truc/glm)

---

## Notes

### Model Matrix

The model matrix is used to transform an object from its local object space into world space.

Example:

```cpp
glm::mat4 rotation =
    glm::rotate(
        glm::mat4(1.0f),
        angle,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
```

Important takeaway:

The model matrix is where object rotation, translation, and scaling are applied.

### View Matrix

The view matrix represents the camera transform.

Example:

```cpp
glm::mat4 view =
    glm::lookAt(
        glm::vec3(2.0f, 3.0f, -2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
```

Important takeaway:

`glm::lookAt` is a convenient way to position the camera and make it look at the scene.

### Projection Matrix

The projection matrix defines how 3D coordinates are mapped onto the 2D screen.

Example:

```cpp
glm::mat4 projection =
    glm::ortho(
        -2.0f, 2.0f,
        -2.0f, 2.0f,
        -20.0f, 20.0f
    );
```

Important takeaway:

Orthographic projection preserves parallel lines and is useful for understanding the transform pipeline clearly.

### Depth Testing

Depth testing ensures that the nearer face of the cube is drawn in front of faces behind it.

Example:

```cpp
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LESS);
```

Important takeaway:

Without depth testing, triangle draw order can make a 3D object look incorrect.

### Keyboard Controls

Keyboard input is used to rotate the cube interactively.

Controls:

- Left / Right arrow keys rotate around the Y axis
- Up / Down arrow keys rotate around the X axis
- Page Up / Page Down rotate around the Z axis

Important takeaway:

Simple input callbacks are enough to make transformation math visible and interactive.

---

## Mini Project: Render a 3D Cube

To reinforce all concepts learned this week, I created a shader-based OpenGL program that renders a colored 3D cube using the model-view-projection pipeline.

### Features

- 36-vertex cube built from 12 triangles
- Per-vertex colors
- Model-view-projection transform pipeline
- Orthographic camera setup
- Depth-tested 3D rendering
- Keyboard-controlled cube rotation

### Concepts Practiced

| Concept | How it was Used |
|----------|----------------|
| Model Matrix | Cube rotation |
| View Matrix | Camera positioning |
| Projection Matrix | Orthographic projection |
| GLM | Matrix and vector math |
| VBO | Vertex and color storage |
| VAO | Vertex attribute configuration |
| Depth Testing | Correct face visibility |
| GLFW Input | Interactive controls |

---

## Build Instructions

### Build with CMake

```bash
mkdir -p build
cd build
cmake .. -G Ninja
cmake --build .
cd ..
./build/cube
```

### Build with Make

```bash
make
./cube
```

> Note: The shaders are loaded from the Week 3 folder, so the program can be run from `week3:3dCube` or from a nearby build directory. `Ninja` is used for the CMake generator because the folder name contains a `:` character.

---

## Completion Status

- [x] Model Matrix
- [x] View Matrix
- [x] Projection Matrix
- [x] Orthographic Projection
- [x] GLM Transformations
- [x] Depth Testing
- [x] 3D Cube Rendering
- [x] Keyboard Rotation

Week 3 completed.
