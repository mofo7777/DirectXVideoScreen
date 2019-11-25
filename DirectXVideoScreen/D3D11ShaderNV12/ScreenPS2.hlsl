//--------------------------------------------------------------------------------------
// ScreenPS2.hlsl
//--------------------------------------------------------------------------------------
Texture2D txInputU : register(t0);
Texture2D txInputV : register(t1);

SamplerState GenericSampler : register(s0);

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

struct PS_OUTPUT
{
	float ColorU : SV_Target0;
	float ColorV: SV_Target1;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT PS(PS_INPUT input) : SV_Target
{
	PS_OUTPUT output;

	output.ColorU = txInputU.Sample(GenericSampler, input.Tex);
	output.ColorV = txInputV.Sample(GenericSampler, input.Tex);

	return output;
}