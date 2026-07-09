#version 440 core

in vec3 fragPos;
in vec3 normal;
in float height;

out vec4 frag_color;

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform float uZMin;
uniform float uZMax;
uniform int uShadingMode;   // 0 = smooth (phong), 1 = flat (per-face normal)

vec3 heightColor(float t)
{
  vec3 low  = vec3(0.0, 0.1, 0.8);   // blue
  vec3 mid  = vec3(0.0, 0.8, 0.2);   // green
  vec3 high = vec3(0.9, 0.2, 0.0);   // red
  if (t < 0.5)
    return mix(low, mid, t * 2.0);
  return mix(mid, high, (t - 0.5) * 2.0);
}

void main ()
{
  float t = clamp((height - uZMin) / (uZMax - uZMin), 0.0, 1.0);
  vec3 baseColor = heightColor(t);

  // Flat shading derives a single normal per triangle from screen-space
  // derivatives of the world position; phong uses the interpolated normal.
  vec3 N;
  if (uShadingMode == 1)
    N = normalize(cross(dFdx(fragPos), dFdy(fragPos)));
  else
    N = normalize(normal);

  vec3 L = normalize(uLightPos - fragPos);
  vec3 V = normalize(uViewPos - fragPos);
  vec3 R = reflect(-L, N);

  float ambient  = 0.2;
  float diffuse  = abs(dot(N, L));              // two-sided
  float specular = 0.4 * pow(max(dot(V, R), 0.0), 32.0);

  vec3 result = baseColor * (ambient + diffuse) + vec3(specular);
  frag_color = vec4(result, 1.0);
}
