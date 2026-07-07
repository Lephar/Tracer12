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

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    
    output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    
    return output;
}
