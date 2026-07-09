#version 440 core

layout(location = 0) in vec4 vPosition;
layout(location = 2) in vec3 vNormal;

out vec3 fragPos;
out vec3 normal;
out float height;

uniform mat4 uModelMatrix;
uniform mat4 uModelViewProjectMatrix;
uniform mat3 uNormalMatrix;

void main ()
{
  gl_Position = uModelViewProjectMatrix * vPosition;
  fragPos = vec3(uModelMatrix * vPosition);
  normal  = normalize(uNormalMatrix * vNormal);
  height  = vPosition.z;
}
