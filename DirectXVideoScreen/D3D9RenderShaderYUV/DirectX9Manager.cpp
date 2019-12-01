//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
// DirectX9Manager.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

CDirectX9Manager::CDirectX9Manager()
	: m_pD3DDevice9(NULL),
	m_pVideoEffect(NULL),
	m_pImageTexture(NULL),
	m_pLumaTexture(NULL),
	m_pChromaCBTexture(NULL),
	m_pChromaCRTexture(NULL),
	m_pYTexture(NULL),
	m_pUTexture(NULL),
	m_pVTexture(NULL),
	m_pYUVTexture(NULL),
	m_pNV12Texture(NULL),
	m_pShiftWidthTexture(NULL),
	m_pShiftHeightTexture(NULL),
	m_uiWidth(0),
	m_uiHeight(0),
	m_iTechnique(SHOWRGB)
{
}

HRESULT CDirectX9Manager::InitDirectX(const HWND hWnd, const HINSTANCE hModule)
{
	HRESULT hr = S_OK;

	if(GetImageSize(IMAGE_FILE, m_uiWidth, m_uiHeight) == FALSE)
	{
		return hr = E_FAIL;
	}

	D3DPRESENT_PARAMETERS pp;
	IDirect3D9* pD3D9 = NULL;
	D3DDISPLAYMODE dm;
	UINT uiMonitor = D3DADAPTER_DEFAULT;

	try
	{
		pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);

		IF_FAILED_THROW((pD3D9 == NULL) ? E_FAIL : S_OK);

		IF_FAILED_THROW(pD3D9->GetAdapterDisplayMode(uiMonitor, &dm));

		ZeroMemory(&pp, sizeof(pp));
		pp.Windowed = TRUE;
		pp.hDeviceWindow = hWnd;
		pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pp.BackBufferFormat = dm.Format;
		pp.BackBufferWidth = m_uiWidth;
		pp.BackBufferHeight = m_uiHeight;

		IF_FAILED_THROW(pD3D9->CreateDevice(uiMonitor, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &m_pD3DDevice9));
		IF_FAILED_THROW((m_pD3DDevice9 == NULL) ? E_FAIL : S_OK);

		m_cQuad.OnCreate();
		IF_FAILED_THROW(m_cQuad.OnRestore(m_pD3DDevice9));

		m_cYUVQuad.OnCreate();
		IF_FAILED_THROW(m_cYUVQuad.OnRestore(m_pD3DDevice9));

		IF_FAILED_THROW(InitTexture());
		IF_FAILED_THROW(InitShader(hModule));
	}
	catch(HRESULT hr1)
	{
		hr = hr1;

#ifdef _DEBUG
		LOG_HRESULT(hr);
		DXTRACE_ERR_MSGBOX(L"InitDirectX", hr);
#endif
	}

	SAFE_RELEASE(pD3D9);

	return hr;
}

HRESULT CDirectX9Manager::RenderDirectX()
{
	HRESULT hr = E_FAIL;

	if(m_pD3DDevice9 == NULL)
		return hr;

	// hr = m_pDevice->TestCooperativeLevel(); // D3DERR_DEVICENOTRESET D3D_OK

	IF_FAILED_RETURN(m_pD3DDevice9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0));

	IF_FAILED_RETURN(m_pD3DDevice9->BeginScene());

	UINT uiNumPasses;

	IF_FAILED_RETURN(m_pVideoEffect->Begin(&uiNumPasses, 0));

	IF_FAILED_RETURN(m_pVideoEffect->BeginPass(0));

	if(m_iTechnique == SHOWDECODEDYUV || m_iTechnique == SHOWDECODEDNV12 || m_iTechnique == SHOWHEIGHTSHIFT)
	{
		IF_FAILED_RETURN(m_cYUVQuad.OnRender(m_pD3DDevice9));
	}
	else
	{
		IF_FAILED_RETURN(m_cQuad.OnRender(m_pD3DDevice9));
	}

	IF_FAILED_RETURN(m_pVideoEffect->EndPass());

	IF_FAILED_RETURN(m_pVideoEffect->End());

	IF_FAILED_RETURN(m_pD3DDevice9->EndScene());

	IF_FAILED_RETURN(m_pD3DDevice9->Present(NULL, NULL, NULL, NULL));

	// D3DERR_DEVICELOST D3DERR_DRIVERINTERNALERROR
	return hr;
}

void CDirectX9Manager::ReleaseDirectX()
{
	SAFE_RELEASE(m_pVideoEffect);
	SAFE_RELEASE(m_pImageTexture);
	SAFE_RELEASE(m_pLumaTexture);
	SAFE_RELEASE(m_pChromaCBTexture);
	SAFE_RELEASE(m_pChromaCRTexture);
	SAFE_RELEASE(m_pYTexture);
	SAFE_RELEASE(m_pUTexture);
	SAFE_RELEASE(m_pVTexture);
	SAFE_RELEASE(m_pYUVTexture);
	SAFE_RELEASE(m_pNV12Texture);
	SAFE_RELEASE(m_pShiftWidthTexture);
	SAFE_RELEASE(m_pShiftHeightTexture);

	m_cQuad.OnDelete();
	m_cYUVQuad.OnDelete();

#ifdef _DEBUG
	ShowRelease();
#else
	SAFE_RELEASE(m_pD3DDevice9);
#endif
}

BOOL CDirectX9Manager::GetImageSize(const LPCWSTR pwszFile, UINT& uiWidth, UINT& uiHeight)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::Status status = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	if(status != Gdiplus::Ok)
	{
		return FALSE;
	}

	Gdiplus::Image* image = new Gdiplus::Image(pwszFile);

	if(image == NULL)
	{
		GdiplusShutdown(gdiplusToken);
		return FALSE;
	}

	uiWidth = image->GetWidth();
	uiHeight = image->GetHeight();

	delete image;
	GdiplusShutdown(gdiplusToken);

	if(uiWidth % 2 || uiHeight % 2)
	{
		return FALSE;
	}

	return TRUE;
}

HRESULT CDirectX9Manager::InitTexture()
{
	HRESULT hr;

	IF_FAILED_RETURN(D3DXCreateTextureFromFileEx(m_pD3DDevice9, IMAGE_FILE, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0,
		D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_pImageTexture));

	D3DSURFACE_DESC desc;
	IF_FAILED_RETURN(m_pImageTexture->GetLevelDesc(0, &desc));

	UINT uiYUVHeight = (UINT)(desc.Height * 3.0f / 2.0f);

	IF_FAILED_RETURN(m_pD3DDevice9->CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pLumaTexture, NULL));
	IF_FAILED_RETURN(m_pD3DDevice9->CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pChromaCBTexture, NULL));
	IF_FAILED_RETURN(m_pD3DDevice9->CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pChromaCRTexture, NULL));

	IF_FAILED_RETURN(m_pD3DDevice9->CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pYTexture, NULL));
	IF_FAILED_RETURN(m_pD3DDevice9->CreateTexture(UINT(desc.Width / 2.0f), UINT(desc.Height / 2.0f), 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pUTexture, NULL));
	IF_FAILED_RETURN(m_pD3DDevice9->CreateTexture(UINT(desc.Width / 2.0f), UINT(desc.Height / 2.0f), 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pVTexture, NULL));

	IF_FAILED_RETURN(m_pD3DDevice9->CreateTexture(desc.Width, uiYUVHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pYUVTexture, NULL));
	IF_FAILED_RETURN(m_pD3DDevice9->CreateTexture(desc.Width, uiYUVHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pNV12Texture, NULL));

	IF_FAILED_RETURN(m_pD3DDevice9->CreateTexture(desc.Width, 1, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pShiftWidthTexture, NULL));
	IF_FAILED_RETURN(m_pD3DDevice9->CreateTexture(1, uiYUVHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pShiftHeightTexture, NULL));

	IF_FAILED_RETURN(RGBToYUV444());
	IF_FAILED_RETURN(DownSampleToYV12());
	IF_FAILED_RETURN(YV12ToNV12());
	IF_FAILED_RETURN(FillShiftTexture());

	return hr;
}

HRESULT CDirectX9Manager::InitShader(const HINSTANCE hModule)
{
	HRESULT hr;

	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_NO_PRESHADER | D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESAMPLERSTATE | D3DXFX_DONOTSAVESHADERSTATE;

	IF_FAILED_RETURN(D3DXCreateEffectFromResource(m_pD3DDevice9, hModule, MAKEINTRESOURCE(IDR_EFFECT), NULL, NULL, dwShaderFlags, NULL, &m_pVideoEffect, NULL));
	IF_FAILED_RETURN(m_pVideoEffect->OnResetDevice());

	m_vTexelSize.x = (1.0f / float(m_uiWidth)) / 2.0f;
	m_vTexelSize.y = (1.0f / float(m_uiHeight)) / 2.0f;

	m_vTexelNV12.x = (1.0f / float(m_uiWidth)) / 2.0f;

	UINT uiHeight = (UINT)(m_uiHeight * 3.0f / 2.0f);
	m_vTexelNV12.y = (1.0f / float(uiHeight)) / 2.0f;

	IF_FAILED_RETURN(m_pVideoEffect->SetTexture("m_pImageTexture", m_pImageTexture));
	IF_FAILED_RETURN(m_pVideoEffect->SetTexture("m_pLumaTexture", m_pLumaTexture));
	IF_FAILED_RETURN(m_pVideoEffect->SetTexture("m_pChromaCBTexture", m_pChromaCBTexture));
	IF_FAILED_RETURN(m_pVideoEffect->SetTexture("m_pChromaCRTexture", m_pChromaCRTexture));
	IF_FAILED_RETURN(m_pVideoEffect->SetTexture("m_pYTexture", m_pYTexture));
	IF_FAILED_RETURN(m_pVideoEffect->SetTexture("m_pUTexture", m_pUTexture));
	IF_FAILED_RETURN(m_pVideoEffect->SetTexture("m_pVTexture", m_pVTexture));
	IF_FAILED_RETURN(m_pVideoEffect->SetTexture("m_pYUVTexture", m_pYUVTexture));
	IF_FAILED_RETURN(m_pVideoEffect->SetTexture("m_pNV12Texture", m_pNV12Texture));
	IF_FAILED_RETURN(m_pVideoEffect->SetTexture("m_pShiftWidthTexture", m_pShiftWidthTexture));
	IF_FAILED_RETURN(m_pVideoEffect->SetTexture("m_pShiftHeightTexture", m_pShiftHeightTexture));

	IF_FAILED_RETURN(m_pVideoEffect->SetValue("m_vTexelSize", &m_vTexelSize, sizeof(D3DXVECTOR2)));
	IF_FAILED_RETURN(m_pVideoEffect->SetValue("m_vTexelNV12", &m_vTexelNV12, sizeof(D3DXVECTOR2)));

	IF_FAILED_RETURN(m_pVideoEffect->SetTechnique(GetCurrentTechnique(m_iTechnique)));

	return hr;
}

HRESULT CDirectX9Manager::RGBToYUV444()
{
	HRESULT hr = S_OK;

	D3DLOCKED_RECT lrIn;
	D3DLOCKED_RECT lrOutL;
	D3DLOCKED_RECT lrOutCb;
	D3DLOCKED_RECT lrOutCr;
	BYTE* pDataIn;
	BYTE* pY;
	BYTE* pU;
	BYTE* pV;

	float c, cb, cr;
	BYTE y, u, v;
	BYTE r, g, b;

	D3DSURFACE_DESC desc;

	IF_FAILED_RETURN(m_pImageTexture->GetLevelDesc(0, &desc));

	IF_FAILED_RETURN(m_pImageTexture->LockRect(0, &lrIn, NULL, 0));
	IF_FAILED_RETURN(m_pLumaTexture->LockRect(0, &lrOutL, NULL, 0));
	IF_FAILED_RETURN(m_pChromaCBTexture->LockRect(0, &lrOutCb, NULL, 0));
	IF_FAILED_RETURN(m_pChromaCRTexture->LockRect(0, &lrOutCr, NULL, 0));

	pDataIn = (BYTE*)lrIn.pBits;
	pY = (BYTE*)lrOutL.pBits;
	pU = (BYTE*)lrOutCb.pBits;
	pV = (BYTE*)lrOutCr.pBits;

	for(UINT ui = 0; ui < desc.Height; ui++)
	{
		for(UINT ui2 = 0; ui2 < desc.Width; ui2++)
		{
			b = *pDataIn++;
			g = *pDataIn++;
			r = *pDataIn++;
			pDataIn++;// skeep alpha

			c = (float)(((66 * r + 129 * g + 25 * b + 128) >> 8) + 16);
			cb = (float)(((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128);
			cr = (float)(((112 * r - 94 * g - 18 * b + 128) >> 8) + 128);

			y = ClampYUV(c);
			u = ClampYUV(cb);
			v = ClampYUV(cr);

			*pY++ = y;
			*pU++ = u;
			*pV++ = v;
		}

		pDataIn += (lrIn.Pitch - (desc.Width * 4));
		pY += (lrOutL.Pitch - desc.Width);
		pU += (lrOutCb.Pitch - desc.Width);
		pV += (lrOutCr.Pitch - desc.Width);
	}

	IF_FAILED_RETURN(m_pImageTexture->UnlockRect(0));
	IF_FAILED_RETURN(m_pLumaTexture->UnlockRect(0));
	IF_FAILED_RETURN(m_pChromaCBTexture->UnlockRect(0));
	IF_FAILED_RETURN(m_pChromaCRTexture->UnlockRect(0));

	return hr;
}

HRESULT CDirectX9Manager::DownSampleToYV12()
{
	HRESULT hr = S_OK;

	BYTE* pU = NULL;
	BYTE* pV = NULL;
	D3DSURFACE_DESC desc;

	IF_FAILED_RETURN(m_pImageTexture->GetLevelDesc(0, &desc));

	UINT uiSize = (UINT)((desc.Width / 2.0f) * (desc.Height / 2.0f));

	try
	{
		pU = new (std::nothrow)BYTE[uiSize];
		IF_FAILED_THROW(pU == NULL ? E_FAIL : S_OK);

		pV = new (std::nothrow)BYTE[uiSize];
		IF_FAILED_THROW(pV == NULL ? E_FAIL : S_OK);

		IF_FAILED_THROW(DownSampleUV(m_pChromaCBTexture, pU, desc));
		IF_FAILED_THROW(DownSampleUV(m_pChromaCRTexture, pV, desc));
		IF_FAILED_THROW(CopyTextureYUV(desc, pU, pV));
	}
	catch(HRESULT){}
	catch(...){ hr = E_FAIL; }

	if(pU)
		delete[] pU;

	if(pV)
		delete[] pV;

	return hr;
}

HRESULT CDirectX9Manager::DownSampleUV(IDirect3DTexture9* pTexture, BYTE* pData, const D3DSURFACE_DESC& desc)
{
	HRESULT hr = S_OK;

	D3DLOCKED_RECT lrIn;
	IF_FAILED_RETURN(pTexture->LockRect(0, &lrIn, NULL, 0));

	BYTE bValue[4];
	FLOAT fRes;

	BYTE* pRes = pData;
	BYTE* pDataIn = (BYTE*)lrIn.pBits;
	BYTE* pDataIn2 = pDataIn + lrIn.Pitch;

	for(UINT ui = 0; ui < desc.Height; ui += 2)
	{
		for(UINT ui2 = 0; ui2 < desc.Width; ui2 += 2)
		{
			bValue[0] = *pDataIn++;
			bValue[1] = *pDataIn++;
			bValue[2] = *pDataIn2++;
			bValue[3] = *pDataIn2++;

			fRes = (bValue[0] + bValue[1] + bValue[2] + bValue[3]) / 4.0f;

			*pRes++ = ClampYUV(fRes);
		}

		pDataIn += (lrIn.Pitch - desc.Width);
		pDataIn2 += (lrIn.Pitch - desc.Width);
		pDataIn += lrIn.Pitch;
		pDataIn2 += lrIn.Pitch;
	}

	IF_FAILED_RETURN(pTexture->UnlockRect(0));

	return hr;
}

HRESULT CDirectX9Manager::CopyTextureYUV(const D3DSURFACE_DESC& desc, BYTE* pU, BYTE* pV)
{
	HRESULT hr = S_OK;

	D3DLOCKED_RECT lrIn;
	D3DLOCKED_RECT lrOut;
	D3DLOCKED_RECT lrOut2;
	BYTE* pDataIn;
	BYTE* pDataOut;
	BYTE* pDataOut2;
	BYTE* pRes;

	//---------------------------------------------------------------------
	// Copy Y
	IF_FAILED_RETURN(m_pLumaTexture->LockRect(0, &lrIn, NULL, 0));
	IF_FAILED_RETURN(m_pYUVTexture->LockRect(0, &lrOut, NULL, 0));
	IF_FAILED_RETURN(m_pYTexture->LockRect(0, &lrOut2, NULL, 0));

	pDataIn = (BYTE*)lrIn.pBits;
	pDataOut = (BYTE*)lrOut.pBits;
	pDataOut2 = (BYTE*)lrOut2.pBits;

	for(UINT ui = 0; ui < desc.Height; ui++)
	{
		memcpy(pDataOut, pDataIn, desc.Width);
		memcpy(pDataOut2, pDataIn, desc.Width);

		pDataIn += lrIn.Pitch;
		pDataOut += lrOut.Pitch;
		pDataOut2 += lrOut2.Pitch;
	}

	IF_FAILED_RETURN(m_pLumaTexture->UnlockRect(0));
	IF_FAILED_RETURN(m_pYTexture->UnlockRect(0));

	//---------------------------------------------------------------------
	// Copy U
	IF_FAILED_RETURN(m_pUTexture->LockRect(0, &lrOut2, NULL, 0));
	pDataOut2 = (BYTE*)lrOut2.pBits;

	pRes = pU;
	UINT uiHalfWidth = (UINT)(desc.Width / 2.0f);
	UINT uiHalfHeight = (UINT)(desc.Height / 2.0f);

	for(UINT ui = 0; ui < uiHalfHeight; ui++)
	{
		memcpy(pDataOut, pRes, uiHalfWidth);
		memcpy(pDataOut2, pRes, uiHalfWidth);

		pRes += uiHalfWidth;
		pDataOut += uiHalfWidth;

		if(ui % 2)
		{
			pDataOut += (lrOut.Pitch - desc.Width);
		}

		pDataOut2 += lrOut2.Pitch;
	}

	IF_FAILED_RETURN(m_pUTexture->UnlockRect(0));

	//---------------------------------------------------------------------
	// Copy V
	IF_FAILED_RETURN(m_pVTexture->LockRect(0, &lrOut2, NULL, 0));
	pDataOut2 = (BYTE*)lrOut2.pBits;

	pRes = pV;

	for(UINT ui = 0; ui < uiHalfHeight; ui++)
	{
		memcpy(pDataOut, pRes, uiHalfWidth);
		memcpy(pDataOut2, pRes, uiHalfWidth);

		pRes += uiHalfWidth;
		pDataOut += uiHalfWidth;

		if(ui % 2)
		{
			pDataOut += (lrOut.Pitch - desc.Width);
		}

		pDataOut2 += lrOut2.Pitch;
	}

	IF_FAILED_RETURN(m_pYUVTexture->UnlockRect(0));
	IF_FAILED_RETURN(m_pVTexture->UnlockRect(0));

	return hr;
}

HRESULT CDirectX9Manager::YV12ToNV12()
{
	HRESULT hr = S_OK;

	D3DSURFACE_DESC desc;
	D3DLOCKED_RECT lrIn;
	D3DLOCKED_RECT lrOut;
	BYTE* pDataIn;
	BYTE* pDataOut;
	BYTE* pUV;

	IF_FAILED_RETURN(m_pImageTexture->GetLevelDesc(0, &desc));

	IF_FAILED_RETURN(m_pYUVTexture->LockRect(0, &lrIn, NULL, 0));
	IF_FAILED_RETURN(m_pNV12Texture->LockRect(0, &lrOut, NULL, 0));

	pDataIn = (BYTE*)lrIn.pBits;
	pDataOut = (BYTE*)lrOut.pBits;

	for(UINT ui = 0; ui < desc.Height; ui++)
	{
		memcpy(pDataOut, pDataIn, desc.Width);
		pDataIn += lrIn.Pitch;
		pDataOut += lrOut.Pitch;
	}

	UINT uiHalfWidth = UINT(desc.Width / 2.0f);
	UINT uiHalfHeight = UINT(desc.Height / 2.0f);

	pUV = pDataIn + (lrIn.Pitch * UINT(uiHalfHeight / 2.0f));

	for(UINT ui = 0; ui < uiHalfHeight; ui++)
	{
		for(UINT ui2 = 0; ui2 < uiHalfWidth; ui2++)
		{
			*pDataOut++ = *pDataIn++;
			*pDataOut++ = *pUV++;
		}

		if(ui % 2)
		{
			pDataIn += (lrIn.Pitch - desc.Width);
			pUV += (lrIn.Pitch - desc.Width);
		}

		pDataOut += (lrOut.Pitch - desc.Width);
	}

	IF_FAILED_RETURN(m_pYUVTexture->UnlockRect(0));
	IF_FAILED_RETURN(m_pNV12Texture->UnlockRect(0));

	return hr;
}

HRESULT CDirectX9Manager::FillShiftTexture()
{
	HRESULT hr = S_OK;

	D3DLOCKED_RECT lr;
	BYTE* pData;
	D3DSURFACE_DESC desc;

	// Width
	IF_FAILED_RETURN(m_pShiftWidthTexture->GetLevelDesc(0, &desc));

	IF_FAILED_RETURN(m_pShiftWidthTexture->LockRect(0, &lr, NULL, 0));

	pData = (BYTE*)lr.pBits;

	for(UINT ui = 0; ui < desc.Width; ui++)
	{
		if(ui % 2)
			*pData++ = 0x00;
		else
			*pData++ = 0x01;
	}

	IF_FAILED_RETURN(m_pShiftWidthTexture->UnlockRect(0));

	// Height
	IF_FAILED_RETURN(m_pShiftHeightTexture->GetLevelDesc(0, &desc));

	IF_FAILED_RETURN(m_pShiftHeightTexture->LockRect(0, &lr, NULL, 0));

	pData = (BYTE*)lr.pBits;

	for(UINT ui = 0; ui < desc.Height; ui++)
	{
		if(ui % 2)
			*pData = 0x00;
		else
			*pData = 0x01;

		pData += lr.Pitch;
	}

	IF_FAILED_RETURN(m_pShiftHeightTexture->UnlockRect(0));

	return hr;
}

D3DXHANDLE CDirectX9Manager::GetCurrentTechnique(const int iTech)
{
	D3DXHANDLE hTechnique = NULL;

	switch(iTech){

		case SHOWRGB: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowRGB"); break;
		case SHOWLUMA: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowLuma"); break;
		case SHOWCHROMAB: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowChromaB"); break;
		case SHOWCHROMAR: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowChromaR"); break;
		case SHOWYUV: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowYUV"); break;
		case SHOWNV12: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowNV12"); break;
		case SHOWWIDTHSHIFT: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowWidthShift"); break;
		case SHOWHEIGHTSHIFT: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowHeightShift"); break;
		case SHOWY: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowY"); break;
		case SHOWU: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowU"); break;
		case SHOWV: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowV"); break;
		case SHOWDECODEDYUV444: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowDecodedYUV444YUV"); break;
		case SHOWDECODEDYUV420: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowDecodedYUV420YUV"); break;
		case SHOWDECODEDYUV: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowDecodedYUV"); break;
		case SHOWDECODEDNV12: hTechnique = m_pVideoEffect->GetTechniqueByName("ShowDecodedNV12"); break;
	}

	return hTechnique;
}

#ifdef _DEBUG

void CDirectX9Manager::ShowRelease()
{
	if(m_pD3DDevice9 == NULL)
		return;

	UINT uiRelease = m_pD3DDevice9->Release();
	m_pD3DDevice9 = NULL;

	if(uiRelease != 0)
	{
		WCHAR wszBuffer[100];

		HRESULT hr = StringCbPrintf(wszBuffer, 100, L"Error Release Device : %i object(s)", uiRelease);

		if(SUCCEEDED(hr) || HRESULT_CODE(hr) == ERROR_INSUFFICIENT_BUFFER)
			MessageBox(NULL, wszBuffer, L"Release Device", MB_OK);
		else
			MessageBox(NULL, L"Error Release Device", L"Release Device", MB_OK);
	}
}

#endif