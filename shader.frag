#version 460

struct Material {
    // sampler2D diffuse;
    // sampler2D specular;

    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3  position;
    vec3  direction;
    float cut_off;
    float outer_cut_off;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout (location = 0) in vec3 normal;
layout (location = 1) in vec3 frag_pos;
layout (location = 2) in vec2 tex_coords;

layout (location = 0) out vec4 frag_color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform vec3 camera_pos;
uniform Material material;
uniform DirLight dir_light;
uniform SpotLight spot_light;
uniform PointLight point_light;

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 view_dir);
vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);

void main()
{
    // properties
    vec3 norm = normalize(normal);
    vec3 view_dir = normalize(camera_pos - frag_pos);

    // directional lighting
    vec3 result = calc_dir_light(dir_light, norm, view_dir);

    // point light
    result += calc_point_light(point_light, norm, frag_pos, view_dir);

    // spot light
    result += calc_spot_light(spot_light, norm, frag_pos, view_dir);

    frag_color = vec4(result, 1.0);
}

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 view_dir)
{
    vec3 light_dir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);

    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(texture_diffuse1, tex_coords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture_diffuse1, tex_coords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, tex_coords));

    return (ambient + diffuse + specular);
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    vec3 light_dir = normalize(light.position - frag_pos);

    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);

    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    // attenuation
    float distance    = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));

    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(texture_diffuse1, tex_coords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture_diffuse1, tex_coords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, tex_coords));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    vec3 light_dir = normalize(light.position - frag_pos);

    // diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);

    // specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // spotlight intensity
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, tex_coords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, tex_coords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, tex_coords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}
