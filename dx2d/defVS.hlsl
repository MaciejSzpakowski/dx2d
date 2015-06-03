cbuffer cbPerObject
{
	float4x4 WorldViewProj;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float3 col : COLOR;
	//if layout would define this
	//float OffsetX : SOME_MORE_DATA;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 col : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.pos = mul(input.pos, WorldViewProj);
	//output.pos.x = input.pos.x + input.OffsetX;
	output.col = input.col;
	return output;
}