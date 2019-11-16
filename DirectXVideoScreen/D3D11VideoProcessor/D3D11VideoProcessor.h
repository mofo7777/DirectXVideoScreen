//----------------------------------------------------------------------------------------------
// D3D11VideoProcessor.h
//----------------------------------------------------------------------------------------------
#ifndef D3D11VIDEOPROCESSOR_H
#define D3D11VIDEOPROCESSOR_H

class CD3D11VideoProcessor{

public:

	CD3D11VideoProcessor(){}
	~CD3D11VideoProcessor(){ OnRelease(); }

	HRESULT InitDXVA2(const UINT, const UINT, const DXGI_FORMAT);
	HRESULT ProcessImageConversion(CWICBitmap&, LPCWSTR);
	void OnRelease();

private:

	ID3D11Device* m_pD3D11Device = NULL;
	ID3D11DeviceContext* m_pD3D11DeviceContext = NULL;
	ID3D11VideoContext* m_pD3D11VideoContext = NULL;
	ID3D11VideoProcessorEnumerator* m_pD3D11VideoProcessorEnumerator = NULL;
	ID3D11VideoProcessor* m_pD3D11VideoProcessor = NULL;
	DXGI_FORMAT m_DXGIFormat = DXGI_FORMAT_UNKNOWN;

	HRESULT InitVideoProcessor(ID3D11VideoDevice*, const DXGI_FORMAT, const UINT, const UINT);
	void ConfigureVideoProcessor(const UINT, const UINT);
	HRESULT CreateBmpFileFromYUVSurface(ID3D11Texture2D*, LPCWSTR);
};

#endif