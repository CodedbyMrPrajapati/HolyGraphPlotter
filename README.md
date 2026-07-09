# Holy Graph Plotter
### **Summer of Code (SOC) Project**
This Repository was created and is currently maintained by Piyush Prajapati (24b0930).
Welcome to the **Holy Graph Plotter** project repository! This project is a high-performance, interactive 3D mathematical surface plotter built from scratch using C++, OpenGL, and a custom expression DSL.

---

## Project Curriculum & Schedule

This repository tracks progress week-by-week. Weeks 0–4 are the **learning phase** (C++, tooling,
and the graphics pipeline). Weeks 5–8 build **The Plotter** — the main project — where each week
adds a working piece of the final interactive 3D math-surface plotter.

### **Phase 1: Foundation & Infrastructure**
* **[Week 0 - C++ Refresher](week0%3AC%2B%2B/)**
  * Core language features: Lvalue/Rvalue references, RAII pattern, Smart Pointers (`unique_ptr`, `shared_ptr`, `weak_ptr`), Operator & Function Overloading, and Class/Function Templates.
* **[Week 1 - Build & Debugging Infrastructure](week1%3AGLSL/)**
  * Practical usage of version control (`git`), debuggers (`gdb`), build managers (`make`, `cmake`), and runtime memory analysis tools (AddressSanitizer and UndefinedBehaviorSanitizer).

### **Phase 2: Graphics Pipeline & Transforms**
* **[Week 2 - OpenGL Setup & First Shader](week2%3ATriangle/)**
  * Context creation using GLFW, dynamic extension loading using GLEW, first GLSL vertex & fragment shaders to draw a triangle, and pipeline inspection using RenderDoc.
* **[Week 3 - Transforms & Camera Controls](week3%3A3dCube/)**
  * 3D Math (Model-View-Projection), interactive spherical orbit camera, perspective/orthographic projections, and rendering a smooth-gradient 3D cube.
* **[Week 4 - Interactive Rubik's Cube](week4%3ARubicksCube/)**
  * Rubik's cube hierarchy (27 independent cubies), layer filtering along axes, smooth rotation animations, and dynamic face coloring (inside black-out).

### **Phase 3: The Plotter (Main Project)**
The core deliverable, built up over four weeks from a lit cube to a fully interactive,
text-driven, animated surface plotter.
* **[Week 5 - GLSL Deep Dive: Phong Lighting](week5%3APhong/)**
  * The Phong lighting model (Ambient, Diffuse, Specular reflection), per-vertex normals, the normal matrix, and world-space lighting in the fragment shader — the shading groundwork for the plotter.
* **[Week 6 - Hardcoded Surface Plotter](week6%3ASurfacePlotter/)**
  * Generating mesh vertices from a formula ($z = \sin(x)\cos(y)$), calculating analytic smooth normals per-vertex, indexed drawing, and rendering with a height-mapped color fragment shader.
* **[Week 7 - Expression Tree Design & C++ DSL](week7%3AExprDSL/)**
  * Designing node types (`Expr`, `Constant`, `Variable`, `BinaryOp`, `FuncCall`) and building an operator-overloaded C++ Domain Specific Language (DSL) for describing surfaces as math.
* **[Week 8 - Runtime Parser & Interactive Plotter](week8%3APlotter/)**
  * Finishing the DSL with a runtime string parser (equations loaded from a text file), then driving the surface with a time variable $t$, keyboard-controlled playback (play / pause / speed), and toggleable render modes (Wireframe, Flat, Phong). **The finished Holy Graph Plotter.**

---

## Optional Stretch Goals

* **Performance Pass**: Compute shaders for GPU-side equation evaluation, GPU timers, profiling CPU vs. GPU evaluation performance, and custom color maps.
* **Bonus Checkpoint**: Multiple simultaneous surfaces, screenshot/GIF export, and a shader gallery (heatmaps, contour lines, gradient-magnitude).

---

## Mentors
* **Balaji**
* **Vishwaajith**
