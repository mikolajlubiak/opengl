#version 460

struct Material {
    sampler2D diffuse;
    sampler2D specular;

    float shininess;
};

struct Light {
    vec3  position;
    vec3  direction;
    float cut_off;
    float outer_cut_off;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

layout (location = 0) in vec3 normal;
layout (location = 1) in vec3 frag_pos;
layout (location = 2) in vec2 tex_coords;

layout (location = 0) out vec4 frag_color;

uniform vec3 camera_pos;
uniform Material material;
uniform Light light;

void main()
{
    /*
    // diffuse light
    vec3 light_dir;
    float attenuation;

    if(light.vector.w == 0.0) {
        light_dir = normalize(-light.vector.xyz);
        attenuation = 1.0;
    }
    else if(light.vector.w == 1.0) {
        light_dir = normalize(light.vector.xyz - frag_pos);
        float distance = length(light.vector.xyz - frag_pos);
        attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    }
    */

    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, tex_coords).rgb;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light.position - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, tex_coords).rgb;

    // specular
    vec3 view_dir = normalize(camera_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, tex_coords).rgb;

    // spotlight
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon   = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    // attenuation
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    diffuse  *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    frag_color = vec4(result, 1.0);
}
