//----------------------------------------------------------------------------------------------
// FormatConversionTools.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

// https://www.vocal.com/video/rgb-and-yuv-color-space-conversion/

BYTE GetR(const int bY, int const bU){

	int iR = (int)(1.164f * bY) + (int)(1.403f * bU);
	iR = iR > 255 ? 255 : iR < 0 ? 0 : iR;

	return (BYTE)iR;
}

BYTE GetG(const int bY, const int bU, const int bV){

	int iG = (int)(1.164f * bY) - (int)(0.392f * bV + 0.813f * bU);
	iG = iG > 255 ? 255 : iG < 0 ? 0 : iG;

	return (BYTE)iG;
}

BYTE GetB(const int bY, const int bV){

	int iB = (int)(1.164f * bY) + (int)(2.017f * bV);
	iB = iB > 255 ? 255 : iB < 0 ? 0 : iB;

	return (BYTE)iB;
}

HRESULT ProcessNV12ToBmpFile(LPCWSTR wszBmpFile, BYTE* pDataIn, const INT iStride, const UINT uiWidth, const UINT uiHeight)
{
	HRESULT hr;

	UINT uiSampleSize = uiWidth * uiHeight * 3;
	BYTE* pDataRgb = new (std::nothrow)BYTE[uiSampleSize];
	IF_FAILED_RETURN(hr = (pDataRgb == NULL ? E_FAIL : S_OK));

	UINT uiStrideRGB = uiWidth * 3;
	UINT uiDelta = iStride - uiWidth;

	BYTE* pDataY = NULL;
	BYTE* pDataY2 = NULL;
	BYTE* pDataUV = NULL;

	BYTE* pRgb1 = NULL;
	BYTE* pRgb2 = NULL;

	BYTE bY1, bY2, bY3, bY4;
	BYTE bU, bV;

	int iY1, iY2, iY3, iY4;
	int iU, iV;

	pRgb1 = pDataRgb;
	pRgb2 = pDataRgb + uiStrideRGB;

	pDataY = pDataIn;
	pDataY2 = pDataIn + iStride;
	pDataUV = pDataIn + (iStride * uiHeight);

	try{

		for(UINT i = 0; i < uiHeight; i += 2){

			for(UINT j = 0; j < uiWidth; j += 2){

				bY1 = *pDataY++;
				bY2 = *pDataY++;
				bY3 = *pDataY2++;
				bY4 = *pDataY2++;
				bU = *pDataUV++;
				bV = *pDataUV++;

				iY1 = bY1 - 16;
				iY2 = bY2 - 16;
				iY3 = bY3 - 16;
				iY4 = bY4 - 16;

				iU = bU - 128;
				iV = bV - 128;

				*pRgb1++ = GetR(iY1, iU);
				*pRgb1++ = GetG(iY1, iU, iV);
				*pRgb1++ = GetB(iY1, iV);

				*pRgb1++ = GetR(iY2, iU);
				*pRgb1++ = GetG(iY2, iU, iV);
				*pRgb1++ = GetB(iY2, iV);

				*pRgb2++ = GetR(iY3, iU);
				*pRgb2++ = GetG(iY3, iU, iV);
				*pRgb2++ = GetB(iY3, iV);

				*pRgb2++ = GetR(iY4, iU);
				*pRgb2++ = GetG(iY4, iU, iV);
				*pRgb2++ = GetB(iY4, iV);
			}

			pDataUV += uiDelta;
			pDataY += (iStride + uiDelta);
			pDataY2 += (iStride + uiDelta);
			pRgb1 += uiStrideRGB;
			pRgb2 += uiStrideRGB;
		}

		IF_FAILED_THROW(hr = CreateBmpFile(wszBmpFile, pDataRgb, uiSampleSize, uiWidth, uiHeight));
	}
	catch(HRESULT){}

	SAFE_DELETE_ARRAY(pDataRgb);

	return hr;
}

HRESULT CreateBmpFile(LPCWSTR wszBmpFile, BYTE* pData, const UINT uiFrameSize, const UINT uiWidth, const UINT uiHeight)
{
	HRESULT hr = S_OK;

	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwWritten;
	UINT uiStride;

	BYTE header24[54] = {0x42, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00,
		0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	DWORD dwSizeFile = uiWidth * uiHeight * 3;
	dwSizeFile += 54;
	header24[2] = dwSizeFile & 0x000000ff;
	header24[3] = static_cast<BYTE>((dwSizeFile & 0x0000ff00) >> 8);
	header24[4] = static_cast<BYTE>((dwSizeFile & 0x00ff0000) >> 16);
	header24[5] = (dwSizeFile & 0xff000000) >> 24;
	dwSizeFile -= 54;
	header24[18] = uiWidth & 0x000000ff;
	header24[19] = (uiWidth & 0x0000ff00) >> 8;
	header24[20] = static_cast<BYTE>((uiWidth & 0x00ff0000) >> 16);
	header24[21] = (uiWidth & 0xff000000) >> 24;

	header24[22] = uiHeight & 0x000000ff;
	header24[23] = (uiHeight & 0x0000ff00) >> 8;
	header24[24] = static_cast<BYTE>((uiHeight & 0x00ff0000) >> 16);
	header24[25] = (uiHeight & 0xff000000) >> 24;

	header24[34] = dwSizeFile & 0x000000ff;
	header24[35] = (dwSizeFile & 0x0000ff00) >> 8;
	header24[36] = static_cast<BYTE>((dwSizeFile & 0x00ff0000) >> 16);
	header24[37] = static_cast<BYTE>((dwSizeFile & 0xff000000) >> 24);

	try
	{
		hFile = CreateFile(wszBmpFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		IF_FAILED_THROW(hr = (hFile == INVALID_HANDLE_VALUE ? E_FAIL : S_OK));

		IF_FAILED_THROW(hr = (WriteFile(hFile, (LPCVOID)header24, 54, &dwWritten, 0) == FALSE));
		IF_FAILED_THROW(hr = (dwWritten == 0 ? E_FAIL : S_OK));

		uiStride = uiWidth * 3;
		BYTE* Tmpbufsrc = pData + (uiFrameSize - uiStride);

		for(UINT i = 0; i < uiHeight; i++){

			IF_FAILED_THROW(hr = (WriteFile(hFile, (LPCVOID)Tmpbufsrc, uiStride, &dwWritten, 0) == FALSE));
			IF_FAILED_THROW(hr = (dwWritten == 0 ? E_FAIL : S_OK));

			Tmpbufsrc -= uiStride;
		}
	}
	catch(HRESULT){}

	if(hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return hr;
}