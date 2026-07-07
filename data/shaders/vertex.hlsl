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

cbuffer ConstantBuffer : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = mul(projection, mul(view, input.position));
    output.tangent = input.tangent;
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    
    return output;
}
