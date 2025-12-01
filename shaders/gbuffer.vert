#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec4 a_Tangent;

out vec3 v_FragPos;
out vec2 v_TexCoord;
out mat3 v_TBN;

layout(std140, binding = 0) uniform PerFrameUniforms
{
    mat4 u_view;
    mat4 u_projection;
    mat4 u_invView;
};

layout(std140, binding = 1) uniform PerDrawUniforms
{
    mat4 u_model;
    mat4 u_normalMatrix;
};

void main()
{
    vec4 worldPos = u_model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;
    v_TexCoord = a_TexCoord;
    
    mat3 normalMatrix = mat3(u_normalMatrix);
    vec3 N = normalize(normalMatrix * a_Normal);
    vec3 T = normalize(normalMatrix * a_Tangent.xyz);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * a_Tangent.w;
    
    v_TBN = mat3(T, B, N);
    
    gl_Position = u_projection * u_view * worldPos;
}