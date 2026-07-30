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
    float3x3 tangentBitangentNormal : TANGENT_BITANGENT_NORMAL;
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
    float3 tangent = normalize(mul(mesh.normal, input.tangent).xyz);
    float3 normal = normalize(mul(mesh.normal, input.normal).xyz);
    float tangentSign = input.tangent.w;
    float3 bitangent = normalize(cross(normal, tangent)) * tangentSign;
    
    output.position = position.xyz;
    output.tangent = float4(tangent, tangentSign);
    output.normal = normal;
    output.texcoord0 = input.texcoord.xy;
    output.texcoord1 = input.texcoord.zw;
    
    output.tangentBitangentNormal = float3x3(
        tangent.x, bitangent.x, normal.x,
        tangent.y, bitangent.y, normal.y,
        tangent.z, bitangent.z, normal.z
    );
    
    output.output = mul(camera.projectionView, position);
    
    return output;
}
