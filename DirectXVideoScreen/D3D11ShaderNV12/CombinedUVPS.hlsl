//--------------------------------------------------------------------------------------
// CombinedUVPS.hlsl
//--------------------------------------------------------------------------------------
Texture2D txInputUV : register(t0);

SamplerState GenericSampler : register(s0);

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float2 PS(PS_INPUT input, float4 inPos : SV_Position) : SV_Target
{
	return txInputUV.Sample(GenericSampler, input.Tex).rg;
}