#version 460

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

layout (location = 0) out vec3 normal;
layout (location = 1) out vec3 frag_pos;

uniform mat4 model;
uniform mat3 normal_matrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    normal = normal_matrix * a_normal;
    frag_pos = vec3(model * vec4(a_pos, 1.0));
}
