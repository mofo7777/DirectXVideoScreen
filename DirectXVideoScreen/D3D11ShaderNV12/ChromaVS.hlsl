//--------------------------------------------------------------------------------------
// ChromaVS.hlsl
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 TexUV1 : TEXCOORD0;
	float2 TexUV2 : TEXCOORD1;
	float2 TexUV3 : TEXCOORD2;
	float2 TexUV4 : TEXCOORD3;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(uint vI : SV_VERTEXID, float4 inPos : SV_Position)
{
	PS_INPUT output;

	uint uiPosX = asuint(inPos.x);
	uint uiPosY = asuint(inPos.y);

	float2 texcoord = float2(vI & 1, vI >> 1);

	output.Pos = float4((texcoord.x - 0.5f) * 2.0f, -(texcoord.y - 0.5f) * 2.0f, 0.0f, 1.0f);

	if(uiPosX % 2)
	{
		if(uiPosY % 2)
		{
			output.TexUV1 = float2(texcoord.x, texcoord.y);
			output.TexUV2 = float2(texcoord.x + 1.0f, texcoord.y);
			output.TexUV3 = float2(texcoord.x, texcoord.y + 1.0f);
			output.TexUV4 = float2(texcoord.x + 1.0f, texcoord.y + 1.0f);
		}
		else
		{
			output.TexUV1 = float2(texcoord.x, texcoord.y - 1.0f);
			output.TexUV2 = float2(texcoord.x + 1.0f, texcoord.y - 1.0f);
			output.TexUV3 = float2(texcoord.x, texcoord.y);
			output.TexUV4 = float2(texcoord.x + 1.0f, texcoord.y);
		}
	}
	else
	{
		if(uiPosY % 2)
		{
			output.TexUV1 = float2(texcoord.x - 1.0f, texcoord.y);
			output.TexUV2 = float2(texcoord.x, texcoord.y);
			output.TexUV3 = float2(texcoord.x - 1.0f, texcoord.y + 1.0f);
			output.TexUV4 = float2(texcoord.x, texcoord.y + 1.0f);
		}
		else
		{
			output.TexUV1 = float2(texcoord.x - 1.0f, texcoord.y - 1.0f);
			output.TexUV2 = float2(texcoord.x, texcoord.y - 1.0f);
			output.TexUV3 = float2(texcoord.x - 1.0f, texcoord.y);
			output.TexUV4 = float2(texcoord.x, texcoord.y);
		}
	}

	return output;
}