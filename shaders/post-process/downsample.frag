#version 450
layout(location = 0) in vec2 vUV;
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 0) uniform sampler2D srcTex;
layout(push_constant) uniform PC { vec2 srcTexelSize; } pc;

void main() {
    vec2 uv = vUV;
    vec2 o = pc.srcTexelSize;

    vec3 center = texture(srcTex, uv).rgb;
    vec3 c1 = texture(srcTex, uv + vec2(-o.x, -o.y)).rgb;
    vec3 c2 = texture(srcTex, uv + vec2( o.x, -o.y)).rgb;
    vec3 c3 = texture(srcTex, uv + vec2(-o.x,  o.y)).rgb;
    vec3 c4 = texture(srcTex, uv + vec2( o.x,  o.y)).rgb;
    vec3 c5 = texture(srcTex, uv + vec2(-2.0*o.x, 0.0)).rgb;
    vec3 c6 = texture(srcTex, uv + vec2( 2.0*o.x, 0.0)).rgb;
    vec3 c7 = texture(srcTex, uv + vec2(0.0, -2.0*o.y)).rgb;
    vec3 c8 = texture(srcTex, uv + vec2(0.0,  2.0*o.y)).rgb;

    vec3 result = center * 0.125;
    result += (c1 + c2 + c3 + c4) * 0.125;
    result += (c5 + c6 + c7 + c8) * 0.0625;

    FragColor = vec4(result, 1.0);
}