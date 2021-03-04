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
	float4 uLightPosition = float4(0.0f, 5.0f, -2.0f, 1.0f);
	float3 uAmbientLightColor = float3(0.5f, 0.5f, 0.5f);
	float3 uLightColor = float3(1.0f, 1.0f, 1.0f);

	float4 directionVector = uLightPosition - input.pos2;
	float distanceToLight = length(directionVector);
	float4 lightDirection = normalize(directionVector);
	float directional = clamp(dot(input.normal, lightDirection), 0.0f, 1.0f);
	float3 reflectedVector = reflect(lightDirection.xyz, input.normal.xyz);
	float3 eyeVector = normalize(uLightPosition.xyz - input.pos2.xyz);
	float s = clamp(dot(reflectedVector, eyeVector), 0.0f, 1.0f);
	s = pow(s, 50.0f);
	float3 specLight = float3(s, 0.0f, 0.0f);
	float attenuation = 1.0f / (1.0f + 0.025f * pow(distanceToLight, 2.0f));
	float3 lighting = uAmbientLightColor + (attenuation * (uLightColor * directional));
	float4 finalColor = float4(input.color.rgb * lighting, 1.0f);    // Pass the color directly through the pipeline.
	float4 oldColor = float4(input.color, 1.0f);


	return finalColor;
}
