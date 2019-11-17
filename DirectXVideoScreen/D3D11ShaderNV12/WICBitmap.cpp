//----------------------------------------------------------------------------------------------
// WICBitmap.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

HRESULT CWICBitmap::InitImage(LPCWSTR wszImageFile)
{
	HRESULT hr = S_OK;
	IWICImagingFactory* pIWICFactory = NULL;
	IWICBitmapDecoder* pIWICDecoder = NULL;
	IWICBitmapFrameDecode* pIWICDecoderFrame = NULL;
	UINT uiFrameCount = 0;

	IF_FAILED_RETURN(m_pIWICBitmap != NULL ? E_UNEXPECTED: S_OK);

	try
	{
		IF_FAILED_THROW(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory)));
		IF_FAILED_THROW(pIWICFactory->CreateDecoderFromFilename(wszImageFile, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pIWICDecoder));
		IF_FAILED_THROW(pIWICDecoder->GetFrameCount(&uiFrameCount));
		IF_FAILED_THROW(uiFrameCount < 1 ? E_UNEXPECTED : S_OK);
		IF_FAILED_THROW(pIWICDecoder->GetFrame(0, &pIWICDecoderFrame));
		IF_FAILED_THROW(pIWICDecoderFrame->GetSize(&m_uiWidth, &m_uiHeight));
		IF_FAILED_THROW(pIWICFactory->CreateBitmapFromSource(pIWICDecoderFrame, WICBitmapCacheOnDemand, &m_pIWICBitmap));
	}
	catch(HRESULT){}

	SAFE_RELEASE(pIWICDecoderFrame);
	SAFE_RELEASE(pIWICDecoder);
	SAFE_RELEASE(pIWICFactory);

	return hr;
}

HRESULT CWICBitmap::Create2DTextureFromBitmap(ID3D11Device* pD3D11Device, ID3D11Texture2D** ppD3D11Texture2D, const D3D11_TEXTURE2D_DESC& desc2D)
{
	HRESULT hr = S_OK;
	IWICBitmapLock* pIWICLock = NULL;
	D3D11_SUBRESOURCE_DATA SubResource2D;
	WICRect rcLock = {0, 0, (INT)m_uiWidth, (INT)m_uiHeight};
	UINT uiImageSize = 0;
	BYTE* pData = NULL;
	BYTE* pDataRgba = NULL;
	BYTE* pDataColor;
	BYTE* pDataRgbaColor;

	// https://docs.microsoft.com/fr-fr/windows/win32/wic/-wic-bitmapsources-howto-modifypixels

	IF_FAILED_RETURN((m_pIWICBitmap == NULL || pD3D11Device == NULL || ppD3D11Texture2D == NULL) ? E_UNEXPECTED : S_OK);

	try
	{
		IF_FAILED_THROW(m_pIWICBitmap->Lock(&rcLock, WICBitmapLockWrite, &pIWICLock));
		IF_FAILED_THROW(pIWICLock->GetDataPointer(&uiImageSize, &pData));

		pDataRgba = new (std::nothrow)BYTE[m_uiWidth * m_uiHeight * 4];
		IF_FAILED_THROW(pDataRgba == NULL ? E_FAIL : S_OK);

		pDataColor = pData;
		pDataRgbaColor = pDataRgba;

		for(UINT i = 0; i < m_uiHeight; i++)
		{
			for(UINT j = 0; j < m_uiWidth; j++)
			{
				*pDataRgbaColor++ = *pDataColor++;
				*pDataRgbaColor++ = *pDataColor++;
				*pDataRgbaColor++ = *pDataColor++;
				*pDataRgbaColor++ = 0x00;
			}
		}

		ZeroMemory(&SubResource2D, sizeof(SubResource2D));
		SubResource2D.pSysMem = (void*)pDataRgba;
		SubResource2D.SysMemPitch = m_uiWidth * 4;

		IF_FAILED_THROW(pD3D11Device->CreateTexture2D(&desc2D, &SubResource2D, ppD3D11Texture2D));
	}
	catch(HRESULT){}

	SAFE_RELEASE(pIWICLock);
	SAFE_DELETE_ARRAY(pDataRgba);

	return hr;
}

void CWICBitmap::OnRelease()
{
	SAFE_RELEASE(m_pIWICBitmap);
	m_uiWidth = 0;
	m_uiHeight = 0;
}