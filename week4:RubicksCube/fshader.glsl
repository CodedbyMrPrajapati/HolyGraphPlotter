#version 430 core

in vec3 localNormal;
out vec4 fColor;

// Uniform array of colors for the 6 faces: +X, -X, +Y, -Y, +Z, -Z
uniform vec4 uFaceColors[6];

void main() {
    // Determine which face we are rendering based on the local normal
    vec3 absN = abs(localNormal);
    vec4 col = vec4(0.0, 0.0, 0.0, 1.0); // Default black

    if (absN.x > absN.y && absN.x > absN.z) {
        if (localNormal.x > 0.0) {
            col = uFaceColors[0]; // +X
        } else {
            col = uFaceColors[1]; // -X
        }
    } else if (absN.y > absN.x && absN.y > absN.z) {
        if (localNormal.y > 0.0) {
            col = uFaceColors[2]; // +Y
        } else {
            col = uFaceColors[3]; // -Y
        }
    } else {
        if (localNormal.z > 0.0) {
            col = uFaceColors[4]; // +Z
        } else {
            col = uFaceColors[5]; // -Z
        }
    }

    fColor = col;
}
