#version 330 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModelViewProjectMatrix;
uniform mat4 uModelMatrix; // model -> world
uniform mat3 uNormalMatrix; // inverse-transpose of model's upper-left 3x3

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} vs_out;

void main()
{
    vec4 worldPos = uModelMatrix * aPos;
    vs_out.FragPos = worldPos.xyz;
    vs_out.Normal = normalize(uNormalMatrix * aNormal);
    gl_Position = uModelViewProjectMatrix * aPos;
}
