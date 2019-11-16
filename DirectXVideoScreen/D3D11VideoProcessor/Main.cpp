//----------------------------------------------------------------------------------------------
// Main.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

#define INPUT_IMAGE L"background.bmp"
#define CONVERTED_IMAGE L"ConvertedImage.bmp"

void main()
{
	CWICBitmap cWICBitmap;

	if(FAILED(CoInitializeEx(NULL, COINITBASE_MULTITHREADED | COINIT_DISABLE_OLE1DDE)))
		return;

	if(FAILED(cWICBitmap.InitImage(INPUT_IMAGE)))
	{
		CoUninitialize();
		return;
	}

	CD3D11VideoProcessor cD3D11VideoProcessor;

	// DXGI_FORMAT_NV12
	// DXGI_FORMAT_YUY2
	if(cD3D11VideoProcessor.InitDXVA2(cWICBitmap.GetWidth(), cWICBitmap.GetHeight(), DXGI_FORMAT_NV12) == S_OK)
	{
		cD3D11VideoProcessor.ProcessImageConversion(cWICBitmap, CONVERTED_IMAGE);
	}

	cD3D11VideoProcessor.OnRelease();
	cWICBitmap.OnRelease();
	CoUninitialize();
}