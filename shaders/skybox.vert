#version 450
layout(location = 0) out vec3 vViewDir;

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec3 position;
} cam;

void main() {
    // Fullscreen triangle
    vec2 uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    vec2 ndc = uv * 2.0 - 1.0;

    vec4 clip = vec4(ndc, 1.0, 1.0);
    vec4 viewPos = inverse(cam.proj) * clip;
    vec3 viewDir = normalize(viewPos.xyz / viewPos.w);
    vViewDir = normalize(mat3(inverse(cam.view)) * viewDir);

    gl_Position = clip;
}