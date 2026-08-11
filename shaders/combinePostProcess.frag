#version 450
layout(location = 0) in vec2 vUV;

layout (location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform sampler2D sceneColor;
layout(set = 0, binding = 1) uniform sampler2D bloomColor;

// ACES filmic tonemap (Narkowicz)
vec3 tonemapACES(vec3 x) {
    const float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    float intensity = 1.0;

    vec3 scene = texture(sceneColor, vUV).rgb;
    vec3 bloom = texture(bloomColor, vUV).rgb;

    vec3 hdrColor = scene + bloom * intensity;

    vec3 mapped = tonemapACES(hdrColor);
    mapped = pow(mapped, vec3(1.0 / 2.2)); // gamma correction
    FragColor = vec4(mapped, 1.0);
}