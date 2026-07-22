struct PS_INPUT
{
    float4 output : SV_POSITION;
    float3 position : POSITION;
    float4 tangent : TANGENT;
    float3 normal : NORMAL;
    float2 texcoord0 : TEXCOORD0;
    float2 texcoord1 : TEXCOORD1;
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
ConstantBuffer<Lights> lights : register(b2);
ConstantBuffer<Constants> constants : register(b3);
Texture2D textures[] : register(t0);
SamplerState samplerState : register(s0);

PS_OUTPUT main(PS_INPUT input)
{
    float Kc = 1.0f;
    float Kl = 0.7f;
    float Kq = 1.8f;

    float specularFalloff = 32.0f;
    
    float3 ambient = float3(0.1f, 0.1f, 0.1f);
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    
    float3 inputNormal = normalize(input.normal);
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
        
        float lightSpecular = pow(max(dot(inputNormal, halfwayDirection), 0.0f), specularFalloff);
        float lightDiffuse = max(dot(inputNormal, lightDirection), 0.0f);

        diffuse = diffuse + (impact * lightDiffuse * lightColor);
        specular = specular + (impact * lightSpecular * lightColor);
    }

    float4 pixel = textures[constants.baseColorIndex].Sample(samplerState, input.texcoord0);
    float3 color = pixel.rgb;
    float alpha = pixel.a;
    
    PS_OUTPUT output;
    
    output.color = float4((ambient + diffuse + specular) * color, alpha);
    
    return output;
}
