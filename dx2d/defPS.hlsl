struct VS_OUTPUT
{
	float4 vPosition : SV_POSITION;
	float3 vColor : COLOR;
};

float4 main(VS_OUTPUT Input) : SV_Target
{
	return float4(Input.vColor.r, Input.vColor.g, Input.vColor.b, 1.0);
}