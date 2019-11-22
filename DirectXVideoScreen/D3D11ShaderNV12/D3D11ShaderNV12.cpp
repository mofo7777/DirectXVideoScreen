//----------------------------------------------------------------------------------------------
// D3D11ShaderNV12.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

HRESULT CD3D11ShaderNV12::InitShaderNV12(CWICBitmap& cWICBitmap)
{
	HRESULT hr = S_OK;

	IF_FAILED_RETURN(m_pSamplerState != NULL ? E_UNEXPECTED : S_OK);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	UINT uiFeatureLevels = ARRAYSIZE(featureLevels);
	D3D_FEATURE_LEVEL featureLevel;
	UINT uiD3D11CreateFlag = D3D11_CREATE_DEVICE_SINGLETHREADED;
	//-->UINT uiFormatSupport;

#ifdef _DEBUG
	uiD3D11CreateFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	try
	{
		IF_FAILED_THROW(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, uiD3D11CreateFlag, featureLevels, uiFeatureLevels, D3D11_SDK_VERSION, &m_pD3D11Device, &featureLevel, &m_pD3D11DeviceContext));
		//-->IF_FAILED_THROW(m_pD3D11Device->CheckFormatSupport(DXGI_FORMAT_NV12, &uiFormatSupport));
		IF_FAILED_THROW(InitVertexPixelShaders());
		IF_FAILED_THROW(InitTextures(cWICBitmap));
		IF_FAILED_THROW(InitD3D11Resources(cWICBitmap.GetWidth(), cWICBitmap.GetHeight()));
	}
	catch(HRESULT){}

	return hr;
}

HRESULT CD3D11ShaderNV12::ProcessShaderNV12(LPCWSTR wszOutputImageFile1, LPCWSTR wszOutputImageFile2, LPCWSTR wszOutputImageFile3, const enum SHADER_CONVERSION ShaderConversion)
{
	HRESULT hr = S_OK;

	IF_FAILED_RETURN(m_pSamplerState == NULL ? E_UNEXPECTED : S_OK);

	m_pD3D11DeviceContext->ClearRenderTargetView(m_pViewRT, DirectX::Colors::Aquamarine);

	if(ShaderConversion == CONVERT_INPUT_SHADER)
	{
		m_pD3D11DeviceContext->OMSetRenderTargets(1, &m_pViewRT, NULL);
		m_pD3D11DeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
		m_pD3D11DeviceContext->PSSetShader(m_pPixelShader, NULL, 0);
		m_pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pInputRSV);
		m_pD3D11DeviceContext->Draw(4, 0);
		m_pD3D11DeviceContext->Flush();

		IF_FAILED_RETURN(CreateBmpFileFromRgbaSurface(m_pViewRT, wszOutputImageFile1));
	}
	else if(ShaderConversion == CONVERT_LUMA_SHADER)
	{
		m_pD3D11DeviceContext->OMSetRenderTargets(1, &m_pLumaRT, NULL);
		m_pD3D11DeviceContext->ClearRenderTargetView(m_pLumaRT, DirectX::Colors::Aquamarine);
		m_pD3D11DeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
		m_pD3D11DeviceContext->PSSetShader(m_pLumaShader, NULL, 0);
		m_pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pInputRSV);
		m_pD3D11DeviceContext->Draw(4, 0);
		m_pD3D11DeviceContext->Flush();

		IF_FAILED_RETURN(CreateBmpFileFromLumaSurface(m_pLumaRT, wszOutputImageFile1));
	}
	else if(ShaderConversion == CONVERT_CHROMA_SHADER)
	{
		m_pD3D11DeviceContext->OMSetRenderTargets(1, &m_pChromaRT, NULL);
		m_pD3D11DeviceContext->ClearRenderTargetView(m_pChromaRT, DirectX::Colors::Aquamarine);
		m_pD3D11DeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
		m_pD3D11DeviceContext->PSSetShader(m_pChromaShader, NULL, 0);
		m_pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pInputRSV);
		m_pD3D11DeviceContext->Draw(4, 0);
		m_pD3D11DeviceContext->Flush();

		IF_FAILED_RETURN(CreateBmpFileFromChromaSurface(m_pChromaRT, wszOutputImageFile1, TRUE));
		IF_FAILED_RETURN(CreateBmpFileFromChromaSurface(m_pChromaRT, wszOutputImageFile2, FALSE));
	}
	else if(ShaderConversion == CONVERT_LUMACHROMA_SHADER)
	{
		ID3D11RenderTargetView* pYCbCrRT[2];
		pYCbCrRT[0] = m_pLumaRT;
		pYCbCrRT[1] = m_pChromaRT;

		m_pD3D11DeviceContext->ClearRenderTargetView(m_pLumaRT, DirectX::Colors::Aquamarine);
		m_pD3D11DeviceContext->ClearRenderTargetView(m_pChromaRT, DirectX::Colors::Aquamarine);

		m_pD3D11DeviceContext->OMSetRenderTargets(2, pYCbCrRT, NULL);
		
		m_pD3D11DeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
		m_pD3D11DeviceContext->PSSetShader(m_pYCbCrShader, NULL, 0);
		m_pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pInputRSV);
		m_pD3D11DeviceContext->Draw(4, 0);
		m_pD3D11DeviceContext->Flush();

		IF_FAILED_RETURN(CreateBmpFileFromLumaSurface(m_pLumaRT, wszOutputImageFile1));
		IF_FAILED_RETURN(CreateBmpFileFromChromaSurface(m_pChromaRT, wszOutputImageFile2, TRUE));
		IF_FAILED_RETURN(CreateBmpFileFromChromaSurface(m_pChromaRT, wszOutputImageFile3, FALSE));
	}
	else
	{
		IF_FAILED_RETURN(E_NOTIMPL);
	}

	return hr;
}

void CD3D11ShaderNV12::OnRelease()
{
	SAFE_RELEASE(m_pViewRT);
	SAFE_RELEASE(m_pLumaRT);
	SAFE_RELEASE(m_pChromaRT);
	SAFE_RELEASE(m_pInputRSV);
	SAFE_RELEASE(m_pLumaRSV);
	SAFE_RELEASE(m_pChromaRSV);

	SAFE_RELEASE(m_pSamplerState);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pLumaShader);
	SAFE_RELEASE(m_pChromaShader);
	SAFE_RELEASE(m_pYCbCrShader);
	SAFE_RELEASE(m_pVertexShader);

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
}

HRESULT CD3D11ShaderNV12::InitVertexPixelShaders()
{
	HRESULT hr = S_OK;

	IF_FAILED_RETURN(InitVertexShaderFromFile(L"ScreenVS.hlsl", &m_pVertexShader, TRUE));
	IF_FAILED_RETURN(InitPixelShaderFromFile(L"ScreenPS.hlsl", &m_pPixelShader));
	IF_FAILED_RETURN(InitPixelShaderFromFile(L"LumaPS.hlsl", &m_pLumaShader));
	IF_FAILED_RETURN(InitPixelShaderFromFile(L"ChromaPS.hlsl", &m_pChromaShader));
	IF_FAILED_RETURN(InitPixelShaderFromFile(L"YCbCrPS.hlsl", &m_pYCbCrShader));

	return hr;
}

HRESULT CD3D11ShaderNV12::InitTextures(CWICBitmap& cWICBitmap)
{
	HRESULT hr = S_OK;

	IF_FAILED_RETURN(InitRenderTargetView(cWICBitmap.GetWidth(), cWICBitmap.GetHeight()));
	IF_FAILED_RETURN(InitInputTexture(cWICBitmap));
	IF_FAILED_RETURN(InitRenderTargetLuma(cWICBitmap.GetWidth(), cWICBitmap.GetHeight()));
	IF_FAILED_RETURN(InitRenderTargetChroma(cWICBitmap.GetWidth(), cWICBitmap.GetHeight()));

	return hr;
}

HRESULT CD3D11ShaderNV12::InitD3D11Resources(const UINT uiWidth, const UINT uiHeight)
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC SampDesc;
	ZeroMemory(&SampDesc, sizeof(SampDesc));

	SampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SampDesc.MinLOD = 0;
	SampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	IF_FAILED_RETURN(m_pD3D11Device->CreateSamplerState(&SampDesc, &m_pSamplerState));

	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)uiWidth;
	vp.Height = (FLOAT)uiHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	m_pD3D11DeviceContext->RSSetViewports(1, &vp);
	m_pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_pD3D11DeviceContext->IASetInputLayout(m_pVertexLayout);
	m_pD3D11DeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);

	return hr;
}

HRESULT CD3D11ShaderNV12::InitVertexShaderFromFile(const WCHAR* wszShaderFile, ID3D11VertexShader** ppID3D11VertexShader, const BOOL bCreateLayout)
{
	HRESULT hr = S_OK;
	ID3DBlob* pVSBlob = NULL;

	try
	{
		IF_FAILED_THROW(CompileShaderFromFile(wszShaderFile, "VS", "vs_5_0", &pVSBlob));
		IF_FAILED_THROW(m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, ppID3D11VertexShader));

		if(bCreateLayout)
		{
			D3D11_INPUT_ELEMENT_DESC layout[] = {{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},};
			UINT numElements = ARRAYSIZE(layout);

			IF_FAILED_THROW(m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVertexLayout));
		}
	}
	catch(HRESULT){}

	SAFE_RELEASE(pVSBlob);

	return hr;
}

HRESULT CD3D11ShaderNV12::InitPixelShaderFromFile(const WCHAR* wszShaderFile, ID3D11PixelShader** ppID3D11PixelShader)
{
	HRESULT hr = S_OK;
	ID3DBlob* pPSBlob = NULL;

	try
	{
		IF_FAILED_THROW(CompileShaderFromFile(wszShaderFile, "PS", "ps_5_0", &pPSBlob));
		IF_FAILED_THROW(m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, ppID3D11PixelShader));
	}
	catch(HRESULT){}

	SAFE_RELEASE(pPSBlob);

	return hr;
}

HRESULT CD3D11ShaderNV12::CompileShaderFromFile(const WCHAR* wszShaderFile, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;
	ID3DBlob* pErrorBlob = NULL;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	hr = D3DCompileFromFile(wszShaderFile, NULL, NULL, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

	if(FAILED(hr) && pErrorBlob)
	{
		OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
	}

	SAFE_RELEASE(pErrorBlob);

	return hr;
}

HRESULT CD3D11ShaderNV12::InitRenderTargetView(const UINT uiWidth, const UINT uiHeight)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* pViewTexture2D = NULL;

	D3D11_TEXTURE2D_DESC desc2D;
	desc2D.Width = uiWidth;
	desc2D.Height = uiHeight;
	desc2D.MipLevels = 1;
	desc2D.ArraySize = 1;
	desc2D.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc2D.SampleDesc.Count = 1;
	desc2D.SampleDesc.Quality = 0;
	desc2D.Usage = D3D11_USAGE_DEFAULT;
	desc2D.BindFlags = D3D11_BIND_RENDER_TARGET;
	desc2D.CPUAccessFlags = 0;
	desc2D.MiscFlags = 0;

	try
	{
		IF_FAILED_THROW(m_pD3D11Device->CreateTexture2D(&desc2D, NULL, &pViewTexture2D));
		IF_FAILED_THROW(m_pD3D11Device->CreateRenderTargetView(pViewTexture2D, NULL, &m_pViewRT));
	}
	catch(HRESULT){}

	SAFE_RELEASE(pViewTexture2D);

	return hr;
}

HRESULT CD3D11ShaderNV12::InitInputTexture(CWICBitmap& cWICBitmap)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* pInTexture2D = NULL;

	D3D11_TEXTURE2D_DESC desc2D;
	desc2D.Width = cWICBitmap.GetWidth();
	desc2D.Height = cWICBitmap.GetHeight();
	desc2D.MipLevels = 1;
	desc2D.ArraySize = 1;
	desc2D.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc2D.SampleDesc.Count = 1;
	desc2D.SampleDesc.Quality = 0;
	desc2D.Usage = D3D11_USAGE_DEFAULT;
	desc2D.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc2D.CPUAccessFlags = 0;
	desc2D.MiscFlags = 0;

	try
	{
		IF_FAILED_THROW(cWICBitmap.Create2DTextureFromBitmap(m_pD3D11Device, &pInTexture2D, desc2D));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc2D;
		srvDesc2D.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc2D.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc2D.Texture2D.MipLevels = 1;
		srvDesc2D.Texture2D.MostDetailedMip = 0;

		IF_FAILED_THROW(m_pD3D11Device->CreateShaderResourceView(pInTexture2D, &srvDesc2D, &m_pInputRSV));
	}
	catch(HRESULT){}

	SAFE_RELEASE(pInTexture2D);

	return hr;
}

HRESULT CD3D11ShaderNV12::InitRenderTargetLuma(const UINT uiWidth, const UINT uiHeight)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* pTexture2D = NULL;

	D3D11_TEXTURE2D_DESC desc2D;
	desc2D.Width = uiWidth;
	desc2D.Height = uiHeight;
	desc2D.MipLevels = 1;
	desc2D.ArraySize = 1;
	desc2D.Format = DXGI_FORMAT_R8_UNORM;
	desc2D.SampleDesc.Count = 1;
	desc2D.SampleDesc.Quality = 0;
	desc2D.Usage = D3D11_USAGE_DEFAULT;
	desc2D.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc2D.CPUAccessFlags = 0;
	desc2D.MiscFlags = 0;

	try
	{
		IF_FAILED_THROW(m_pD3D11Device->CreateTexture2D(&desc2D, NULL, &pTexture2D));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc2D;
		srvDesc2D.Format = DXGI_FORMAT_R8_UNORM;
		srvDesc2D.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc2D.Texture2D.MipLevels = 1;
		srvDesc2D.Texture2D.MostDetailedMip = 0;

		IF_FAILED_THROW(m_pD3D11Device->CreateShaderResourceView(pTexture2D, &srvDesc2D, &m_pLumaRSV));

		D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
		rtDesc.Format = DXGI_FORMAT_R8_UNORM;
		rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtDesc.Texture2D.MipSlice = 0;

		IF_FAILED_THROW(m_pD3D11Device->CreateRenderTargetView(pTexture2D, &rtDesc, &m_pLumaRT));
	}
	catch(HRESULT){}

	SAFE_RELEASE(pTexture2D);

	return hr;
}

HRESULT CD3D11ShaderNV12::InitRenderTargetChroma(const UINT uiWidth, const UINT uiHeight)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* pTexture2D = NULL;

	D3D11_TEXTURE2D_DESC desc2D;
	desc2D.Width = uiWidth;
	desc2D.Height = uiHeight;
	desc2D.MipLevels = 1;
	desc2D.ArraySize = 1;
	desc2D.Format = DXGI_FORMAT_R8G8_UNORM;
	desc2D.SampleDesc.Count = 1;
	desc2D.SampleDesc.Quality = 0;
	desc2D.Usage = D3D11_USAGE_DEFAULT;
	desc2D.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc2D.CPUAccessFlags = 0;
	desc2D.MiscFlags = 0;

	try
	{
		IF_FAILED_THROW(m_pD3D11Device->CreateTexture2D(&desc2D, NULL, &pTexture2D));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc2D;
		srvDesc2D.Format = DXGI_FORMAT_R8G8_UNORM;
		srvDesc2D.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc2D.Texture2D.MipLevels = 1;
		srvDesc2D.Texture2D.MostDetailedMip = 0;

		IF_FAILED_THROW(m_pD3D11Device->CreateShaderResourceView(pTexture2D, &srvDesc2D, &m_pChromaRSV));

		D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
		rtDesc.Format = DXGI_FORMAT_R8G8_UNORM;
		rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtDesc.Texture2D.MipSlice = 0;

		IF_FAILED_THROW(m_pD3D11Device->CreateRenderTargetView(pTexture2D, &rtDesc, &m_pChromaRT));
	}
	catch(HRESULT){}

	SAFE_RELEASE(pTexture2D);

	return hr;
}

HRESULT CD3D11ShaderNV12::CreateBmpFileFromRgbaSurface(ID3D11RenderTargetView* pD3D11RenderTargetView, LPCWSTR wszOutputImageFile)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* pInTexture2 = NULL;
	ID3D11Texture2D* pOutTexture2D = NULL;
	D3D11_TEXTURE2D_DESC desc2D;
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	UINT uiSubResource = D3D11CalcSubresource(0, 0, 0);
	BYTE* pDataRgb = NULL;

	try
	{
		pD3D11RenderTargetView->GetResource(reinterpret_cast<ID3D11Resource**>(&pInTexture2));
		pInTexture2->GetDesc(&desc2D);

		UINT uiImageSize = desc2D.Width * desc2D.Height * 3;

		pDataRgb = new (std::nothrow)BYTE[uiImageSize];
		IF_FAILED_THROW(pDataRgb == NULL ? E_OUTOFMEMORY : S_OK);

		desc2D.BindFlags = 0;
		desc2D.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc2D.Usage = D3D11_USAGE_STAGING;
		desc2D.MiscFlags = 0;

		IF_FAILED_THROW(m_pD3D11Device->CreateTexture2D(&desc2D, NULL, &pOutTexture2D));

		m_pD3D11DeviceContext->CopyResource(pOutTexture2D, pInTexture2);

		IF_FAILED_THROW(m_pD3D11DeviceContext->Map(pOutTexture2D, uiSubResource, D3D11_MAP_READ, 0, &MappedSubResource));

		BYTE* pDataRgbaColor = (BYTE*)MappedSubResource.pData;
		BYTE* pDataRgbColor = pDataRgb;

		for(UINT i = 0; i < desc2D.Height; i++)
		{
			for(UINT j = 0; j < desc2D.Width; j++)
			{
				*pDataRgbColor++ = *pDataRgbaColor++;
				*pDataRgbColor++ = *pDataRgbaColor++;
				*pDataRgbColor++ = *pDataRgbaColor++;
				pDataRgbaColor++;
			}
		}

		m_pD3D11DeviceContext->Unmap(pOutTexture2D, uiSubResource);

		IF_FAILED_THROW(CreateBmpFile(wszOutputImageFile, pDataRgb, uiImageSize, desc2D.Width, desc2D.Height));
	}
	catch(HRESULT){}

	SAFE_DELETE_ARRAY(pDataRgb);
	SAFE_RELEASE(pOutTexture2D);
	SAFE_RELEASE(pInTexture2);

	return hr;
}

HRESULT CD3D11ShaderNV12::CreateBmpFileFromLumaSurface(ID3D11RenderTargetView* pD3D11RenderTargetView, LPCWSTR wszOutputImageFile)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* pInTexture2 = NULL;
	ID3D11Texture2D* pOutTexture2D = NULL;
	D3D11_TEXTURE2D_DESC desc2D;
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	UINT uiSubResource = D3D11CalcSubresource(0, 0, 0);
	BYTE* pDataRgb = NULL;

	try
	{
		pD3D11RenderTargetView->GetResource(reinterpret_cast<ID3D11Resource**>(&pInTexture2));
		pInTexture2->GetDesc(&desc2D);

		UINT uiImageSize = desc2D.Width * desc2D.Height * 3;

		pDataRgb = new (std::nothrow)BYTE[uiImageSize];
		IF_FAILED_THROW(pDataRgb == NULL ? E_OUTOFMEMORY : S_OK);

		desc2D.BindFlags = 0;
		desc2D.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc2D.Usage = D3D11_USAGE_STAGING;
		desc2D.MiscFlags = 0;

		IF_FAILED_THROW(m_pD3D11Device->CreateTexture2D(&desc2D, NULL, &pOutTexture2D));

		m_pD3D11DeviceContext->CopyResource(pOutTexture2D, pInTexture2);

		IF_FAILED_THROW(m_pD3D11DeviceContext->Map(pOutTexture2D, uiSubResource, D3D11_MAP_READ, 0, &MappedSubResource));

		BYTE* pDataYColor = (BYTE*)MappedSubResource.pData;
		BYTE* pDataRgbColor = pDataRgb;

		for(UINT i = 0; i < desc2D.Height; i++)
		{
			for(UINT j = 0; j < desc2D.Width; j++)
			{
				*pDataRgbColor++ = *pDataYColor;
				*pDataRgbColor++ = *pDataYColor;
				*pDataRgbColor++ = *pDataYColor++;
			}
		}

		m_pD3D11DeviceContext->Unmap(pOutTexture2D, uiSubResource);

		IF_FAILED_THROW(CreateBmpFile(wszOutputImageFile, pDataRgb, uiImageSize, desc2D.Width, desc2D.Height));
	}
	catch(HRESULT){}

	SAFE_DELETE_ARRAY(pDataRgb);
	SAFE_RELEASE(pOutTexture2D);
	SAFE_RELEASE(pInTexture2);

	return hr;
}

HRESULT CD3D11ShaderNV12::CreateBmpFileFromChromaSurface(ID3D11RenderTargetView* pD3D11RenderTargetView, LPCWSTR wszOutputImageFile, const BOOL bU)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* pInTexture2 = NULL;
	ID3D11Texture2D* pOutTexture2D = NULL;
	D3D11_TEXTURE2D_DESC desc2D;
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	UINT uiSubResource = D3D11CalcSubresource(0, 0, 0);
	BYTE* pDataRgb = NULL;

	try
	{
		pD3D11RenderTargetView->GetResource(reinterpret_cast<ID3D11Resource**>(&pInTexture2));
		pInTexture2->GetDesc(&desc2D);

		UINT uiImageSize = desc2D.Width * desc2D.Height * 3;

		pDataRgb = new (std::nothrow)BYTE[uiImageSize];
		IF_FAILED_THROW(pDataRgb == NULL ? E_OUTOFMEMORY : S_OK);

		desc2D.BindFlags = 0;
		desc2D.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc2D.Usage = D3D11_USAGE_STAGING;
		desc2D.MiscFlags = 0;

		IF_FAILED_THROW(m_pD3D11Device->CreateTexture2D(&desc2D, NULL, &pOutTexture2D));

		m_pD3D11DeviceContext->CopyResource(pOutTexture2D, pInTexture2);

		IF_FAILED_THROW(m_pD3D11DeviceContext->Map(pOutTexture2D, uiSubResource, D3D11_MAP_READ, 0, &MappedSubResource));

		BYTE* pDataUVColor = (BYTE*)MappedSubResource.pData;
		BYTE* pDataRgbColor = pDataRgb;

		for(UINT i = 0; i < desc2D.Height; i++)
		{
			for(UINT j = 0; j < desc2D.Width; j++)
			{
				if(bU == TRUE)
				{
					// Show U
					*pDataRgbColor++ = *pDataUVColor;
					*pDataRgbColor++ = *pDataUVColor;
					*pDataRgbColor++ = *pDataUVColor++;
					pDataUVColor++;
				}
				else
				{
					// Show V
					pDataUVColor++;
					*pDataRgbColor++ = *pDataUVColor;
					*pDataRgbColor++ = *pDataUVColor;
					*pDataRgbColor++ = *pDataUVColor++;
				}
				
			}
		}

		m_pD3D11DeviceContext->Unmap(pOutTexture2D, uiSubResource);

		IF_FAILED_THROW(CreateBmpFile(wszOutputImageFile, pDataRgb, uiImageSize, desc2D.Width, desc2D.Height));
	}
	catch(HRESULT){}

	SAFE_DELETE_ARRAY(pDataRgb);
	SAFE_RELEASE(pOutTexture2D);
	SAFE_RELEASE(pInTexture2);

	return hr;
}