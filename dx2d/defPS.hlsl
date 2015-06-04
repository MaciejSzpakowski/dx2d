cbuffer dataPSbuffer
{
	float4 inColor;
};

struct VS_OUTPUT
{
	float4 vPosition : SV_POSITION;
	float3 vColor : COLOR;
};

float4 main(VS_OUTPUT Input) : SV_Target
{
	/*return float4(Input.vColor.r * inColor[0], Input.vColor.g * inColor[1], 
		Input.vColor.b * inColor[2], 1.0 * inColor[3]);*/
	return inColor;
}