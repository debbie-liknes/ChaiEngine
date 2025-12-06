#version 450

layout(location = 0) in vec3 a_position;

layout(std140, binding = 2) uniform LightMatrices {
    mat4 u_lightViewProj;
};

layout(std140, binding = 1) uniform PerDrawUniforms
{
    mat4 u_model;
    mat4 u_normalMatrix;
};

void main() {
    gl_Position = u_lightViewProj * u_model * vec4(a_position, 1.0);
}