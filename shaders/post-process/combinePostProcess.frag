#version 450

#include <color/tonemap.glsl>

layout(location = 0) in vec2 vUV;

layout (location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform sampler2D sceneColor;
layout(set = 0, binding = 1) uniform sampler2D bloomColor;

void main()
{
    float intensity = 1.0;

    vec3 scene = texture(sceneColor, vUV).rgb;
    vec3 bloom = texture(bloomColor, vUV).rgb;

    vec3 hdrColor = scene + bloom * intensity;

    vec3 mapped = acesFilm(hdrColor);
    mapped = pow(mapped, vec3(1.0 / 2.2)); // gamma correction
    FragColor = vec4(mapped, 1.0);
}