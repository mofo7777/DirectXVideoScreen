//----------------------------------------------------------------------------------------------
// D3D11ShaderNV12.h
//----------------------------------------------------------------------------------------------
#ifndef D3D11SHADERNV12_H
#define D3D11SHADERNV12_H

class CD3D11ShaderNV12{

public:

	CD3D11ShaderNV12(){}
	~CD3D11ShaderNV12(){ OnRelease(); }

	HRESULT InitShaderNV12(CWICBitmap&);
	HRESULT ProcessShaderNV12(LPCWSTR, const int);
	void OnRelease();

private:

	ID3D11Device* m_pD3D11Device = NULL;
	ID3D11DeviceContext* m_pD3D11DeviceContext = NULL;
	IDXGISwapChain* m_pDXGISwapChain = NULL;

	ID3D11RenderTargetView* m_pViewRT = NULL;
	ID3D11RenderTargetView* m_pLumaRT = NULL;

	ID3D11ShaderResourceView* m_pInputRSV = NULL;
	ID3D11ShaderResourceView* m_pLumaRSV = NULL;

	ID3D11VertexShader* m_pVertexShader = NULL;
	ID3D11PixelShader* m_pPixelShader = NULL;
	ID3D11PixelShader* m_pLumaShader = NULL;
	ID3D11InputLayout* m_pVertexLayout = NULL;
	ID3D11SamplerState* m_pSamplerState = NULL;

	HRESULT InitD3D11Resources(CWICBitmap&);
	HRESULT InitVertexPixelShader();
	HRESULT InitRenderTargetView(const UINT, const UINT);
	HRESULT InitInputTexture(CWICBitmap&);
	HRESULT InitRenderTargetLuma(const UINT, const UINT);
	HRESULT InitVertexShaderFromFile(const WCHAR*, ID3D11VertexShader**, const BOOL);
	HRESULT InitPixelShaderFromFile(const WCHAR*, ID3D11PixelShader**);
	HRESULT CompileShaderFromFile(const WCHAR*, LPCSTR, LPCSTR, ID3DBlob**);
	HRESULT CreateBmpFileFromRgbaSurface(ID3D11RenderTargetView*, LPCWSTR);
	HRESULT CreateBmpFileFromLumaSurface(ID3D11RenderTargetView*, LPCWSTR);
};

#endif
