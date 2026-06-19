#version 450
layout(location = 0) out vec2 vUV;

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec3 position;
} cam;


void main() {
    // Fullscreen triangle
    vec2 uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);

    gl_Position = vec4(uv * 2.0 - 1.0, 0.0, 1.0);
    vUV = uv;

}