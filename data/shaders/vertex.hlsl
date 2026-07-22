struct VS_INPUT
{
    float4 position : POSITION;
    float4 tangent : TANGENT;
    float4 normal : NORMAL;
    float4 texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 output : SV_POSITION;
    float3 position : POSITION;
    float4 tangent : TANGENT;
    float3 normal : NORMAL;
    float2 texcoord0 : TEXCOORD0;
    float2 texcoord1 : TEXCOORD1;
};

struct Mesh
{
    float4x4 model;
    float4x4 normal;
};

struct Camera
{
    float4x4 view;
    float4x4 projection;
    float4x4 projectionView;
    float4 position;
    float4 properties;
};

ConstantBuffer<Mesh> mesh : register(b0);
ConstantBuffer<Camera> camera : register(b1);

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 position = mul(mesh.model, input.position);
    
    output.position = position.xyz;
    output.tangent = input.tangent;
    output.normal = mul(mesh.normal, input.normal).xyz;
    output.texcoord0 = input.texcoord.xy;
    output.texcoord1 = input.texcoord.zw;
    
    output.output = mul(camera.projectionView, position);
    
    return output;
}
