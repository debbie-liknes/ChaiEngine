#version 430 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

// Per-frame uniforms (UBO binding 0)
layout(std140, binding = 0) uniform PerFrame
{
    mat4 view;
    mat4 projection;
};

// Per-draw uniforms (UBO binding 1)
layout(std140, binding = 1) uniform PerDraw
{
    mat4 model;
    mat4 normalMatrix;
};

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    vec4 worldPos = model * vec4(aPosition, 1.0);
    FragPos = worldPos.xyz;
    Normal = mat3(normalMatrix) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = projection * view * worldPos;
}