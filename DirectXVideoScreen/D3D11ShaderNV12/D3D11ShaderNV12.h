//----------------------------------------------------------------------------------------------
// D3D11ShaderNV12.h
//----------------------------------------------------------------------------------------------
#ifndef D3D11SHADERNV12_H
#define D3D11SHADERNV12_H

class CD3D11ShaderNV12{

public:

	CD3D11ShaderNV12(){}
	~CD3D11ShaderNV12(){ OnRelease(); }

	enum SHADER_CONVERSION{ CONVERT_INPUT_SHADER, CONVERT_LUMA_SHADER, CONVERT_CHROMA_SHADER, CONVERT_LUMACHROMA_SHADER, CONVERT_NV12_SHADER };

	HRESULT InitShaderNV12(CWICBitmap&);
	HRESULT ProcessShaderNV12(LPCWSTR, const enum SHADER_CONVERSION);
	void OnRelease();

private:

	ID3D11Device* m_pD3D11Device = NULL;
	ID3D11DeviceContext* m_pD3D11DeviceContext = NULL;

	ID3D11RenderTargetView* m_pViewRT = NULL;
	ID3D11RenderTargetView* m_pLumaRT = NULL;
	ID3D11RenderTargetView* m_pChromaRT = NULL;

	ID3D11ShaderResourceView* m_pInputRSV = NULL;
	ID3D11ShaderResourceView* m_pLumaRSV = NULL;
	ID3D11ShaderResourceView* m_pChromaRSV = NULL;

	ID3D11VertexShader* m_pVertexShader = NULL;
	ID3D11PixelShader* m_pPixelShader = NULL;
	ID3D11PixelShader* m_pLumaShader = NULL;
	ID3D11PixelShader* m_pChromaShader = NULL;
	ID3D11InputLayout* m_pVertexLayout = NULL;
	ID3D11SamplerState* m_pSamplerState = NULL;

	HRESULT InitVertexPixelShaders();
	HRESULT InitTextures(CWICBitmap&);
	HRESULT InitD3D11Resources(const UINT, const UINT);
	HRESULT InitVertexShaderFromFile(const WCHAR*, ID3D11VertexShader**, const BOOL);
	HRESULT InitPixelShaderFromFile(const WCHAR*, ID3D11PixelShader**);
	HRESULT CompileShaderFromFile(const WCHAR*, LPCSTR, LPCSTR, ID3DBlob**);
	HRESULT InitRenderTargetView(const UINT, const UINT);
	HRESULT InitInputTexture(CWICBitmap&);
	HRESULT InitRenderTargetLuma(const UINT, const UINT);
	HRESULT InitRenderTargetChroma(const UINT, const UINT);
	HRESULT CreateBmpFileFromRgbaSurface(ID3D11RenderTargetView*, LPCWSTR);
	HRESULT CreateBmpFileFromLumaSurface(ID3D11RenderTargetView*, LPCWSTR);
	HRESULT CreateBmpFileFromChromaSurface(ID3D11RenderTargetView*, LPCWSTR, const BOOL);
};

#endif
