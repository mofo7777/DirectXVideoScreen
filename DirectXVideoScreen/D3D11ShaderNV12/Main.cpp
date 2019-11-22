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
		// CONVERT_INPUT_SHADER
		// CONVERT_LUMA_SHADER
		// CONVERT_CHROMA_SHADER
		// CONVERT_LUMACHROMA_SHADER
		// CONVERT_NV12_SHADER (not implemented)
		cD3D11ShaderNV12.ProcessShaderNV12(CONVERTED1_IMAGE, CONVERTED2_IMAGE, CONVERTED3_IMAGE, CD3D11ShaderNV12::SHADER_CONVERSION::CONVERT_LUMACHROMA_SHADER);
	}

	cD3D11ShaderNV12.OnRelease();
	cWICBitmap.OnRelease();
	CoUninitialize();
}