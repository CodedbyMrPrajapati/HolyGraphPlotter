#version 440 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec3 vNormal;

out vec4 color;
out vec3 fragPos;   // world-space position, for lighting
out vec3 normal;    // world-space normal

uniform mat4 uModelMatrix;
uniform mat4 uModelViewProjectMatrix;
uniform mat3 uNormalMatrix;

void main ()
{
  gl_Position = uModelViewProjectMatrix * vPosition;
  fragPos = vec3(uModelMatrix * vPosition);
  normal  = normalize(uNormalMatrix * vNormal);
  color   = vColor;
}
