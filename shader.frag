#version 460

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mix_val;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(-TexCoord.x, TexCoord.y)), mix_val);
}

