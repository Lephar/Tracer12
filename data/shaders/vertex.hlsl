struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.Position = mul(projection, mul(view, float4(input.Position, 1.0f)));
    output.Color = float4(input.Color, 1.0f);
    
    return output;
}
