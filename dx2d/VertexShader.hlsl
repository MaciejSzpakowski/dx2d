cbuffer cbBufferVS
{
	float4x4 worldViewProj;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Col : COLOR;
	float2 TexCoord : TEXCOORD;
};

VS_OUTPUT main(float4 inPos : POSITION, float3 inCol : COLOR, float2 inTexCoord : TEXCOORD)
{
	VS_OUTPUT output;
	output.Pos = mul(inPos, worldViewProj);
	output.Col = inCol;
	output.TexCoord = inTexCoord;

	return output;
}