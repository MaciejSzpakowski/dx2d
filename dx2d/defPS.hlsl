cbuffer dataPSbuffer
{
	float4 inColor;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 col : COLOR;
	float2 tex : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	return float4(input.col.r * inColor[0], input.col.g * inColor[1],
		input.col.b * inColor[2], inColor[3]);
}