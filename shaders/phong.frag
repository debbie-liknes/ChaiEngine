#version 420 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

out vec4 FragColor;

layout(std140, binding = 0) uniform PerFrameUniforms
{
    mat4 u_view;
    mat4 u_projection;
};

struct ShaderLightData {
    vec4 positionAndType;   // xyz = pos/dir, w = type
    vec4 color;             // rgb = color, a = intensity
    vec4 directionAndRange; // xyz = direction, w = range
    vec4 spotParams;        // x = inner cone, y = outer cone
};

layout(std140, binding = 2) uniform LightingData {
    ShaderLightData lights[8];
    int numLights;
} uLighting;

uniform vec3 u_DiffuseColor;
uniform vec3 u_SpecularColor;
uniform float u_Shininess;

float CalculateAttenuation(float distance, float range) {
    if (distance >= range) return 0.0;

    float attenuation = 1.0 - (distance / range);
    attenuation = attenuation * attenuation;

    return attenuation;
}

vec3 CalculateLight(ShaderLightData light, vec3 worldPos, vec3 normal, vec3 viewDir, vec3 albedo) {
    vec3 lightDir;
    float attenuation = 1.0;

    // Get light direction and attenuation based on type
    int lightType = int(light.positionAndType.w);

    if (lightType == 0) { // Directional
        lightDir = normalize(-light.directionAndRange.xyz);
        attenuation = 1.0; // No falloff
    }
    else if (lightType == 1) { // Point
        vec3 lightPos = light.positionAndType.xyz;
        vec3 toLight = lightPos - worldPos;
        float distance = length(toLight);
        lightDir = normalize(toLight);

        // Attenuation
        float range = light.directionAndRange.w;
        attenuation = CalculateAttenuation(distance, range);
    }
    else if (lightType == 2) { // Spot
        vec3 lightPos = light.positionAndType.xyz;
        vec3 toLight = lightPos - worldPos;
        float distance = length(toLight);
        lightDir = normalize(toLight);

        // Distance attenuation
        float range = light.directionAndRange.w;
        attenuation = CalculateAttenuation(distance, range);

        // Spot cone attenuation
        vec3 spotDir = normalize(light.directionAndRange.xyz);
        float theta = dot(lightDir, -spotDir);
        float innerCone = light.spotParams.x;
        float outerCone = light.spotParams.y;
        float spotEffect = smoothstep(outerCone, innerCone, theta);

        attenuation *= spotEffect;
    }

    // Early out if light doesn't reach here
    if (attenuation < 0.001) return vec3(0.0);

    // Diffuse lighting (Lambertian)
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = albedo * light.color.rgb * NdotL;

    // Specular (Blinn-Phong)
    vec3 halfDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfDir), 0.0);
    float specular = pow(NdotH, u_Shininess); // Use your uniform shininess
    vec3 specularColor = light.color.rgb * specular * u_SpecularColor; // Use your specular color

    return (diffuse + specularColor) * light.color.a * attenuation; // .a is intensity
}

void main()
{
    // Use your diffuse color as albedo for now
    vec3 albedo = u_DiffuseColor;

    // Normalize normal
    vec3 normal = normalize(v_Normal);

    // Get camera position from view matrix inverse
    vec3 viewPos = vec3(inverse(u_view)[3]);
    vec3 viewDir = normalize(viewPos - v_FragPos);

    // Ambient lighting (simple constant for now)
    vec3 ambient = albedo * 0.03;

    // Accumulate light contributions
    vec3 lighting = ambient;

    for (int i = 0; i < uLighting.numLights; ++i) {
        lighting += CalculateLight(
        uLighting.lights[i],
        v_FragPos,
        normal,
        viewDir,
        albedo
        );
    }

    // Final color
    FragColor = vec4(lighting, 1.0);
}