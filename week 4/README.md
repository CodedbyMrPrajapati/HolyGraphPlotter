# Week 4: Rubik's Cube with Per-Face Rotations

This week, we synthesize our knowledge of shaders, buffers, transforms, and matrices to build an interactive 3D Rubik's Cube simulation. The application models the Rubik's Cube as 27 independent elements (cubies) that can be rotated dynamically in 3D space by filtering layers along the X, Y, and Z axes.

## How to Verify
Compile and run the program:

**Using GNU Make:**
```bash
make
./rubik.exe
```

**Using CMake:**
```bash
mkdir build
cd build
cmake ..
cmake --build .
./rubik.exe
```

### Controls
- **Rotate Face (Clockwise)**: Press `U`, `D`, `L`, `R`, `F`, `B` for the respective Up, Down, Left, Right, Front, and Back faces.
- **Rotate Face (Counter-Clockwise)**: Hold `SHIFT` while pressing the face key (e.g., `SHIFT + R` for $R'$).
- **Arrow Keys**: Orbit camera around the Rubik's Cube.
- **ESC**: Close the application.

---

## Concepts & Definitions

### 1. Rubik's Cube Representation
Instead of modeling the Rubik's Cube as a single monolithic mesh, it is split into a $3 \times 3 \times 3$ grid of **27 independent cubies**:
- **Grid Coordinates**: Each cubie stores an integer position `gridPos` in $\{ -1, 0, 1 \}^3$ representing its position in the Rubik's Cube.
- **Orientation Matrix**: Each cubie maintains an orientation matrix `currentRotation` (`glm::mat4`) representing its cumulative rotations.

### 2. Layer Filtering & Rotation Math
When a face rotation is requested, we identify the cubies that belong to that face:
1. **Identify the Layer**: For example, rotating the Right face ($R$) corresponds to filtering all cubies with `gridPos.x == 1`.
2. **Apply Rotations during Animation**: While animating, a temporary rotation matrix is applied to the model matrix of the filtered cubies:
   $$\mathbf{M}_{\text{anim}} = \mathbf{R}_{\text{axis}}(\theta) \times \mathbf{T}(\text{gridPos}) \times \mathbf{R}_{\text{orientation}}$$
3. **Commit the State**: When the animation completes (reaches $90^\circ$ or $-90^\circ$), we update the cubies' state variables:
   - The orientation matrix is updated: $\mathbf{R}_{\text{orientation}} \leftarrow \mathbf{R}_{\text{axis}}(\pm 90^\circ) \times \mathbf{R}_{\text{orientation}}$.
   - The integer grid position is rotated using integer matrix multiplication or rounding the result of the float rotation.

### 3. Dynamic Per-Face Coloring (Inner Black-Out)
A real Rubik's Cube has colored stickers only on the outer faces; the inside faces are solid black.
- **Boundary Detection**: During the draw loop, we inspect each cubie's grid position. If a cubie is not at the boundary (e.g., $x \neq 1$), its $+X$ face is internal, and we set its color to dark gray/black.
- **Local Normal Shading**: The vertex shader outputs the local, un-rotated vertex normals (e.g., $(0, 1, 0)$ for top face). The fragment shader receives these interpolated normals and assigns the corresponding face color, ensuring the correct face receives the correct color even as the cubie rotates.
- **Gaps**: Each cubie is scaled down slightly (e.g., to $93\%$ size) during rendering to expose a dark line between them, creating a clean Rubik's Cube aesthetic.
