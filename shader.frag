#version 460

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout (location = 0) in vec3 normal;
layout (location = 1) in vec3 frag_pos;

layout (location = 0) out vec4 frag_color;

uniform vec3 camera_pos;
uniform Material material;
uniform Light light;

void main()
{
    // ambient light
    vec3 ambient  = light.ambient * material.ambient;

    // diffuse light
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light.position - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse  = light.diffuse * (diff * material.diffuse);

    // specular light
    vec3 camera_dir = normalize(camera_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(camera_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    // final result
    vec3 result = ambient + diffuse + specular;
    frag_color = vec4(result, 1.0);
}
