#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} fs_in;

out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 color;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main()
{
    vec3 N = normalize(fs_in.Normal);
    vec3 L = normalize(light.position - fs_in.FragPos);
    vec3 V = normalize(viewPos - fs_in.FragPos);
    vec3 R = reflect(-L, N);

    // Ambient
    vec3 ambient = material.ambient * light.color;

    // Diffuse
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = material.diffuse * diff * light.color;

    // Specular (Phong)
    float spec = 0.0;
    if (diff > 0.0)
        spec = pow(max(dot(R, V), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.color;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
