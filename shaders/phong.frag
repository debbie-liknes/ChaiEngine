
#version 330 core

// Light structure - matches your C++ Light struct
struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;
    vec3 attenuation;   // constant, linear, quadratic
    float innerCone;    // cosine of inner cone angle
    float outerCone;    // cosine of outer cone angle
    int enabled;
};

// Inputs from vertex shader
in vec3 FragPos;
in vec3 Normal;

// Output
out vec4 FragColor;

// Material properties
uniform mat4 u_view;
uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_specular;

uniform Light u_lights[16];
uniform int u_lightCount;

// Light type constants
const int DIRECTIONAL_LIGHT = 0;
const int POINT_LIGHT = 1;
const int SPOT_LIGHT = 2;

vec3 calculatePhong(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir;
    float attenuation = 1.0;
    
    // Calculate light direction and attenuation based on light type
    if (light.type == DIRECTIONAL_LIGHT) {
        lightDir = normalize(-light.direction);
        attenuation = 1.0; // No attenuation for directional lights
    } else {
        // Point and spot lights
        lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);
        
        // Calculate attenuation
        attenuation = 1.0 / (light.attenuation.x + 
                           light.attenuation.y * distance + 
                           light.attenuation.z * distance * distance);
        
        // Apply range cutoff
        if (distance > light.range) {
            attenuation = 0.0;
        }
        
        // Spot light cone calculation
        if (light.type == SPOT_LIGHT) {
            vec3 spotDir = normalize(-light.direction);
            float theta = dot(lightDir, spotDir);
            
            if (theta > light.outerCone) {
                // Smooth transition between inner and outer cone
                float epsilon = light.innerCone - light.outerCone;
                float intensity = clamp((theta - light.outerCone) / epsilon, 0.0, 1.0);
                attenuation *= intensity;
            } else {
                attenuation = 0.0; // Outside the cone
            }
        }
    }

    
    // Ambient component
    vec3 ambient = u_ambient * light.color;
    
    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_diffuse * light.color;
    
    // Specular component
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // Using default shininess of 32
    vec3 specular = spec * u_specular * light.color;
    
    // Combine all components with attenuation and intensity
    return (ambient + diffuse + specular) * attenuation * light.intensity;
}

void main() {
    vec3 viewPos = vec3(inverse(u_view)[3]); // Extract camera position from view matrix
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);
    
    vec3 result = vec3(0.0);
    
    FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    // Calculate lighting contribution from each enabled light
    for (int i = 0; i < u_lightCount && i < 16; i++) {
        if (u_lights[i].enabled == 1) {
            result += calculatePhong(u_lights[i], Normal, FragPos, viewDir);
        }
    }

    FragColor = vec4(result, 1.0);
}