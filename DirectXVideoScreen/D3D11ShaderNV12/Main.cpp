//----------------------------------------------------------------------------------------------
// Main.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

#define INPUT_IMAGE L"background.bmp"
#define CONVERTED1_IMAGE L"ConvertedImage1.bmp"
#define CONVERTED2_IMAGE L"ConvertedImage2.bmp"
#define CONVERTED3_IMAGE L"ConvertedImage3.bmp"

void main()
{
	CWICBitmap cWICBitmap;
	CWICBitmap cWICBitmapUV;

	if(FAILED(CoInitializeEx(NULL, COINIT_DISABLE_OLE1DDE)))
		return;

	if(FAILED(cWICBitmap.InitImage(INPUT_IMAGE)))
	{
		CoUninitialize();
		return;
	}

	CD3D11ShaderNV12 cD3D11ShaderNV12;

	if(cD3D11ShaderNV12.InitShaderNV12(cWICBitmap) == S_OK)
	{
		// CONVERT_INPUT_SHADER (OK)
		// CONVERT_SMALL_INPUT_SHADER (OK)
		// CONVERT_LUMA_SHADER (OK)
		// CONVERT_CHROMA_SHADER (OK)
		// CONVERT_LUMACHROMA_SHADER (OK)
		// CONVERT_LUMACHROMACBCR_SHADER (OK)
		// CONVERT_CHROMADOWNSAMPLED_SHADER (OK)
		// CONVERT_CHROMADOWNSAMPLED2_SHADER (OK)
		// CONVERT_FAKE_NV12_SHADER (OK)
		// CONVERT_NV12_SHADER (not implemented)
		cD3D11ShaderNV12.ProcessShaderNV12(cWICBitmap.GetWidth(), cWICBitmap.GetHeight(), CONVERTED1_IMAGE, CONVERTED2_IMAGE, CONVERTED3_IMAGE, CD3D11ShaderNV12::SHADER_CONVERSION::CONVERT_FAKE_NV12_SHADER);
	}

	cD3D11ShaderNV12.OnRelease();
	cWICBitmap.OnRelease();
	cWICBitmapUV.OnRelease();
	CoUninitialize();
}