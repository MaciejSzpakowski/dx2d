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
	if (input.Col.r == 0)
	{
		float4 result = ObjTexture.Sample(ObjSamplerState,input.TexCoord);
			clip(result.a - 0.001f);
			return result * color;
	}
	else
	{
		return float4(color.r, color.g, color.b, 1);
	}
}
