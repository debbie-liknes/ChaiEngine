#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

// Per-frame uniforms (view, projection)
layout(std140) uniform PerFrameUniforms
{
    mat4 u_View;
    mat4 u_Projection;
};

// Per-draw uniforms (model matrix)
layout(std140) uniform PerDrawUniforms
{
    mat4 u_Model;
    mat4 u_NormalMatrix;
};

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;
    v_Normal = mat3(u_NormalMatrix) * a_Normal;
    v_TexCoord = a_TexCoord;
    
    gl_Position = u_Projection * u_View * worldPos;
}