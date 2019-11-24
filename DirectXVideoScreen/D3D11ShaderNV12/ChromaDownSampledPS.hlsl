//--------------------------------------------------------------------------------------
// ChromaDownSampledPS.hlsl
//--------------------------------------------------------------------------------------
Texture2D txInput : register(t0);

SamplerState GenericSampler : register(s0);

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 TexUV1 : TEXCOORD0;
	float2 TexUV2 : TEXCOORD1;
	float2 TexUV3 : TEXCOORD2;
	float2 TexUV4 : TEXCOORD3;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float2 PS(PS_INPUT input) : SV_Target
{
	float2 ColorUV1 = txInput.Sample(GenericSampler, input.TexUV1);
	float2 ColorUV2 = txInput.Sample(GenericSampler, input.TexUV2);
	float2 ColorUV3 = txInput.Sample(GenericSampler, input.TexUV3);
	float2 ColorUV4 = txInput.Sample(GenericSampler, input.TexUV4);

	float ColorU = (ColorUV1.r + ColorUV2.r + ColorUV3.r + ColorUV4.r) / 4.0f;
	float ColorV = (ColorUV1.g + ColorUV2.g + ColorUV3.g + ColorUV4.g) / 4.0f;

	ColorU = clamp(ColorU, 0.0f, 255.0f);
	ColorV = clamp(ColorV, 0.0f, 255.0f);

	return float2(ColorU, ColorV);
}