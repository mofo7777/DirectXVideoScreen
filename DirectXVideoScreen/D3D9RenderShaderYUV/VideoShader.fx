//----------------------------------------------------------------------------------------------
// Shader.fx
//----------------------------------------------------------------------------------------------
float2 m_vTexelSize;
float2 m_vTexelNV12;

texture m_pImageTexture;
texture m_pLumaTexture;
texture m_pChromaCBTexture;
texture m_pChromaCRTexture;
texture m_pYUVTexture;
texture m_pNV12Texture;
texture m_pShiftWidthTexture;
texture m_pShiftHeightTexture;
texture m_pYTexture;
texture m_pUTexture;
texture m_pVTexture;

sampler ImageSampler = sampler_state
{
	Texture = <m_pImageTexture>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Wrap;
	AddressV = Wrap;
};

sampler LumaSampler = sampler_state
{
	Texture = <m_pLumaTexture>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Wrap;
	AddressV = Wrap;
};

sampler ChromaCBSampler = sampler_state
{
	Texture = <m_pChromaCBTexture>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Wrap;
	AddressV = Wrap;
};

sampler ChromaCRSampler = sampler_state
{
	Texture = <m_pChromaCRTexture>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Wrap;
	AddressV = Wrap;
};

sampler YUVSampler = sampler_state
{
	Texture = <m_pYUVTexture>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Wrap;
	AddressV = Wrap;
};

sampler NV12Sampler = sampler_state
{
	Texture = <m_pNV12Texture>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Wrap;
	AddressV = Wrap;
};

sampler1D WidthShiftSampler = sampler_state
{
	Texture = <m_pShiftWidthTexture>;
	MipFilter = Point;
	MinFilter = Point;
	MagFilter = Point;
	AddressU = Wrap;
};

sampler1D HeightShiftSampler = sampler_state
{
	Texture = <m_pShiftHeightTexture>;
	MipFilter = Point;
	MinFilter = Point;
	MagFilter = Point;
	AddressU = Wrap;
};

sampler YSampler = sampler_state
{
	Texture = <m_pYTexture>;
	MipFilter = Point;
	MinFilter = Point;
	MagFilter = Point;
	AddressU = Wrap;
};

sampler USampler = sampler_state
{
	Texture = <m_pUTexture>;
	MipFilter = Point;
	MinFilter = Point;
	MagFilter = Point;
	AddressU = Wrap;
};

sampler VSampler = sampler_state
{
	Texture = <m_pVTexture>;
	MipFilter = Point;
	MinFilter = Point;
	MagFilter = Point;
	AddressU = Wrap;
};

struct VS_INPUT
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

VS_OUTPUT ShaderVS(VS_INPUT i)
{
	VS_OUTPUT o;

	o.Pos = float4(i.Pos, 1.0f);
	o.Tex = float2(i.Tex.x + m_vTexelSize.x, i.Tex.y + m_vTexelSize.y);

	return o;
}

//-----------------------------------------------------------------
float4 ShowRGBPS(VS_OUTPUT i) : COLOR
{
	return tex2D(ImageSampler, i.Tex);
}

technique ShowRGB
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShaderVS();
		PixelShader = compile ps_3_0 ShowRGBPS();
	}
}

//-----------------------------------------------------------------
float4 ShowLumaPS(VS_OUTPUT i) : COLOR
{
	return tex2D(LumaSampler, i.Tex);
}

technique ShowLuma
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShaderVS();
		PixelShader = compile ps_3_0 ShowLumaPS();
	}
}

//-----------------------------------------------------------------
float4 ShowChromaBPS(VS_OUTPUT i) : COLOR
{
	return tex2D(ChromaCBSampler, i.Tex);
}

technique ShowChromaB
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShaderVS();
		PixelShader = compile ps_3_0 ShowChromaBPS();
	}
}

//-----------------------------------------------------------------
float4 ShowChromaRPS(VS_OUTPUT i) : COLOR
{
	return tex2D(ChromaCRSampler, i.Tex);
}

technique ShowChromaR
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShaderVS();
		PixelShader = compile ps_3_0 ShowChromaRPS();
	}
}

//-----------------------------------------------------------------
float4 ShowYUVPS(VS_OUTPUT i) : COLOR
{
	return tex2D(YUVSampler, i.Tex);
}

technique ShowYUV
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShaderVS();
		PixelShader = compile ps_3_0 ShowYUVPS();
	}
}

//-----------------------------------------------------------------
float4 ShowNV12PS(VS_OUTPUT i) : COLOR
{
	return tex2D(NV12Sampler, i.Tex);
}

technique ShowNV12
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShaderVS();
		PixelShader = compile ps_3_0 ShowNV12PS();
	}
}

//-----------------------------------------------------------------
float4 ShowWidthShiftPS(VS_OUTPUT i) : COLOR
{
	float fShift = tex1D(WidthShiftSampler, i.Tex.x).r;

	float4 Color;
	
	if(fShift == 0.0f)
		Color = 0.0f;
	else
		Color = 1.0f;

	return Color;
}

technique ShowWidthShift
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShaderVS();
		PixelShader = compile ps_3_0 ShowWidthShiftPS();
	}
}

//-----------------------------------------------------------------
float4 ShowYPS(VS_OUTPUT i) : COLOR
{
	return tex2D(YSampler, i.Tex);
}

technique ShowY
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShaderVS();
		PixelShader = compile ps_3_0 ShowYPS();
	}
}

//-----------------------------------------------------------------
float4 ShowUPS(VS_OUTPUT i) : COLOR
{
	return tex2D(USampler, i.Tex);
}

technique ShowU
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShaderVS();
		PixelShader = compile ps_3_0 ShowUPS();
	}
}

//-----------------------------------------------------------------
float4 ShowVPS(VS_OUTPUT i) : COLOR
{
	return tex2D(VSampler, i.Tex);
}

technique ShowV
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShaderVS();
		PixelShader = compile ps_3_0 ShowVPS();
	}
}

//-----------------------------------------------------------------
float4 ShowDecodedYUV444PS(VS_OUTPUT i) : COLOR
{
	float fY;
	float fU;
	float fV;

	fY = tex2D(LumaSampler, i.Tex).r - (16.0f / 256.0f);
	fU = tex2D(ChromaCBSampler, i.Tex).r - (128.0f / 256.0f);
	fV = tex2D(ChromaCRSampler, i.Tex).r - (128.0f / 256.0f);

	float4 Color;

	Color.r = clamp((1.164383f * fY + 1.596027f * fV), 0.0f, 255.0f);
	Color.g = clamp((1.164383f * fY - 0.391762f * fU - 0.812968f * fV), 0.0f, 255.0f);
	Color.b = clamp((1.164383f * fY + 2.017232f * fU), 0.0f, 255.0f);
	Color.a = 1.0f;

	return Color;
}

technique ShowDecodedYUV444YUV
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShaderVS();
		PixelShader = compile ps_3_0 ShowDecodedYUV444PS();
	}
}

//-----------------------------------------------------------------
float4 ShowDecodedYUV420PS(VS_OUTPUT i) : COLOR
{
	float fY;
	float fU;
	float fV;

	fY = tex2D(YSampler, i.Tex).r - (16.0f / 256.0f);
	fU = tex2D(USampler, i.Tex).r - (128.0f / 256.0f);
	fV = tex2D(VSampler, i.Tex).r - (128.0f / 256.0f);

	float4 Color;

	Color.r = clamp((1.164383f * fY + 1.596027f * fV), 0.0f, 255.0f);
	Color.g = clamp((1.164383f * fY - 0.391762f * fU - 0.812968f * fV), 0.0f, 255.0f);
	Color.b = clamp((1.164383f * fY + 2.017232f * fU), 0.0f, 255.0f);
	Color.a = 1.0f;

	return Color;
}

technique ShowDecodedYUV420YUV
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShaderVS();
		PixelShader = compile ps_3_0 ShowDecodedYUV420PS();
	}
}

//-----------------------------------------------------------------
struct VS_INPUT_YUV
{
	float3 Pos : POSITION;
	float2 TexY : TEXCOORD0;
	float2 TexU : TEXCOORD1;
	float2 TexV : TEXCOORD2;
	float2 TexUV : TEXCOORD3;
	float2 TexU2 : TEXCOORD4;
	float2 TexV2 : TEXCOORD5;
};

struct VS_OUTPUT_YUV
{
	float4 Pos : POSITION;
	float2 TexY : TEXCOORD0;
	float2 TexU : TEXCOORD1;
	float2 TexV : TEXCOORD2;
	float2 TexUV : TEXCOORD3;
	float2 TexU2 : TEXCOORD4;
	float2 TexV2 : TEXCOORD5;
};

VS_OUTPUT_YUV ShowYUVVS(VS_INPUT_YUV i)
{
	VS_OUTPUT_YUV o;

	o.Pos = float4(i.Pos, 1.0f);
	o.TexY = float2(i.TexY.x + m_vTexelNV12.x, i.TexY.y + m_vTexelNV12.y);
	o.TexUV = float2(i.TexUV.x + m_vTexelNV12.x, i.TexUV.y + m_vTexelNV12.y);

	o.TexU = float2(i.TexU.x + m_vTexelNV12.x, i.TexU.y + m_vTexelNV12.y);
	o.TexV = float2(i.TexV.x + m_vTexelNV12.x, i.TexV.y + m_vTexelNV12.y);
	o.TexU2 = float2(i.TexU2.x + m_vTexelNV12.x, i.TexU2.y + m_vTexelNV12.y);
	o.TexV2 = float2(i.TexV2.x + m_vTexelNV12.x, i.TexV2.y + m_vTexelNV12.y);

	return o;
}

float4 ShowHeightShiftPS(VS_OUTPUT_YUV i) : COLOR
{
	float fShift = tex1D(HeightShiftSampler, i.TexY.y).r;

	float4 Color;

	if(fShift == 0.0f)
		Color = 0.0f;
	else
		Color = 1.0f;

	return Color;
}

technique ShowHeightShift
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShowYUVVS();
		PixelShader = compile ps_3_0 ShowHeightShiftPS();
	}
}

//-----------------------------------------------------------------
float4 ShowDecodedYUVPS(VS_OUTPUT_YUV i) : COLOR
{
	float fShift = tex1D(HeightShiftSampler, i.TexY.y).r;

	float fY;
	float fU;
	float fV;

	fY = tex2D(YUVSampler, i.TexY).r - (16.0f / 256.0f);

	if(fShift == 0.0f)
	{
		fU = tex2D(YUVSampler, i.TexU).r - (128.0f / 256.0f);

		fV = tex2D(YUVSampler, i.TexV).r - (128.0f / 256.0f);
	}
	else
	{
		fU = tex2D(YUVSampler, i.TexU2).r - (128.0f / 256.0f);
		fV = tex2D(YUVSampler, i.TexV2).r - (128.0f / 256.0f);
	}

	float4 Color;

	Color.r = clamp((1.164383f * fY + 1.596027f * fV), 0.0f, 255.0f);
	Color.g = clamp((1.164383f * fY - 0.391762f * fU - 0.812968f * fV), 0.0f, 255.0f);
	Color.b = clamp((1.164383f * fY + 2.017232f * fU), 0.0f, 255.0f);
	Color.a = 1.0f;

	return Color;
}

technique ShowDecodedYUV
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShowYUVVS();
		PixelShader = compile ps_3_0 ShowDecodedYUVPS();
	}
}

//-----------------------------------------------------------------
float4 ShowDecodedNV12PS(VS_OUTPUT_YUV i) : COLOR
{
	float fShift = tex1D(WidthShiftSampler, i.TexY.x).r;

	float fY;
	float fU;
	float fV;

	fY = tex2D(NV12Sampler, i.TexY).r - (16.0f / 256.0f);

	if(fShift == 0.0f)
	{
		fU = tex2D(NV12Sampler, float2(i.TexUV.x - 2.0f * m_vTexelNV12.x, i.TexUV.y)).r - (128.0f / 256.0f);
		fV = tex2D(NV12Sampler, i.TexUV).r - (128.0f / 256.0f);
	}
	else
	{
		fU = tex2D(NV12Sampler, i.TexUV).r - (128.0f / 256.0f);
		fV = tex2D(NV12Sampler, float2(i.TexUV.x + 2.0f * m_vTexelNV12.x, i.TexUV.y)).r - (128.0f / 256.0f);
	}

	float4 Color;

	Color.r = clamp((1.164383f * fY + 1.596027f * fV), 0.0f, 255.0f);
	Color.g = clamp((1.164383f * fY - 0.391762f * fU - 0.812968f * fV), 0.0f, 255.0f);
	Color.b = clamp((1.164383f * fY + 2.017232f * fU), 0.0f, 255.0f);
	Color.a = 1.0f;

	return Color;
}

technique ShowDecodedNV12
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShowYUVVS();
		PixelShader = compile ps_3_0 ShowDecodedNV12PS();
	}
}