#version 440 core

in vec4 color;
in vec3 fragPos;
in vec3 normal;

out vec4 frag_color;

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;

void main ()
{
  // Phong reflection model: ambient + diffuse + specular

  // Ambient
  float ambientStrength = 0.1;
  vec3 ambient = ambientStrength * uLightColor;

  // Diffuse
  vec3 N = normalize(normal);
  vec3 L = normalize(uLightPos - fragPos);
  float diff = max(dot(N, L), 0.0);
  vec3 diffuse = diff * uLightColor;

  // Specular
  float specularStrength = 0.5;
  vec3 V = normalize(uViewPos - fragPos);
  vec3 R = reflect(-L, N);
  float spec = pow(max(dot(V, R), 0.0), 32.0);
  vec3 specular = specularStrength * spec * uLightColor;

  vec3 result = (ambient + diffuse + specular) * color.rgb;
  frag_color = vec4(result, 1.0);
}
