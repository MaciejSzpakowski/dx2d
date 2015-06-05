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
	if (color.a == 0)
	{
		return color;
	}
	else
	{
		return ObjTexture.Sample(ObjSamplerState, input.TexCoord);
	}
}
