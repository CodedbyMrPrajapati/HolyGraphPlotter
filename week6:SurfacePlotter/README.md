# Week 6 - Hardcoded Surface Plotter

The first real piece of **The Plotter**: turning a math formula into a rendered 3D surface.

Topics covered:
- Generating a mesh from a formula ($z = \sin(x)\cos(y)$)
- Indexed drawing with an element buffer (`glDrawElements`)
- Analytic per-vertex smooth normals
- Height-mapped color fragment shader
- Diffuse lighting on a generated surface

---

## Resources Used

- LearnOpenGL - Hello Triangle / Element Buffer Objects (https://learnopengl.com/Getting-started/Hello-Triangle)
- Paul Bourke - Surface Normals (http://paulbourke.net/geometry/)
- GLM Documentation (https://github.com/g-truc/glm)

---

## Notes

### Mesh Generation

The surface is sampled on an `N x N` grid over `x, y ∈ [-π, π]`. Each grid point becomes one
vertex at `(x, y, sin(x)cos(y))`.

```cpp
float surfaceZ(float x, float y) {
    return std::sin(x) * std::cos(y);
}
```

Adjacent grid points are stitched into two triangles per cell using an **index buffer**, so shared
vertices are stored only once.

Important takeaway:

Indexed drawing (`glDrawElements`) avoids duplicating the shared vertices between triangles.

### Analytic Smooth Normals

Because the formula is known, the exact gradient gives a perfectly smooth normal at every vertex:

```
dz/dx =  cos(x)cos(y)
dz/dy = -sin(x)sin(y)
normal = normalize(-dz/dx, -dz/dy, 1)
```

Important takeaway:

Analytic normals are exact; for arbitrary/parsed formulas (Week 8) a finite-difference estimate is
used instead.

### Color by Height

The fragment shader maps the normalised height into a blue → green → red gradient, then modulates
it with a diffuse term so the surface reads as 3D.

```glsl
float t = clamp((height - uZMin) / (uZMax - uZMin), 0.0, 1.0);
vec3  baseColor = heightColor(t);
```

Important takeaway:

Coloring by height makes the shape of the surface immediately readable.

### Keyboard Controls

- Left / Right arrow keys rotate around the Y axis
- Up / Down arrow keys rotate around the X axis
- Page Up / Page Down rotate around the Z axis

---

## Mini Project: Render `z = sin(x)cos(y)`

### Features

- Procedural `N x N` surface mesh
- Indexed triangle rendering
- Analytic smooth normals
- Blue → green → red height color map
- Diffuse lighting
- Perspective camera + keyboard rotation

### Concepts Practiced

| Concept | How it was Used |
|----------|----------------|
| Mesh Generation | Sample formula on a grid |
| Element Buffer | Indexed triangles |
| Smooth Normals | Analytic surface gradient |
| Fragment Color Map | Height → color gradient |
| Diffuse Lighting | Shading the generated surface |

---

## Build Instructions

### Build with CMake

```bash
mkdir -p build
cd build
cmake .. -G Ninja
cmake --build .
cd ..
./build/surface
```

### Build with Make

```bash
make
./surface
```

> Note: Shaders are loaded from the Week 6 folder, so the program can be run from
> `week6:SurfacePlotter` or from a nearby build directory. `Ninja` is used for the CMake generator
> because the folder name contains a `:` character.

---

## Completion Status

- [x] Mesh Generation from a Formula
- [x] Indexed Drawing
- [x] Analytic Smooth Normals
- [x] Height Color Fragment Shader
- [x] Diffuse Lighting
- [x] Keyboard Rotation

Week 6 completed.
