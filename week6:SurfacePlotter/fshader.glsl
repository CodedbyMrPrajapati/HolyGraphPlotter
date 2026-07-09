#version 440 core

in vec3 fragPos;
in vec3 normal;
in float height;

out vec4 frag_color;

uniform vec3 uLightPos;
uniform float uZMin;
uniform float uZMax;

// Map a normalised height t in [0,1] to a blue -> green -> red gradient.
vec3 heightColor(float t)
{
  vec3 low  = vec3(0.0, 0.1, 0.8);   // blue   (valleys)
  vec3 mid  = vec3(0.0, 0.8, 0.2);   // green  (mid level)
  vec3 high = vec3(0.9, 0.2, 0.0);   // red    (peaks)
  if (t < 0.5)
    return mix(low, mid, t * 2.0);
  return mix(mid, high, (t - 0.5) * 2.0);
}

void main ()
{
  float t = clamp((height - uZMin) / (uZMax - uZMin), 0.0, 1.0);
  vec3 baseColor = heightColor(t);

  // Simple diffuse term so the surface reads as 3D. Two-sided so the
  // underside of the surface is also lit.
  vec3 N = normalize(normal);
  vec3 L = normalize(uLightPos - fragPos);
  float diff = abs(dot(N, L));
  float ambient = 0.25;
  float lighting = ambient + (1.0 - ambient) * diff;

  frag_color = vec4(baseColor * lighting, 1.0);
}
