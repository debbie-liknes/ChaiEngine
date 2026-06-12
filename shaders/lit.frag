#version 450
layout(push_constant) uniform Push {
    mat4 mvp;
    vec4 color;
} pc;

layout(set = 2, binding = 0) uniform Light { 
    vec4 direction;
    vec4 color;
} lightData;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUV;
layout(location = 0) out vec4 outColor;
layout(set = 1, binding = 0) uniform sampler2D tex;

void main() {
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(-lightData.direction.xyz);
    float diff = max(dot(N, L), 0.0); // Lambert

    vec3 albedo = texture(tex, fragUV).rgb;
    vec3 ambient = 0.1 * albedo;                // so shadowed faces arent pure black
    vec3 lit = albedo * lightData.color.rgb * diff;

    outColor = vec4(ambient + lit, 1.0);
}