cbuffer dataPSbuffer
{
	float4 inColor;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 col : COLOR;
	float2 tex : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	float4 outCol = ObjTexture.Sample(ObjSamplerState, input.tex);
	outCol[0] *= inColor[0];
	outCol[1] *= inColor[1];
	outCol[2] *= inColor[2];
	outCol[3] *= inColor[3];
	return outCol;
	//return float4(input.col.r * inColor[0], input.col.g * inColor[1], input.col.b * inColor[2], inColor[3]);
}