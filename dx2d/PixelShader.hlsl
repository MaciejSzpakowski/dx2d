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
		float4 result = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
			clip(result.a-0.01f);
			return float4(result.b*color[0], result.g*color[1], result.r*color[2], result.a*color[3]);
	}
}
