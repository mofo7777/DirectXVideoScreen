//----------------------------------------------------------------------------------------------
// WICBitmap.h
//----------------------------------------------------------------------------------------------
#ifndef WICBITMAP_H
#define WICBITMAP_H

class CWICBitmap{

public:

	CWICBitmap(){}
	~CWICBitmap(){ OnRelease(); }

	HRESULT InitImage(LPCWSTR);
	HRESULT Create2DTextureFromBitmap(ID3D11Device*, ID3D11Texture2D**, const D3D11_TEXTURE2D_DESC&);
	void OnRelease();
	const UINT GetWidth() const{ return m_uiWidth; }
	const UINT GetHeight() const{ return m_uiHeight; }

private:

	IWICBitmap* m_pIWICBitmap = NULL;
	UINT m_uiWidth = 0;
	UINT m_uiHeight = 0;
};

#endif