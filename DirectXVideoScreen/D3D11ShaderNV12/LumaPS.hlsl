//--------------------------------------------------------------------------------------
// LumaPS.hlsl
//--------------------------------------------------------------------------------------
Texture2D txInput : register(t0);

SamplerState GenericSampler : register(s0);

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float PS(PS_INPUT input) : SV_Target
{
	// BT.709
	float4 InputColor = txInput.Sample(GenericSampler, input.Tex);

	float Color = 0.2126f * InputColor.r + 0.7152f * InputColor.g + 0.0722f * InputColor.b;

	Color = clamp(Color, 0.0f, 256.0f);

	return Color;
}