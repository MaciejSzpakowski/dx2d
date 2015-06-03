struct VS_INPUT
{
	float4 vPosition : POSITION;
	float3 vColor : COLOR;
	//if layout would define this
	//float OffsetX : SOME_MORE_DATA;
};

struct VS_OUTPUT
{
	float4 vPosition : SV_POSITION;
	float3 vColor : COLOR;
};

VS_OUTPUT main(VS_INPUT Input)
{
	VS_OUTPUT Output;
	/*Output.vPosition.x = Input.vPosition.x + Input.OffsetX;
	Output.vPosition.y = Input.vPosition.y;
	Output.vPosition.z = Input.vPosition.z;
	Output.vPosition.w = Input.vPosition.w;*/
	Output.vPosition = Input.vPosition;
	Output.vColor = Input.vColor;
	return Output;
}