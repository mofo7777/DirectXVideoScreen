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
	float4 InputColor = txInput.Sample(GenericSampler, input.Tex);

	// Range 0-255
	float ColorY = (0.257f * InputColor.r + 0.504f * InputColor.g + 0.098f * InputColor.b) + (16 / 256.0f);

	// BT.709
	//float ColorY = 0.2126f * InputColor.r + 0.7152f * InputColor.g + 0.0722f * InputColor.b;

	ColorY = clamp(ColorY, 0.0f, 255.0f);

	return ColorY;
}