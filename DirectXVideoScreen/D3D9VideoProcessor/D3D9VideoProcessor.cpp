//----------------------------------------------------------------------------------------------
// D3D9VideoProcessor.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

HRESULT CD3D9VideoProcessor::InitDXVA2(const HWND hWnd, const UINT uiWidth, const UINT uiHeight)
{
	HRESULT hr = S_OK;
	IDirect3D9Ex* pD3D9Ex = NULL;

	IF_FAILED_RETURN(Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3D9Ex));

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferWidth = uiWidth;
	d3dpp.BackBufferHeight = uiHeight;
	d3dpp.Windowed = TRUE;

	try
	{
		IF_FAILED_THROW(pD3D9Ex->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, NULL, &m_pDevice9Ex));
		IF_FAILED_THROW(InitVideoProcessor(uiWidth, uiHeight));
		m_uiWidth = uiWidth;
		m_uiHeight = uiHeight;
	}
	catch(HRESULT){}

	SAFE_RELEASE(pD3D9Ex);

	return hr;
}

HRESULT CD3D9VideoProcessor::ProcessImageConversion(LPCWSTR wszImageFile, LPCWSTR wszOutputImageFile)
{
	HRESULT hr = S_OK;
	IDirect3DSurface9* pInputSurface = NULL;
	IDirect3DSurface9* pOutputSurface = NULL;
	DXVAHD_STREAM_DATA stream_data;
	ZeroMemory(&stream_data, sizeof(DXVAHD_STREAM_DATA));

	try
	{
		IF_FAILED_THROW(m_pDXVAHDDevice->CreateVideoSurface(m_uiWidth, m_uiHeight, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 0, DXVAHD_SURFACE_TYPE_VIDEO_INPUT, 1, &pInputSurface, NULL));

		IF_FAILED_THROW(m_pDXVAHDDevice->CreateVideoSurface(m_uiWidth, m_uiHeight, (D3DFORMAT)D3DFMT_NV12, D3DPOOL_DEFAULT, 0, DXVAHD_SURFACE_TYPE_VIDEO_OUTPUT, 1, &pOutputSurface, NULL));

		IF_FAILED_THROW(D3DXLoadSurfaceFromFile(pInputSurface, NULL, NULL, wszImageFile, NULL, D3DX_FILTER_NONE, 0, NULL));

		stream_data.Enable = TRUE;
		stream_data.OutputIndex = 0;
		stream_data.InputFrameOrField = 0;
		stream_data.pInputSurface = pInputSurface;

		IF_FAILED_THROW(m_pDXVAVP->VideoProcessBltHD(pOutputSurface, 0, 1, &stream_data));

		IF_FAILED_THROW(CreateBmpFileFromNV12Surface(pOutputSurface, wszOutputImageFile));
	}
	catch(HRESULT){}

	SAFE_RELEASE(pOutputSurface);
	SAFE_RELEASE(pInputSurface);

	return hr;
}

void CD3D9VideoProcessor::OnRelease()
{
	m_uiDeviceCpas = 0;
	m_uiWidth = 0;
	m_uiHeight = 0;

	SAFE_RELEASE(m_pDXVAVP);
	SAFE_RELEASE(m_pDXVAHDDevice);

	if(m_pDevice9Ex){

		ULONG ulCount = m_pDevice9Ex->Release();
		m_pDevice9Ex = NULL;
		assert(ulCount == 0);
	}
}

HRESULT CD3D9VideoProcessor::InitVideoProcessor(const UINT uiWidth, const UINT uiHeight)
{
	HRESULT hr = S_OK;
	DXVAHD_CONTENT_DESC DxvaHdDesc;
	DXVAHD_VPDEVCAPS caps;
	DXVAHD_VPCAPS* pVPCaps = NULL;

	DxvaHdDesc.InputFrameFormat = DXVAHD_FRAME_FORMAT_PROGRESSIVE;
	DxvaHdDesc.InputFrameRate.Numerator = 30;
	DxvaHdDesc.InputFrameRate.Denominator = 1;
	DxvaHdDesc.InputWidth = uiWidth;
	DxvaHdDesc.InputHeight = uiHeight;
	DxvaHdDesc.OutputFrameRate.Numerator = 30;
	DxvaHdDesc.OutputFrameRate.Denominator = 1;
	DxvaHdDesc.OutputWidth = uiWidth;
	DxvaHdDesc.OutputHeight = uiHeight;

	try
	{
		IF_FAILED_THROW(DXVAHD_CreateDevice(m_pDevice9Ex, &DxvaHdDesc, DXVAHD_DEVICE_USAGE_OPTIMAL_QUALITY, NULL, &m_pDXVAHDDevice));

		ZeroMemory(&caps, sizeof(caps));

		IF_FAILED_THROW(m_pDXVAHDDevice->GetVideoProcessorDeviceCaps(&caps));

		if(caps.VideoProcessorCount > 1)
		{
			// todo : IDXVAHD_Device::GetVideoProcessorDeviceCaps
			MessageBox(NULL, L"VideoProcessorCount > 1", L"todo", MB_OK);
		}

		IF_FAILED_THROW(caps.MaxInputStreams < 1 ? E_FAIL : S_OK);
		IF_FAILED_THROW(caps.MaxStreamStates < 1 ? E_FAIL : S_OK);
		IF_FAILED_THROW(caps.VideoProcessorCount < 1 ? E_FAIL : S_OK);
		IF_FAILED_THROW(caps.OutputFormatCount < 1 || caps.InputFormatCount < 1 ? E_FAIL : S_OK);
		IF_FAILED_THROW(caps.InputPool != D3DPOOL_DEFAULT ? E_FAIL : S_OK);
		IF_FAILED_THROW(caps.DeviceType != DXVAHD_DEVICE_TYPE_HARDWARE ? E_FAIL : S_OK);

		m_uiDeviceCpas = caps.DeviceCaps;

		IF_FAILED_THROW(CheckDxva2Format((D3DFORMAT)D3DFMT_NV12, D3DFMT_A8R8G8B8, caps.OutputFormatCount, caps.InputFormatCount));

		pVPCaps = new (std::nothrow)DXVAHD_VPCAPS[caps.VideoProcessorCount];

		IF_FAILED_THROW(pVPCaps == NULL ? E_OUTOFMEMORY : S_OK);

		IF_FAILED_THROW(m_pDXVAHDDevice->GetVideoProcessorCaps(caps.VideoProcessorCount, pVPCaps));

		IF_FAILED_THROW(m_pDXVAHDDevice->CreateVideoProcessor(&pVPCaps[0].VPGuid, &m_pDXVAVP));

		IF_FAILED_THROW(ConfigureVideoProcessor(D3DFMT_A8R8G8B8, uiWidth, uiHeight));
	}
	catch(HRESULT){}

	SAFE_DELETE_ARRAY(pVPCaps);

	return hr;
}

HRESULT CD3D9VideoProcessor::CheckDxva2Format(const D3DFORMAT OutputFomat, const D3DFORMAT InputFomat, const UINT uiOutputFormatCount, const UINT uiInputFormatCount)
{
	HRESULT hr = S_OK;
	D3DFORMAT* pFormats = NULL;
	UINT uiIndex;

	try
	{
		pFormats = new (std::nothrow)D3DFORMAT[uiOutputFormatCount];

		IF_FAILED_THROW(pFormats == NULL ? E_OUTOFMEMORY : S_OK);

		IF_FAILED_THROW(m_pDXVAHDDevice->GetVideoProcessorOutputFormats(uiOutputFormatCount, pFormats));

		for(uiIndex = 0; uiIndex < uiOutputFormatCount; uiIndex++)
		{
			if(pFormats[uiIndex] == OutputFomat)
			{
				break;
			}
		}

		IF_FAILED_THROW(uiIndex == uiOutputFormatCount ? E_FAIL : S_OK);

		SAFE_DELETE_ARRAY(pFormats);

		pFormats = new (std::nothrow)D3DFORMAT[uiInputFormatCount];

		IF_FAILED_THROW(pFormats == NULL ? E_OUTOFMEMORY : S_OK);

		IF_FAILED_THROW(m_pDXVAHDDevice->GetVideoProcessorInputFormats(uiInputFormatCount, pFormats));

		for(uiIndex = 0; uiIndex < uiInputFormatCount; uiIndex++)
		{
			if(pFormats[uiIndex] == InputFomat)
			{
				break;
			}
		}

		IF_FAILED_THROW(uiIndex == uiInputFormatCount ? E_FAIL : S_OK);
	}
	catch(HRESULT){}

	SAFE_DELETE_ARRAY(pFormats);

	return hr;
}

HRESULT CD3D9VideoProcessor::ConfigureVideoProcessor(const D3DFORMAT d3dFmt, const UINT uiWidth, const UINT uiHeight)
{
	HRESULT hr = S_OK;

	const RECT rc = {0L, 0L, (LONG)uiWidth, (LONG)uiHeight};
	const DXVAHD_STREAM_STATE_FRAME_FORMAT_DATA FrameFormat = {DXVAHD_FRAME_FORMAT_PROGRESSIVE};

	/*
	We could use those capabilities to handle DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE_DATA :
	UINT uiLinearSpace = m_uiDeviceCpas & DXVAHD_DEVICE_CAPS_LINEAR_SPACE;
	UINT uixvYCC = m_uiDeviceCpas & DXVAHD_DEVICE_CAPS_xvYCC;
	UINT uiRgbRangeConversion = m_uiDeviceCpas & DXVAHD_DEVICE_CAPS_RGB_RANGE_CONVERSION;
	UINT uiYCbCrMatrixConversion = m_uiDeviceCpas & DXVAHD_DEVICE_CAPS_YCbCr_MATRIX_CONVERSION;
	*/

	// Type : 0 = Video / 1 = Graphics
	// RGB_Range : 0 = Full range (0-255) / 1 = Limited range (16-235)
	// YCbCr_Matrix : 0 = ITU-R BT.601 / 1 = ITU-R BT.709
	// YCbCr_xvYCC : 0 = Conventional YCbCr / 1 = Extended YCbCr (xvYCC)
	const DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE_DATA Color = {1, 0, 0, 0};
	const DXVAHD_STREAM_STATE_SOURCE_RECT_DATA rcData = {TRUE, rc};

	IF_FAILED_RETURN(m_pDXVAVP->SetVideoProcessStreamState(0, DXVAHD_STREAM_STATE_D3DFORMAT, sizeof(d3dFmt), &d3dFmt));
	IF_FAILED_RETURN(m_pDXVAVP->SetVideoProcessStreamState(0, DXVAHD_STREAM_STATE_FRAME_FORMAT, sizeof(FrameFormat), &FrameFormat));
	IF_FAILED_RETURN(m_pDXVAVP->SetVideoProcessStreamState(0, DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE, sizeof(Color), &Color));
	IF_FAILED_RETURN(m_pDXVAVP->SetVideoProcessStreamState(0, DXVAHD_STREAM_STATE_SOURCE_RECT, sizeof(rcData), &rcData));
	IF_FAILED_RETURN(m_pDXVAVP->SetVideoProcessStreamState(0, DXVAHD_STREAM_STATE_DESTINATION_RECT, sizeof(rcData), &rcData));

	return hr;
}

HRESULT CD3D9VideoProcessor::CreateBmpFileFromNV12Surface(IDirect3DSurface9* pOutputSurface, LPCWSTR wszOutputImageFile)
{
	HRESULT hr = S_OK;
	D3DSURFACE_DESC desc;
	D3DLOCKED_RECT d3dLock;

	IF_FAILED_RETURN(pOutputSurface->GetDesc(&desc));

	IF_FAILED_RETURN(pOutputSurface->LockRect(&d3dLock, NULL, D3DLOCK_READONLY));

	hr = ProcessNV12ToBmpFile(wszOutputImageFile, (BYTE*)d3dLock.pBits, d3dLock.Pitch, desc.Width, desc.Height);

	pOutputSurface->UnlockRect();

	return hr;
}