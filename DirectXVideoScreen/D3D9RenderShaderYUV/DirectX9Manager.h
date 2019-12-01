//----------------------------------------------------------------------------------------------
// DirectX9Manager.h
//----------------------------------------------------------------------------------------------
#ifndef DIRECTX9MANAGER_H
#define DIRECTX9MANAGER_H

class CDirectX9Manager{

public:

	// DirectX9Manager.cpp
	CDirectX9Manager();
	~CDirectX9Manager(){ ReleaseDirectX(); }

	// DirectX9Manager.cpp
	HRESULT InitDirectX(const HWND, const HINSTANCE);
	HRESULT RenderDirectX();
	void ReleaseDirectX();

	const UINT GetImageWidth() const{ return m_uiWidth; }
	const UINT GetImageHeight() const{ return m_uiHeight; }

	void SetTechnique(const int iTechnique){ if(m_pVideoEffect){ m_iTechnique = iTechnique; m_pVideoEffect->SetTechnique(GetCurrentTechnique(iTechnique)); } }

private:

	IDirect3DDevice9 * m_pD3DDevice9;
	ID3DXEffect* m_pVideoEffect;
	IDirect3DTexture9* m_pImageTexture;

	IDirect3DTexture9* m_pLumaTexture;
	IDirect3DTexture9* m_pChromaCBTexture;
	IDirect3DTexture9* m_pChromaCRTexture;

	IDirect3DTexture9* m_pYTexture;
	IDirect3DTexture9* m_pUTexture;
	IDirect3DTexture9* m_pVTexture;

	IDirect3DTexture9* m_pYUVTexture;
	IDirect3DTexture9* m_pNV12Texture;

	IDirect3DTexture9* m_pShiftWidthTexture;
	IDirect3DTexture9* m_pShiftHeightTexture;

	D3DXVECTOR2 m_vTexelSize;
	D3DXVECTOR2 m_vTexelNV12;

	CQuad m_cQuad;
	CYUVQuad m_cYUVQuad;

	UINT m_uiWidth;
	UINT m_uiHeight;
	int m_iTechnique;

	BOOL GetImageSize(const LPCWSTR, UINT&, UINT&);
	HRESULT InitTexture();
	HRESULT InitShader(const HINSTANCE);
	HRESULT RGBToYUV444();
	HRESULT DownSampleToYV12();
	HRESULT DownSampleUV(IDirect3DTexture9*, BYTE*, const D3DSURFACE_DESC&);
	HRESULT CopyTextureYUV(const D3DSURFACE_DESC&, BYTE*, BYTE*);
	HRESULT YV12ToNV12();
	HRESULT FillShiftTexture();
	D3DXHANDLE GetCurrentTechnique(const int);

#ifdef _DEBUG
	void ShowRelease();
#endif

	BYTE ClampYUV(const float);
};

inline BYTE CDirectX9Manager::ClampYUV(const float yuv)
{
	BYTE b;

	if(yuv < 0.0f)
		b = 0;
	else if(yuv > 255.0f)
		b = 255;
	else
		b = (BYTE)((UINT)yuv);

	return b;
}

#endif