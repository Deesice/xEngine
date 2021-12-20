#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 outColor;

uniform sampler2D ourTexture;

struct PointLight {    
    vec3 position;
    vec3 color;
    float radius;
};
uniform int lightCount;
uniform PointLight pointLights[1000];
uniform vec3 ambientColor;
uniform float globalLightIntensity;

void main()
{
    vec3 norm = normalize(Normal);

    vec3 totalLight = vec3(0);
    for (int i = 0; i < lightCount; i++)
    {
        float distance = length(pointLights[i].position - FragPos);
        if (distance > pointLights[i].radius)
            continue;

        vec3 lightDir = (pointLights[i].position - FragPos) / distance;
        float diffStrength = max(dot(norm, lightDir), 0.0);

        float attenuation = globalLightIntensity / (1
                        + 4.5 * distance / pointLights[i].radius
                        + 75 * distance * distance / pointLights[i].radius / pointLights[i].radius);

        totalLight += diffStrength * attenuation * pointLights[i].color;
    }    

    //outColor = vec4(totalLight, 1);
    outColor = vec4(totalLight * texture(ourTexture, TexCoord).xyz, 1.0f);
}