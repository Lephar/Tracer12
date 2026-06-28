struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

struct PS_OUTPUT
{
    float4 Color : SV_TARGET;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    output.Color = input.Color;
    return output;
}
