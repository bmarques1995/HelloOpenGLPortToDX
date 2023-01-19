struct PS_In
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

float4 main(PS_In input) : SV_TARGET
{
    return input.color;
}