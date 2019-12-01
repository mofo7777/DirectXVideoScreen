//-------------------------------------------------------------------------
// YUVQuad.h
//-------------------------------------------------------------------------
#ifndef YUVQUAD_H
#define YUVQUAD_H

#define D3DFVF_YUVQUADVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEX2 | D3DFVF_TEX3 | D3DFVF_TEX4 | D3DFVF_TEX5 | D3DFVF_TEX6)

struct YUVQUADVERTEX
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR2 vY;
	D3DXVECTOR2 vU;
	D3DXVECTOR2 vV;
	D3DXVECTOR2 vUV;
	D3DXVECTOR2 vU2;
	D3DXVECTOR2 vV2;
};

class CYUVQuad{

public:

	CYUVQuad() : m_QuadVB(NULL){}
	virtual ~CYUVQuad(){ OnDelete(); }

	void OnCreate()
	{
		m_QuadVertex[0].vPos = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
		m_QuadVertex[1].vPos = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
		m_QuadVertex[2].vPos = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
		m_QuadVertex[3].vPos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);

		const float fY = 2.0f / 3.0f;
		const float fUV = fY + ((1.0f / 3.0f) / 2.0f);

		m_QuadVertex[0].vY.x = 0.0f; m_QuadVertex[0].vY.y = fY;
		m_QuadVertex[1].vY.x = 0.0f; m_QuadVertex[1].vY.y = 0.0f;
		m_QuadVertex[2].vY.x = 1.0f; m_QuadVertex[2].vY.y = fY;
		m_QuadVertex[3].vY.x = 1.0f; m_QuadVertex[3].vY.y = 0.0f;

		m_QuadVertex[0].vU.x = 0.0f; m_QuadVertex[0].vU.y = fUV;
		m_QuadVertex[1].vU.x = 0.0f; m_QuadVertex[1].vU.y = fY;
		m_QuadVertex[2].vU.x = 0.5f; m_QuadVertex[2].vU.y = fUV;
		m_QuadVertex[3].vU.x = 0.5f; m_QuadVertex[3].vU.y = fY;

		m_QuadVertex[0].vV.x = 0.0f; m_QuadVertex[0].vV.y = 1.0f;
		m_QuadVertex[1].vV.x = 0.0f; m_QuadVertex[1].vV.y = fUV;
		m_QuadVertex[2].vV.x = 0.5f; m_QuadVertex[2].vV.y = 1.0f;
		m_QuadVertex[3].vV.x = 0.5f; m_QuadVertex[3].vV.y = fUV;

		m_QuadVertex[0].vUV.x = 0.0f; m_QuadVertex[0].vUV.y = 1.0f;
		m_QuadVertex[1].vUV.x = 0.0f; m_QuadVertex[1].vUV.y = fY;
		m_QuadVertex[2].vUV.x = 1.0f; m_QuadVertex[2].vUV.y = 1.0f;
		m_QuadVertex[3].vUV.x = 1.0f; m_QuadVertex[3].vUV.y = fY;

		m_QuadVertex[0].vU2.x = 0.5f; m_QuadVertex[0].vU2.y = fUV;
		m_QuadVertex[1].vU2.x = 0.5f; m_QuadVertex[1].vU2.y = fY;
		m_QuadVertex[2].vU2.x = 1.0f; m_QuadVertex[2].vU2.y = fUV;
		m_QuadVertex[3].vU2.x = 1.0f; m_QuadVertex[3].vU2.y = fY;

		m_QuadVertex[0].vV2.x = 0.5f; m_QuadVertex[0].vV2.y = 1.0f;
		m_QuadVertex[1].vV2.x = 0.5f; m_QuadVertex[1].vV2.y = fUV;
		m_QuadVertex[2].vV2.x = 1.0f; m_QuadVertex[2].vV2.y = 1.0f;
		m_QuadVertex[3].vV2.x = 1.0f; m_QuadVertex[3].vV2.y = fUV;
	}

	HRESULT OnRestore(IDirect3DDevice9* pDevice)
	{
		HRESULT hr;
		OnDelete();

		IF_FAILED_RETURN(pDevice == NULL ? E_POINTER : S_OK);

		IF_FAILED_RETURN(pDevice->CreateVertexBuffer(sizeof(m_QuadVertex), D3DUSAGE_WRITEONLY, D3DFVF_YUVQUADVERTEX, D3DPOOL_DEFAULT, &m_QuadVB, NULL));

		void* pData;

		IF_FAILED_RETURN(m_QuadVB->Lock(0, sizeof(pData), &pData, 0));
		memcpy(pData, m_QuadVertex, sizeof(m_QuadVertex));
		IF_FAILED_RETURN(m_QuadVB->Unlock());

		return hr;
	}

	HRESULT OnRender(IDirect3DDevice9* pDevice)
	{
		HRESULT hr;

		IF_FAILED_RETURN(pDevice == NULL ? E_POINTER : S_OK);

		if(m_QuadVB)
		{
			IF_FAILED_RETURN(pDevice->SetStreamSource(0, m_QuadVB, 0, sizeof(YUVQUADVERTEX)));
			IF_FAILED_RETURN(pDevice->SetFVF(D3DFVF_YUVQUADVERTEX));
			IF_FAILED_RETURN(pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));
		}

		return hr;
	}

	void OnDelete(){ SAFE_RELEASE(m_QuadVB); }

private:

	YUVQUADVERTEX m_QuadVertex[4];
	IDirect3DVertexBuffer9* m_QuadVB;
};

#endif