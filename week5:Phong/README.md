# Week 5 - GLSL Deep Dive (Phong Lighting)

Topics covered:
- The Phong reflection model (Ambient + Diffuse + Specular)
- Per-vertex normals
- The normal matrix (`transpose(inverse(model))`)
- World-space lighting in the fragment shader
- Passing light/camera uniforms into GLSL

---

## Resources Used

- LearnOpenGL - Basic Lighting (https://learnopengl.com/Lighting/Basic-Lighting)
- LearnOpenGL - Materials (https://learnopengl.com/Lighting/Materials)
- GLM Documentation (https://github.com/g-truc/glm)

---

## Notes

### Phong Reflection Model

Lighting is split into three additive terms:

```glsl
vec3 ambient  = ambientStrength * lightColor;
vec3 diffuse  = max(dot(N, L), 0.0) * lightColor;
vec3 specular = specularStrength * pow(max(dot(V, R), 0.0), shininess) * lightColor;
vec3 result   = (ambient + diffuse + specular) * objectColor;
```

- **Ambient** is a small constant so unlit faces are not pure black.
- **Diffuse** depends on the angle between the surface normal `N` and the light direction `L`.
- **Specular** is the shiny highlight, computed from the reflection vector `R` and the view
  direction `V`, sharpened by the `shininess` exponent.

Important takeaway:

Lighting is computed in **world space**, so both the fragment position and the normal are
transformed out of object space in the vertex shader.

### The Normal Matrix

Normals cannot be transformed by the model matrix directly when it contains non-uniform scaling.
The correct transform is:

```cpp
glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
```

Important takeaway:

Using the normal matrix keeps normals perpendicular to the surface after transformation.

### Keyboard Controls

- Left / Right arrow keys rotate around the Y axis
- Up / Down arrow keys rotate around the X axis
- Page Up / Page Down rotate around the Z axis

Rotating the cube moves the specular highlight across its faces, which makes the lighting model
visible and interactive.

---

## Mini Project: Phong-Lit Cube

A shader-based OpenGL program that renders a single cube lit by the full Phong model. The cube can
be rotated to watch the diffuse shading and the specular highlight respond to the light.

### Features

- 36-vertex cube with per-face normals
- Ambient + diffuse + specular lighting
- Normal matrix for correct normals
- Perspective camera
- Keyboard-controlled rotation

### Concepts Practiced

| Concept | How it was Used |
|----------|----------------|
| Vertex Normals | Per-face outward normals stored in attribute 2 |
| Normal Matrix | Correct normal transformation |
| Diffuse Lighting | Angle between normal and light |
| Specular Lighting | View/reflection based highlight |
| Uniforms | Light position, color, camera position |
| GLM | Matrix and vector math |

---

## Build Instructions

### Build with CMake

```bash
mkdir -p build
cd build
cmake .. -G Ninja
cmake --build .
cd ..
./build/phong
```

### Build with Make

```bash
make
./phong
```

> Note: Shaders are loaded from the Week 5 folder, so the program can be run from `week5:Phong` or
> from a nearby build directory. `Ninja` is used for the CMake generator because the folder name
> contains a `:` character.

---

## Completion Status

- [x] Phong Reflection Model
- [x] Vertex Normals
- [x] Normal Matrix
- [x] Ambient / Diffuse / Specular
- [x] Perspective Camera
- [x] Keyboard Rotation

Week 5 completed.
