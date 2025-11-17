#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// Outputs to fragment shader
out vec3 FragPos;
out vec3 Normal;

layout(location = 0) in vec3 a_position;

layout(std140, binding = 0) uniform PerFrame
{
    mat4 u_view;
    mat4 u_proj;
};

layout(std140, binding = 0) uniform PerDraw
{
    mat4 u_model;
    mat4 u_normal;
};

void main()
{
    // Transform position to world space
    vec4 worldPos = u_model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    
    // Transform normal to world space (assuming uniform scaling)
    // For non-uniform scaling, use: transpose(inverse(mat3(u_transform)))
    Normal = mat3(u_model) * aNormal;
    
    // Final position
    gl_Position = u_proj * u_view * worldPos;
}