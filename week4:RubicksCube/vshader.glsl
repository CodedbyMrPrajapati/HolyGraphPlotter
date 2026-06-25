#version 440 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec3 vNormal;

out vec4 color;
out vec3 normal;

uniform mat4 uModelViewProjectMatrix;

void main () 
{
  gl_Position = uModelViewProjectMatrix * vPosition;
  color = vColor;
  normal = vNormal;
}
