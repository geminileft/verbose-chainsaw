cbuffer ModelViewProjectionConstantBuffer22asw346 : register(b1)
{
	matrix model;
	matrix view;
	matrix projection;
	matrix normal;
	float4 lightDirection;
};

struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR;
	float3 normal : NORMAL;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 pos2 : POSITION;
	float3 color : COLOR;
	float4 normal : NORMAL;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output = (PixelShaderInput)0;
	float4 pos = float4(input.pos, 1.0f);

	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;
	output.pos2 = pos;
	output.color = input.color;

	output.normal = mul(float4(input.normal, 0.0f), normal);

	return output;
}
