//-------------------------------------------------------------------------
// Quad.h
//-------------------------------------------------------------------------
#ifndef QUAD_H
#define QUAD_H

#define D3DFVF_QUADVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

struct QUADVERTEX
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR2 vUV;
};

class CQuad{

public:

	CQuad() : m_QuadVB(NULL){}
	virtual ~CQuad(){ OnDelete(); }

	void OnCreate()
	{
		m_QuadVertex[0].vPos = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
		m_QuadVertex[1].vPos = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
		m_QuadVertex[2].vPos = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
		m_QuadVertex[3].vPos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);

		m_QuadVertex[0].vUV.x = 0.0f; m_QuadVertex[0].vUV.y = 1.0f;
		m_QuadVertex[1].vUV.x = 0.0f; m_QuadVertex[1].vUV.y = 0.0f;
		m_QuadVertex[2].vUV.x = 1.0f; m_QuadVertex[2].vUV.y = 1.0f;
		m_QuadVertex[3].vUV.x = 1.0f; m_QuadVertex[3].vUV.y = 0.0f;
	}

	HRESULT OnRestore(IDirect3DDevice9* pDevice)
	{
		HRESULT hr;
		OnDelete();

		IF_FAILED_RETURN(pDevice == NULL ? E_POINTER : S_OK);

		IF_FAILED_RETURN(pDevice->CreateVertexBuffer(sizeof(m_QuadVertex), D3DUSAGE_WRITEONLY, D3DFVF_QUADVERTEX, D3DPOOL_DEFAULT, &m_QuadVB, NULL));

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
			IF_FAILED_RETURN(pDevice->SetStreamSource(0, m_QuadVB, 0, sizeof(QUADVERTEX)));
			IF_FAILED_RETURN(pDevice->SetFVF(D3DFVF_QUADVERTEX));
			IF_FAILED_RETURN(pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));
		}

		return hr;
	}

	void OnDelete(){ SAFE_RELEASE(m_QuadVB); }

private:

	QUADVERTEX m_QuadVertex[4];
	IDirect3DVertexBuffer9* m_QuadVB;
};

#endif