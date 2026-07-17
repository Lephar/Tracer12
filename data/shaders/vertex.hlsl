struct VS_INPUT
{
    float4 position : POSITION;
    float4 tangent : TANGENT;
    float4 normal : NORMAL;
    float4 texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 tangent : TANGENT;
    float4 normal : NORMAL;
    float4 texcoord : TEXCOORD;
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

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = mul(camera.projection, mul(camera.view, mul(mesh.model, input.position)));
    output.tangent = input.tangent;
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    
    return output;
}
