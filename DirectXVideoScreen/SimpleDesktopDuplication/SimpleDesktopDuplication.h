//----------------------------------------------------------------------------------------------
// SimpleDesktopDuplication.h
//----------------------------------------------------------------------------------------------
#ifndef SIMPLEDESKTOPDUPLICATION_H
#define SIMPLEDESKTOPDUPLICATION_H

class CSimpleDesktopDuplication{

public:

	CSimpleDesktopDuplication(){}
	~CSimpleDesktopDuplication(){ OnRelease(); }

	HRESULT InitDesktopDuplication();
	HRESULT ProcessDesktopDuplication(LPCWSTR);
	void OnRelease();

private:

	ID3D11Device* m_pD3D11Device = NULL;
	ID3D11DeviceContext* m_pD3D11DeviceContext = NULL;
	IDXGIOutputDuplication* m_pDXGIOutputDuplication = NULL;

	HRESULT CreateBmpFileFromScreenShot(ID3D11Texture2D*, LPCWSTR);
};

#endif