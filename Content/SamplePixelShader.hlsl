cbuffer ModelViewProjectionConstantBuffer22asw346 : register(b1)
{
	matrix model;
	matrix view;
	matrix projection;
	matrix normal;
	float4 lightDirection;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 pos2 : POSITION;
	float3 color : COLOR;
	float4 normal : NORMAL;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 ambient = {0.0, 0.0, 0.0, 1.0};
	return ambient + float4(input.color, 1.0) * saturate(dot(input.normal, lightDirection));
}
