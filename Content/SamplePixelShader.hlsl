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
	float4 uLightPosition = float4(0.0f, 5.0f, 0.0f, 1.0f);
	float4 directionVector = uLightPosition - input.pos2;
	float4 lightDirection = normalize(directionVector);
	float4 diffuse = { 0.0f, 1.0f, 0.0f, 1.0f};
	float4 ambient = {0.0, 0.0, 0.0, 1.0};
	return ambient + float4(input.color, 1.0) * saturate(dot(lightDirection, input.normal));
}
