#version 460

out vec4 frag_color;

uniform vec3 object_color;
uniform vec3 light_color;

void main()
{
    frag_color = vec4(light_color * object_color, 1.0);
}
