#version 450

#include <camera/camera.glsl>

layout(location = 0) out vec3 vViewDir;

layout(set = 0, binding = 0) uniform Camera {
    CameraData data;
} cam;

void main() {
    // Fullscreen triangle
    vec2 uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    vec2 ndc = uv * 2.0 - 1.0;

    vec4 clip = vec4(ndc, 1.0, 1.0);
    vec4 viewPos = inverse(cam.data.proj) * clip;
    vec3 viewDir = normalize(viewPos.xyz / viewPos.w);
    vViewDir = normalize(mat3(inverse(cam.data.view)) * viewDir);

    gl_Position = clip;
}