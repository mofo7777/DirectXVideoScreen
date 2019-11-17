//----------------------------------------------------------------------------------------------
// Main.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

#define INPUT_IMAGE L"background.bmp"
#define CONVERTED_IMAGE L"ConvertedImage.bmp"

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
		cD3D11ShaderNV12.ProcessShaderNV12(CONVERTED_IMAGE);
	}

	cD3D11ShaderNV12.OnRelease();
	cWICBitmap.OnRelease();
	CoUninitialize();
}