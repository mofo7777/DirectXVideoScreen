//----------------------------------------------------------------------------------------------
// Main.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

#define WINDOWS_CLASS_NAME L"D3D9VideoProcessorClass"

#define INPUT_IMAGE L"background.bmp"
#define CONVERTED_IMAGE L"ConvertedImage.bmp"

HRESULT GetImageSize(LPCWSTR, UINT*, UINT*);

void main()
{
	UINT uiWidth;
	UINT uiHeight;

	if(FAILED(GetImageSize(INPUT_IMAGE, &uiWidth, &uiHeight)))
		return;

	HWND hWnd = InitWindow(WINDOWS_CLASS_NAME, uiWidth, uiHeight);

	if(hWnd == NULL)
		return;

	CD3D9VideoProcessor cD3D9VideoProcessor;

	// D3DFMT_YUY2
	// (D3DFORMAT)D3DFMT_NV12
	if(cD3D9VideoProcessor.InitDXVA2(hWnd, uiWidth, uiHeight, (D3DFORMAT)D3DFMT_NV12) == S_OK)
	{
		cD3D9VideoProcessor.ProcessImageConversion(INPUT_IMAGE, CONVERTED_IMAGE);
	}

	cD3D9VideoProcessor.OnRelease();
	EndWindow(WINDOWS_CLASS_NAME, hWnd);
}

HRESULT GetImageSize(LPCWSTR wszImageFile, UINT* puiWidth, UINT* puiHeight)
{
	HRESULT hr = S_OK;
	D3DXIMAGE_INFO d3dImageInfo = {0};

	IF_FAILED_RETURN(D3DXGetImageInfoFromFile(wszImageFile, &d3dImageInfo));

	*puiWidth = d3dImageInfo.Width;
	*puiHeight = d3dImageInfo.Height;

	return hr;
}