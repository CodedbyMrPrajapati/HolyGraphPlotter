# Holy Graph Plotter
### **Summer of Code (SOC) Project**
This Repository was created and is currently maintained by Piyush Prajapati (24b0930).
Welcome to the **Holy Graph Plotter** project repository! This project is a high-performance, interactive 3D mathematical surface plotter built from scratch using C++, OpenGL, and a custom expression DSL.

---

## Project Curriculum & Schedule

This repository tracks progress week-by-week as we build the math engine, graphics pipeline, and user interface.

### **Phase 1: Foundation & Infrastructure**
* **[Week 0 - C++ Refresher](week%200/)**
  * Core language features: Lvalue/Rvalue references, RAII pattern, Smart Pointers (`unique_ptr`, `shared_ptr`, `weak_ptr`), Operator & Function Overloading, and Class/Function Templates.
* **[Week 1 - Build & Debugging Infrastructure](week%201/)**
  * Practical usage of version control (`git`), debuggers (`gdb`), build managers (`make`, `cmake`), and runtime memory analysis tools (AddressSanitizer and UndefinedBehaviorSanitizer).

### **Phase 2: Graphics Pipeline & Transforms**
* **[Week 2 - OpenGL Setup & First Shader](week%202/)**
  * Context creation using GLFW, dynamic extension loading using GLEW, first GLSL vertex & fragment shaders to draw a triangle, and pipeline inspection using RenderDoc.
* **[Week 3 - Transforms & Camera Controls](week%203/)**
  * 3D Math (Model-View-Projection), interactive spherical orbit camera, perspective/orthographic projections, and rendering a smooth-gradient 3D cube.
* **[Week 4 - Interactive Rubik's Cube](week%204/)**
  * Rubik's cube hierarchy (27 independent cubies), layer filtering along axes, smooth rotation animations, and dynamic face coloring (inside black-out).

### **Phase 3: Shading, Math DSL, & Runtime Parsing**
* **Week 5 - GLSL Deep Dive**
  * Phong lighting model (Ambient, Diffuse, Specular reflection), vertex normals, uniform buffers, and shader stepping with RenderDoc.
* **Week 6 - Hardcoded Surface Plotter**
  * Generating mesh vertices from a formula ($z = \sin(x)\cos(y)$), calculating smooth normals per-vertex, and rendering with a height-mapped color fragment shader.
* **Week 7 - Expression Tree Design & C++ DSL**
  * Designing node types (`Expr`, `BinaryOp`, `Variable`, `FuncCall`) and building an operator-overloaded C++ Domain Specific Language (DSL).
* **Week 8 - Expression Parser**
  * Finishing the DSL and adding a runtime string parser to evaluate and plot mathematical equations dynamically loaded from text files.

### **Phase 4: Dynamics & Rendering Modes**
* **Week 9 - Time-Varying Surfaces**
  * Driving surfaces dynamically using a time variable $t$, keyboard-controlled playback (play, pause, speed adjustment), and rendering mode toggles (Wireframe, Flat shading, Phong lighting).

---

## Optional Stretch Goals

* **Performance Pass**: Compute shaders for GPU-side equation evaluation, GPU timers, profiling CPU vs. GPU evaluation performance, and custom color maps.
* **Bonus Checkpoint**: Multiple simultaneous surfaces, screenshot/GIF export, and a shader gallery (heatmaps, contour lines, gradient-magnitude).

---

## Mentors
* **Balaji**
* **Vishwaajith**
