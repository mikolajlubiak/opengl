#version 460

layout (location = 0) in vec3 normal;
layout (location = 1) in vec3 frag_pos;

layout (location = 0) out vec4 frag_color;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 camera_pos;

void main()
{
    // ambient light
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * light_color;

    // diffuse light
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light_pos - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    // specular light
    float specular_strength = 0.5;
    vec3 camera_dir = normalize(camera_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(camera_dir, reflect_dir), 0.0), 256);
    vec3 specular = specular_strength * spec * light_color;

    // final result
    vec3 result = (ambient + diffuse + specular) * object_color;
    frag_color = vec4(result, 1.0);
}
