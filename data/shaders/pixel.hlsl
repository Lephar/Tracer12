struct PS_INPUT
{
    float4 output : SV_POSITION;
    float3 position : POSITION;
    float4 tangent : TANGENT;
    float3 normal : NORMAL;
    float2 texcoord0 : TEXCOORD0;
    float2 texcoord1 : TEXCOORD1;
    float3x3 tangentBitangentNormal : TANGENT_BITANGENT_NORMAL;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

struct Camera
{
    float4x4 view;
    float4x4 projection;
    float4x4 projectionView;
    float4 position;
    float4 properties;
};

struct Material
{
    float4 baseColorFactor;
    float3 metallicRoughnessNormal;
};

#define LIGHTS_MAX 24

struct Light
{
    float4 position;
    float4 color;
};

struct Lights
{
    Light lights[LIGHTS_MAX];
};

struct Constants
{
    uint lightCount;
    uint baseColorIndex;
    uint metallicRoughnessIndex;
    uint normalIndex;
};

ConstantBuffer<Camera> camera : register(b1);
ConstantBuffer<Material> material : register(b2);
ConstantBuffer<Lights> lights : register(b3);
ConstantBuffer<Constants> constants : register(b4);
Texture2D textures[] : register(t0);
SamplerState samplerState : register(s0);

PS_OUTPUT main(PS_INPUT input)
{
    float Kc = 1.0f;
    float Kl = 0.35;
    float Kq = 0.44;

    float specularFalloff = 32.0f;
    
    float3 ambient = float3(0.2f, 0.2f, 0.2f);
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    
    float3 normalScale = float3(material.metallicRoughnessNormal.z, material.metallicRoughnessNormal.z, 1.0f);
    float3 textureNormal = normalize((textures[constants.normalIndex].Sample(samplerState, input.texcoord0).xyz * 2.0f - 1.0f) * normalScale);
    float3 normal = normalize(mul(input.tangentBitangentNormal, textureNormal));
    
    float3 viewDirection = normalize(camera.position.xyz - input.position);
    
    uint lightCount = min(constants.lightCount, LIGHTS_MAX);
    
    for (uint lightIndex = 0; lightIndex < lightCount; lightIndex++)
    {
        Light light = lights.lights[lightIndex];
        
        float3 lightPosition = light.position.xyz;
        float3 lightVector = lightPosition - input.position;
        float3 lightDirection = normalize(lightVector);
        float lightDistance = length(lightVector);

        float3 halfwayDirection = normalize(viewDirection + lightDirection);
        
        float3 lightColor = light.color.rgb;
        float intensity = light.color.a;

        float attenuation = Kc + Kl * lightDistance + Kq * lightDistance * lightDistance;
        float impact = intensity / attenuation;
        
        float lightSpecular = pow(max(dot(normal, halfwayDirection), 0.0f), specularFalloff);
        float lightDiffuse = max(dot(normal, lightDirection), 0.0f);

        diffuse = diffuse + (impact * lightDiffuse * lightColor);
        specular = specular + (impact * lightSpecular * lightColor);
    }

    float4 pixel = textures[constants.baseColorIndex].Sample(samplerState, input.texcoord0);
    float3 color = float3(material.baseColorFactor.r * pixel.r, material.baseColorFactor.g * pixel.g, material.baseColorFactor.b * pixel.b);
    float alpha = material.baseColorFactor.a * pixel.a;
    
    PS_OUTPUT output;
    
    output.color = float4((ambient + diffuse + specular) * color, alpha);
    //output.color = float4(0.5f * normal + 0.5f, 1.0f);
    
    return output;
}
