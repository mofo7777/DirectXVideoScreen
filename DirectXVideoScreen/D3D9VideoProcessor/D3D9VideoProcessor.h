//----------------------------------------------------------------------------------------------
// D3D9VideoProcessor.h
//----------------------------------------------------------------------------------------------
#ifndef D3D9VIDEOPROCESSOR_H
#define D3D9VIDEOPROCESSOR_H

const DWORD D3DFMT_NV12 = MAKEFOURCC('N', 'V', '1', '2');

class CD3D9VideoProcessor{

public:

	CD3D9VideoProcessor(){}
	~CD3D9VideoProcessor(){ OnRelease(); }

	HRESULT InitDXVA2(const HWND, const UINT, const UINT);
	HRESULT ProcessImageConversion(LPCWSTR, LPCWSTR);
	void OnRelease();

private:

	IDirect3DDevice9Ex* m_pDevice9Ex = NULL;
	IDXVAHD_Device* m_pDXVAHDDevice = NULL;
	IDXVAHD_VideoProcessor* m_pDXVAVP = NULL;
	UINT m_uiWidth = 0;
	UINT m_uiHeight = 0;

	HRESULT InitVideoProcessor(const UINT, const UINT);
	HRESULT CheckDxva2Format(const D3DFORMAT, const D3DFORMAT, const UINT, const UINT);
	HRESULT ConfigureVideoProcessor(const D3DFORMAT, const UINT, const UINT);
	HRESULT CreateBmpFileFromNV12Surface(IDirect3DSurface9*, LPCWSTR);
};

#endif
