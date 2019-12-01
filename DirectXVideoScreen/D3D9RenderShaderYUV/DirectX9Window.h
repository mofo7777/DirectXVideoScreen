//----------------------------------------------------------------------------------------------
// DirectX9Window.h
//----------------------------------------------------------------------------------------------
#ifndef DIRECTX9WINDOW_H
#define DIRECTX9WINDOW_H

class CDirectX9Window{

public:

	CDirectX9Window();
	~CDirectX9Window();

	bool Init(HINSTANCE);
	int  Run();

	static CDirectX9Window* GetCDirectX9Window(){ return m_pCDirectX9Window; }

	LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

private:

	static CDirectX9Window* m_pCDirectX9Window;

	CDirectX9Manager m_cDirectX9Manager;

	HINSTANCE m_hInst;
	HWND      m_hWnd;

	int m_iTechnique;
	wstring m_wszTechnique;

	BOOL OnCommand(const DWORD);
	void SetTechnique(const int);
	LPCWSTR GetNameTechnique(const int);
};

inline void CDirectX9Window::SetTechnique(const int iTechnique)
{
	m_iTechnique = iTechnique;

	m_cDirectX9Manager.SetTechnique(m_iTechnique);
	SetWindowText(m_hWnd, GetNameTechnique(m_iTechnique));
}

inline LPCWSTR CDirectX9Window::GetNameTechnique(const int iTechnique)
{
	switch(iTechnique)
	{
		case SHOWRGB: m_wszTechnique = L"ShowRGB"; break;
		case SHOWLUMA: m_wszTechnique = L"ShowLuma"; break;
		case SHOWCHROMAB: m_wszTechnique = L"ShowChromaB"; break;
		case SHOWCHROMAR: m_wszTechnique = L"ShowChromaR"; break;
		case SHOWYUV: m_wszTechnique = L"ShowYUV"; break;
		case SHOWNV12: m_wszTechnique = L"ShowNV12"; break;
		case SHOWWIDTHSHIFT: m_wszTechnique = L"ShowWidthShift"; break;
		case SHOWHEIGHTSHIFT: m_wszTechnique = L"ShowHeightShift"; break;
		case SHOWY: m_wszTechnique = L"ShowY"; break;
		case SHOWU: m_wszTechnique = L"ShowU"; break;
		case SHOWV: m_wszTechnique = L"ShowV"; break;
		case SHOWDECODEDYUV444: m_wszTechnique = L"ShowDecodedYUV444YUV"; break;
		case SHOWDECODEDYUV420: m_wszTechnique = L"ShowDecodedYUV420YUV"; break;
		case SHOWDECODEDYUV: m_wszTechnique = L"ShowDecodedYUV"; break;
		case SHOWDECODEDNV12: m_wszTechnique = L"ShowDecodedNV12"; break;
		default: m_wszTechnique = L"Technique unknown"; break;
	}

	return m_wszTechnique.c_str();
}

#endif