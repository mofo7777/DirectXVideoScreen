//----------------------------------------------------------------------------------------------
// D3D11VideoProcessor.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

HRESULT CD3D11VideoProcessor::InitDXVA2(const HWND, const UINT uiWidth, const UINT uiHeight, const DXGI_FORMAT dxgiFormat)
{
	HRESULT hr = S_OK;

	IF_FAILED_RETURN(m_pD3D11VideoContext != NULL ? E_UNEXPECTED : S_OK);

	ID3D11VideoDevice* pD3D11VideoDevice = NULL;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	UINT uiFeatureLevels = ARRAYSIZE(featureLevels);
	D3D_FEATURE_LEVEL featureLevel;

	try
	{
		IF_FAILED_THROW(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, uiFeatureLevels, D3D11_SDK_VERSION, &m_pD3D11Device, &featureLevel, &m_pD3D11DeviceContext));
		IF_FAILED_THROW(m_pD3D11Device->QueryInterface(__uuidof(ID3D11VideoDevice), reinterpret_cast<void**>(&pD3D11VideoDevice)));
		IF_FAILED_THROW(InitVideoProcessor(pD3D11VideoDevice, dxgiFormat, uiWidth, uiHeight));
		IF_FAILED_THROW(m_pD3D11DeviceContext->QueryInterface(__uuidof(ID3D11VideoContext), (void**)&m_pD3D11VideoContext));
		ConfigureVideoProcessor(uiWidth, uiHeight);
		m_DXGIFormat = dxgiFormat;
	}
	catch(HRESULT){}

	SAFE_RELEASE(pD3D11VideoDevice);

	return hr;
}

HRESULT CD3D11VideoProcessor::ProcessImageConversion(CWICBitmap& cWICBitmap, LPCWSTR wszOutputImageFile)
{
	HRESULT hr = S_OK;
	ID3D11VideoProcessorInputView* pD3D11VideoProcessorInputViewIn = NULL;
	ID3D11VideoProcessorOutputView* pD3D11VideoProcessorOutputView = NULL;
	ID3D11VideoDevice* pD3D11VideoDevice = NULL;
	ID3D11Texture2D* pInTexture2D = NULL;
	ID3D11Texture2D* pOutTexture2D = NULL;

	IF_FAILED_RETURN(m_pD3D11VideoContext == NULL ? E_UNEXPECTED : S_OK);

	try
	{
		IF_FAILED_THROW(m_pD3D11Device->QueryInterface(__uuidof(ID3D11VideoDevice), reinterpret_cast<void**>(&pD3D11VideoDevice)));

		D3D11_TEXTURE2D_DESC desc2D;
		desc2D.Width = cWICBitmap.GetWidth();
		desc2D.Height = cWICBitmap.GetHeight();
		desc2D.MipLevels = 1;
		desc2D.ArraySize = 1;
		desc2D.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc2D.SampleDesc.Count = 1;
		desc2D.SampleDesc.Quality = 0;
		desc2D.Usage = D3D11_USAGE_DEFAULT;
		desc2D.BindFlags = D3D11_BIND_RENDER_TARGET;
		desc2D.CPUAccessFlags = 0;
		desc2D.MiscFlags = 0;

		IF_FAILED_THROW(cWICBitmap.Create2DTextureFromBitmap(m_pD3D11Device, &pInTexture2D, desc2D));

		D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC pInDesc;
		ZeroMemory(&pInDesc, sizeof(pInDesc));
		pInDesc.FourCC = DXGI_FORMAT_NV12;
		pInDesc.ViewDimension = D3D11_VPIV_DIMENSION_TEXTURE2D;
		pInDesc.Texture2D.MipSlice = 0;
		pInDesc.Texture2D.ArraySlice = 0;

		IF_FAILED_THROW(pD3D11VideoDevice->CreateVideoProcessorInputView(pInTexture2D, m_pD3D11VideoProcessorEnumerator, &pInDesc, &pD3D11VideoProcessorInputViewIn));

		desc2D.Format = DXGI_FORMAT_NV12;

		D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC pOutDesc;
		ZeroMemory(&pOutDesc, sizeof(pOutDesc));
		pOutDesc.ViewDimension = D3D11_VPOV_DIMENSION_TEXTURE2D;
		pOutDesc.Texture2D.MipSlice = 0;

		IF_FAILED_THROW(pD3D11VideoDevice->CreateVideoProcessorOutputView(pOutTexture2D, m_pD3D11VideoProcessorEnumerator, &pOutDesc, &pD3D11VideoProcessorOutputView));

		D3D11_VIDEO_PROCESSOR_STREAM StreamData;
		ZeroMemory(&StreamData, sizeof(StreamData));
		StreamData.Enable = TRUE;
		StreamData.OutputIndex = 0;
		StreamData.InputFrameOrField = 0;
		StreamData.PastFrames = 0;
		StreamData.FutureFrames = 0;
		StreamData.ppPastSurfaces = NULL;
		StreamData.ppFutureSurfaces = NULL;
		StreamData.pInputSurface = pD3D11VideoProcessorInputViewIn;
		StreamData.ppPastSurfacesRight = NULL;
		StreamData.ppFutureSurfacesRight = NULL;

		IF_FAILED_THROW(m_pD3D11VideoContext->VideoProcessorBlt(m_pD3D11VideoProcessor, pD3D11VideoProcessorOutputView, 0, 1, &StreamData));

		IF_FAILED_THROW(CreateBmpFileFromYUVSurface(pOutTexture2D, wszOutputImageFile));
	}
	catch(HRESULT){}

	SAFE_RELEASE(pOutTexture2D);
	SAFE_RELEASE(pInTexture2D);
	SAFE_RELEASE(pD3D11VideoProcessorOutputView);
	SAFE_RELEASE(pD3D11VideoProcessorInputViewIn);
	SAFE_RELEASE(pD3D11VideoDevice);

	return hr;
}

void CD3D11VideoProcessor::OnRelease()
{
	SAFE_RELEASE(m_pD3D11VideoProcessorEnumerator);
	SAFE_RELEASE(m_pD3D11VideoContext);
	SAFE_RELEASE(m_pD3D11VideoProcessor);

	if(m_pD3D11DeviceContext)
	{
		m_pD3D11DeviceContext->ClearState();
		m_pD3D11DeviceContext->Release();
		m_pD3D11DeviceContext = NULL;
	}

	if(m_pD3D11Device){

		ULONG ulCount = m_pD3D11Device->Release();
		m_pD3D11Device = NULL;
		assert(ulCount == 0);
	}

	m_DXGIFormat = DXGI_FORMAT_UNKNOWN;
}

HRESULT CD3D11VideoProcessor::InitVideoProcessor(ID3D11VideoDevice* pD3D11VideoDevice, const DXGI_FORMAT dxgiFormat, const UINT uiWidth, const UINT uiHeight)
{
	HRESULT hr = S_OK;
	D3D11_VIDEO_PROCESSOR_CAPS VPCaps;
	UINT uiFormat;

	D3D11_VIDEO_PROCESSOR_CONTENT_DESC descVP;
	descVP.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
	descVP.InputFrameRate.Numerator = 30;
	descVP.InputFrameRate.Denominator = 1;
	descVP.InputWidth = uiWidth;
	descVP.InputHeight = uiHeight;
	descVP.OutputFrameRate.Numerator = 30;
	descVP.OutputFrameRate.Denominator = 1;
	descVP.OutputWidth = uiWidth;
	descVP.OutputHeight = uiHeight;
	descVP.Usage = D3D11_VIDEO_USAGE_OPTIMAL_QUALITY;

	try
	{
		IF_FAILED_THROW(pD3D11VideoDevice->CreateVideoProcessorEnumerator(&descVP, &m_pD3D11VideoProcessorEnumerator));
		IF_FAILED_THROW(m_pD3D11VideoProcessorEnumerator->GetVideoProcessorCaps(&VPCaps));

		IF_FAILED_THROW(VPCaps.MaxInputStreams < 1 ? E_FAIL : S_OK);
		IF_FAILED_THROW(VPCaps.MaxStreamStates < 1 ? E_FAIL : S_OK);

		uiFormat = D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_INPUT;
		IF_FAILED_THROW(m_pD3D11VideoProcessorEnumerator->CheckVideoProcessorFormat(DXGI_FORMAT_R8G8B8A8_UNORM, &uiFormat));

		uiFormat = D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_OUTPUT;
		IF_FAILED_THROW(m_pD3D11VideoProcessorEnumerator->CheckVideoProcessorFormat(dxgiFormat, &uiFormat));

		IF_FAILED_THROW(pD3D11VideoDevice->CreateVideoProcessor(m_pD3D11VideoProcessorEnumerator, 0, &m_pD3D11VideoProcessor));
	}
	catch(HRESULT){}

	return hr;
}

void CD3D11VideoProcessor::ConfigureVideoProcessor(const UINT uiWidth, const UINT uiHeight)
{
	RECT rc = {0, 0, (LONG)uiWidth, (LONG)uiHeight};

	m_pD3D11VideoContext->VideoProcessorSetStreamFrameFormat(m_pD3D11VideoProcessor, 0, D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE);
	m_pD3D11VideoContext->VideoProcessorSetStreamOutputRate(m_pD3D11VideoProcessor, 0, D3D11_VIDEO_PROCESSOR_OUTPUT_RATE_NORMAL, TRUE, NULL);
	m_pD3D11VideoContext->VideoProcessorSetStreamSourceRect(m_pD3D11VideoProcessor, 0, TRUE, &rc);
	m_pD3D11VideoContext->VideoProcessorSetStreamDestRect(m_pD3D11VideoProcessor, 0, TRUE, &rc);
	m_pD3D11VideoContext->VideoProcessorSetOutputTargetRect(m_pD3D11VideoProcessor, TRUE, &rc);

	/*D3D11_VIDEO_PROCESSOR_COLOR_SPACE D3DColorSpace = {};
	D3DColorSpace.YCbCr_xvYCC = 1;

	m_pD3D11VideoContext->VideoProcessorSetStreamColorSpace(m_pD3D11VideoProcessor, 0, &D3DColorSpace);
	m_pD3D11VideoContext->VideoProcessorSetOutputColorSpace(m_pD3D11VideoProcessor, &D3DColorSpace);*/
}

HRESULT CD3D11VideoProcessor::CreateBmpFileFromYUVSurface(ID3D11Texture2D* pD3D11Texture2D, LPCWSTR wszOutputImageFile)
{
	HRESULT hr = S_OK;
	return hr;
}