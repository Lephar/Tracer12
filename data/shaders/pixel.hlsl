struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 tangent : TANGENT;
    float4 normal : NORMAL;
    float4 texcoord : TEXCOORD;
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
};

struct Light
{
    float4 position;
    float4 color;
};

struct Mesh
{
    float4x4 model;
};

cbuffer Constants : register(b0)
{
    uint lightCount;
    uint baseColorIndex;
    uint metallicRoughnessIndex;
    uint normalIndex;
};

ConstantBuffer<Camera> camera : register(b1);
ConstantBuffer<Light> lights[] : register(b2);
ConstantBuffer<Mesh> mesh : register(b3);
SamplerState samplerState : register(s0);
Texture2D textures[] : register(t0);

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    
    output.color = textures[baseColorIndex].Sample(samplerState, input.texcoord.xy);
    
    return output;
}
