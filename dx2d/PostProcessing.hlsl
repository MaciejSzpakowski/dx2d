cbuffer cbBufferPS
{
	float4 color;
	int4 vars;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Col : COLOR;
	float2 TexCoord : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float2 uv = float2(input.TexCoord[0], input.TexCoord[1]) * 200;
	int2 iuv = trunc(uv);
	uv = iuv / 200.0f;
	float4 result = ObjTexture.Sample(ObjSamplerState,uv);
	return result * color;
}
