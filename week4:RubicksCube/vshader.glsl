#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;

out vec3 localNormal;

uniform mat4 uMVP;

void main() {
    gl_Position = uMVP * vec4(vPosition, 1.0);
    localNormal = vNormal;
}
