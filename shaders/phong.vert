#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// Outputs to fragment shader
out vec3 FragPos;
out vec3 Normal;

// Uniforms
uniform mat4 u_transform;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    // Transform position to world space
    vec4 worldPos = u_transform * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    
    // Transform normal to world space (assuming uniform scaling)
    // For non-uniform scaling, use: transpose(inverse(mat3(u_transform)))
    Normal = mat3(u_transform) * aNormal;
    
    // Final position
    gl_Position = u_projection * u_view * worldPos;
}