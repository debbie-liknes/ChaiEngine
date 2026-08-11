#version 450
layout(location = 0) in vec2 vUV;

layout (location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform sampler2D sceneColor;

void main()
{
    float threshold = 0.4;
    vec3 color = texture(sceneColor, vUV).rgb;

    float brightness = max(color.r, max(color.g, color.b));
    float knee = threshold * 0.5; // width of the soft transition
    float soft = brightness - threshold + knee;
    soft = clamp(soft, 0.0, 2.0 * knee);
    soft = soft * soft / (4.0 * knee + 0.0001);
    float contribution = max(soft, brightness - threshold) / max(brightness, 0.0001);
    vec3 bloomColor = color * contribution;
    FragColor = vec4(bloomColor, 1.0);
}