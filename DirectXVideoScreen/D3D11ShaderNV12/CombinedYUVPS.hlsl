//--------------------------------------------------------------------------------------
// CombinedYUVPS.hlsl
//--------------------------------------------------------------------------------------
Texture2D txInputY : register(t0);
Texture2D txInputU : register(t1);
Texture2D txInputV : register(t2);

SamplerState GenericSampler : register(s0);

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input, float4 inPos : SV_Position) : SV_Target
{
	uint uiPosX = asuint(inPos.x);
	uint uiPosY = asuint(inPos.y);

	if(uiPosY < 640)
	{
		return txInputY.Sample(GenericSampler, input.Tex);
	}
	else
	{
		if(uiPosY % 2)
		{
			return txInputU.Sample(GenericSampler, input.Tex);
		}
		else
		{
			return txInputV.Sample(GenericSampler, input.Tex);
		}
	}
}