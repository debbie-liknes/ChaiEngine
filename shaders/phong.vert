#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform PerFrameUniforms
{
    mat4 u_view;
    mat4 u_projection;
};

layout(std140, binding = 1) uniform PerDrawUniforms
{
    mat4 u_model;
    mat4 u_normalMatrix;
};

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
    vec4 worldPos = u_model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;
    v_Normal = mat3(u_normalMatrix) * a_Normal;
    v_TexCoord = a_TexCoord;

    gl_Position = u_projection * u_view * worldPos;
}