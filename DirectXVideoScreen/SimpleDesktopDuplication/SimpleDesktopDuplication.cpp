//----------------------------------------------------------------------------------------------
// SimpleDesktopDuplication.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

HRESULT CSimpleDesktopDuplication::InitDesktopDuplication()
{
	HRESULT hr = S_OK;

	IF_FAILED_RETURN(m_pDXGIOutputDuplication != NULL ? E_UNEXPECTED : S_OK);

	IDXGIDevice* pDXGIDevice = NULL;
	IDXGIAdapter* pDXGIAdapter = NULL;
	IDXGIOutput* pDXGIOutput = NULL;
	IDXGIOutput1* pDXGIOutput1 = NULL;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	UINT uiFeatureLevels = ARRAYSIZE(featureLevels);
	D3D_FEATURE_LEVEL featureLevel;
	UINT uiD3D11CreateFlag = D3D11_CREATE_DEVICE_SINGLETHREADED;
	DXGI_OUTDUPL_DESC OutputDesc;

#ifdef _DEBUG
	uiD3D11CreateFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	try
	{
		IF_FAILED_THROW(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, uiD3D11CreateFlag, featureLevels, uiFeatureLevels, D3D11_SDK_VERSION, &m_pD3D11Device, &featureLevel, &m_pD3D11DeviceContext));
		IF_FAILED_THROW(m_pD3D11Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDXGIDevice)));
		IF_FAILED_THROW(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDXGIAdapter)));
		IF_FAILED_THROW(pDXGIAdapter->EnumOutputs(0, &pDXGIOutput));
		IF_FAILED_THROW(pDXGIOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&pDXGIOutput1)));
		IF_FAILED_THROW(pDXGIOutput1->DuplicateOutput(m_pD3D11Device, &m_pDXGIOutputDuplication));
		m_pDXGIOutputDuplication->GetDesc(&OutputDesc);

		/*
		typedef struct DXGI_OUTDUPL_DESC {
			DXGI_MODE_DESC     ModeDesc;
			DXGI_MODE_ROTATION Rotation;
			BOOL               DesktopImageInSystemMemory;
		} DXGI_OUTDUPL_DESC;

		DesktopImageInSystemMemory

		Specifies whether the resource that contains the desktop image is already located in system memory.
		TRUE if the resource is in system memory; otherwise, FALSE. If this value is TRUE and the application requires CPU access,
		it can use the IDXGIOutputDuplication::MapDesktopSurface and IDXGIOutputDuplication::UnMapDesktopSurface methods to avoid copying the data into a staging buffer.
		*/
	}
	catch(HRESULT){}

	SAFE_RELEASE(pDXGIOutput1);
	SAFE_RELEASE(pDXGIOutput);
	SAFE_RELEASE(pDXGIAdapter);
	SAFE_RELEASE(pDXGIDevice);

	return hr;
}

HRESULT CSimpleDesktopDuplication::ProcessDesktopDuplication(LPCWSTR wszScreenShotFile)
{
	HRESULT hr = S_OK;

	IF_FAILED_RETURN(m_pDXGIOutputDuplication == NULL ? E_UNEXPECTED : S_OK);

	IDXGIResource* pDXGIResource = NULL;
	ID3D11Texture2D* pScreenShotTexture2D = NULL;
	DXGI_OUTDUPL_FRAME_INFO FrameInfo;

	try
	{
		IF_FAILED_THROW(m_pDXGIOutputDuplication->AcquireNextFrame(500, &FrameInfo, &pDXGIResource));
		IF_FAILED_THROW(FrameInfo.TotalMetadataBufferSize > 0 ? S_OK : E_FAIL);
		IF_FAILED_THROW(pDXGIResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pScreenShotTexture2D)));
		IF_FAILED_THROW(CreateBmpFileFromScreenShot(pScreenShotTexture2D, wszScreenShotFile));
	}
	catch(HRESULT){}

	SAFE_RELEASE(pScreenShotTexture2D);
	SAFE_RELEASE(pDXGIResource);

	return hr;
}

void CSimpleDesktopDuplication::OnRelease()
{
	SAFE_RELEASE(m_pDXGIOutputDuplication);

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

HRESULT CSimpleDesktopDuplication::CreateBmpFileFromScreenShot(ID3D11Texture2D* pScreenShotTexture2D, LPCWSTR wszScreenShotFile)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* pOutTexture2D = NULL;
	D3D11_TEXTURE2D_DESC desc2D;
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	UINT uiSubResource = D3D11CalcSubresource(0, 0, 0);
	BYTE* pDataRgb = NULL;

	try
	{
		pScreenShotTexture2D->GetDesc(&desc2D);

		UINT uiImageSize = desc2D.Width * desc2D.Height * 3;

		pDataRgb = new (std::nothrow)BYTE[uiImageSize];
		IF_FAILED_THROW(pDataRgb == NULL ? E_OUTOFMEMORY : S_OK);

		desc2D.BindFlags = 0;
		desc2D.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc2D.Usage = D3D11_USAGE_STAGING;
		desc2D.MiscFlags = 0;

		IF_FAILED_THROW(m_pD3D11Device->CreateTexture2D(&desc2D, NULL, &pOutTexture2D));

		m_pD3D11DeviceContext->CopyResource(pOutTexture2D, pScreenShotTexture2D);

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

		IF_FAILED_THROW(CreateBmpFile(wszScreenShotFile, pDataRgb, uiImageSize, desc2D.Width, desc2D.Height));
	}
	catch(HRESULT){}

	SAFE_DELETE_ARRAY(pDataRgb);
	SAFE_RELEASE(pOutTexture2D);

	return hr;
}