cbuffer cbBufferVS
{
	float4x4 worldViewProj;
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
	output.Pos = mul(inPos, worldViewProj);
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