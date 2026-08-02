struct VS_INPUT
{
    float4 position : POSITION;
    float4 tangent : TANGENT;
    float4 normal : NORMAL;
    float4 texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 tangent : TANGENT;
    float4 normal : NORMAL;
    float4 texcoord : TEXCOORD;
    float4 output : SV_Position;
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
    
    output.position = mul(mesh.model, input.position);
    output.tangent = float4(mul(mesh.normal, input.tangent).xyz, input.tangent.w);
    output.normal = mul(mesh.normal, input.normal);
    output.texcoord = input.texcoord;
    
    output.output = mul(camera.projectionView, output.position);
    
    return output;
}
