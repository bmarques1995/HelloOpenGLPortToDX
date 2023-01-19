#pragma pack_matrix( row_major )

struct VS_In
{
	float3 pos : POSITION;
	float4 color : COLOR;
};

struct VS_Out
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

VS_Out main( VS_In input )
{
	VS_Out output;
	output.pos = float4(input.pos, 1.0f);
	output.color = input.color;
	return output;
}