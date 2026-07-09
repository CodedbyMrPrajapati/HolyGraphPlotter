# Week 8 - Runtime Parser & Interactive Plotter

The finished **Holy Graph Plotter**. This week completes the DSL with a runtime string parser so
equations can be loaded from a text file, then drives the surface with a time variable `t` and adds
keyboard-controlled playback and render-mode toggles. It **reuses the Week 7 DSL**
(`../week7:ExprDSL/expr.hpp`).

Topics covered:
- Recursive-descent parsing of math strings into a Week 7 expression tree
- Loading equations from a text file at runtime (or via `argv[1]`)
- Time-varying surfaces driven by `t`, re-evaluated every frame
- Finite-difference smooth normals for arbitrary parsed expressions
- Keyboard-controlled playback (play / pause / speed)
- Toggleable render modes: wireframe, flat, Phong

---

## Resources Used

- Crafting Interpreters - Parsing Expressions (https://craftinginterpreters.com/parsing-expressions.html)
- LearnOpenGL - Basic Lighting (https://learnopengl.com/Lighting/Basic-Lighting)
- OpenGL `glPolygonMode` reference (https://registry.khronos.org/OpenGL-Refpages/gl4/)

---

## Notes

### Runtime Parser

`parser.cpp` is a hand-written recursive-descent parser. The grammar handles operator precedence,
parentheses, unary minus, `^` (right-associative), numeric literals, the constants `pi`/`e`, the
variables `x`/`y`/`t`, and the functions `sin cos tan exp log sqrt abs`. It returns a Week 7
`dsl::Expression`:

```
expr    := term (('+' | '-') term)*
term    := unary (('*' | '/') unary)*
unary   := ('-' | '+') unary | power
power   := primary ('^' unary)?
primary := number | ident '(' expr ')' | ident | '(' expr ')'
```

`^` is right-associative and binds tighter than unary minus, so `-2^2 == -(2^2) == -4`.

The surface equation is read from `equation.txt` (first non-comment line) or from a path passed as
`argv[1]`.

Important takeaway:

Because the parser targets the same node types as the C++ DSL, everything downstream (evaluation,
meshing) is shared between hardcoded and text-loaded equations.

### Time-Varying Surfaces

Each frame (when not paused) advances `t += speed * dt` and rebuilds the mesh by evaluating the
expression over the grid with the binding `{x, y, t}`, then re-uploads via `glBufferSubData`.
Normals are estimated by **central differences** of the height grid, since an analytic gradient is
not available for an arbitrary parsed expression.

### Render Modes

- **Wireframe** — `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)`
- **Flat** — per-face normal from `cross(dFdx(fragPos), dFdy(fragPos))`
- **Phong** — interpolated smooth normal with ambient + diffuse + specular

---

## Controls

| Key | Action |
|-----|--------|
| `SPACE` | Play / pause the animation |
| `+` / `-` | Increase / decrease animation speed |
| `1` / `2` / `3` | Wireframe / Flat / Phong (or `M` to cycle) |
| Arrow keys | Orbit camera (X/Y) |
| Page Up / Page Down | Rotate around Z |
| `ESC` | Quit |

---

## Mini Project: Load & Animate an Equation

Edit `equation.txt` to plot a different surface without recompiling, e.g.:

```
sin(x + t) * cos(y + t)
sin(sqrt(x*x + y*y) - t)
0.5 * sin(x*y + t)
```

### Concepts Practiced

| Concept | How it was Used |
|----------|----------------|
| Recursive-Descent Parsing | String → expression tree |
| DSL Reuse | Week 7 nodes as parser output |
| Runtime Data Loading | Equation from a text file |
| Time Uniform / Animation | `t`-driven mesh rebuild |
| Finite-Difference Normals | Smooth normals for parsed surfaces |
| `glPolygonMode` / `dFdx` | Wireframe / flat / phong modes |

---

## Build Instructions

### Build with Make

```bash
make
./plotter                 # uses equation.txt
./plotter my_equation.txt # or supply your own
```

### Build with CMake

```bash
mkdir -p build
cd build
cmake .. -G Ninja
cmake --build .
cd ..
./build/plotter
```

> Note: shaders and `equation.txt` are resolved relative to the Week 8 folder, so the program can be
> run from `week8:Plotter` or a nearby build directory. `Ninja` is used for the CMake generator
> because the folder name contains a `:` character.

---

## Completion Status

- [x] Recursive-descent string parser
- [x] Equations loaded from a text file
- [x] DSL reused from Week 7
- [x] Time-varying surfaces driven by `t`
- [x] Play / pause / speed controls
- [x] Wireframe / flat / phong toggles

Week 8 completed. **The Holy Graph Plotter is complete.**
