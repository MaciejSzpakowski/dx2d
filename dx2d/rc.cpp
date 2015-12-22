namespace dx2d
{
	extern const char rc_PixelShader[] = R"(
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
		float4 result = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
		clip(result.a - 0.001f);
		return result * color;
	}
	else
	{
		return float4(color.r, color.g, color.b, 1);
	}
}
)";

	extern const char rc_PostProcessing[] = R"(
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
	float4 result = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
	clip(result.a - 0.001f);
	return result * color;
}
)";

	extern const char rc_VertexShader[] = R"(
cbuffer cbBufferVS
{
	float4x4 transformation;
};

cbuffer cbBufferUV
{
	//left,top,right,bottom
	float4 uv;
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
	output.Pos = mul(inPos, transformation);
	output.Col = inCol;
	output.TexCoord = inTexCoord;
	//
	if (inTexCoord[0] == 0 && inTexCoord[1] == 0)
	{
		output.TexCoord = float2(uv[0], 1-uv[1]);
	}
	if (inTexCoord[0] == 1 && inTexCoord[1] == 0)
	{
		output.TexCoord = float2(uv[2], 1-uv[1]);
	}
	if (inTexCoord[0] == 0 && inTexCoord[1] == 1)
	{
		output.TexCoord = float2(uv[0], 1-uv[3]);
	}
	if (inTexCoord[0] == 1 && inTexCoord[1] == 1)
	{
		output.TexCoord = float2(uv[2], 1-uv[3]);
	}

	return output;
}
)";

	//my own resource file :)
	extern const unsigned char rc_font[] = {
#include "default_font.txt"
	};
}