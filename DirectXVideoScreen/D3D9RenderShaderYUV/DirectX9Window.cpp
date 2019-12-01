//----------------------------------------------------------------------------------------------
// DirectX9Window.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

CDirectX9Window* CDirectX9Window::m_pCDirectX9Window = NULL;

LRESULT CALLBACK WndMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return CDirectX9Window::GetCDirectX9Window()->WindowProc(hWnd, msg, wParam, lParam);
}

CDirectX9Window::CDirectX9Window() : m_hInst(NULL), m_hWnd(NULL), m_iTechnique(SHOWRGB)
{
	m_pCDirectX9Window = this;
}

CDirectX9Window::~CDirectX9Window()
{
	if(IsWindow(m_hWnd))
		DestroyWindow(m_hWnd);

	UnregisterClass(DIRECTX9_CLASS, m_hInst);
}

bool CDirectX9Window::Init(HINSTANCE hInst)
{
	m_hInst = hInst;

	WNDCLASSEX WndClassEx;

	WndClassEx.cbSize = sizeof(WNDCLASSEX);
	WndClassEx.style = CS_HREDRAW | CS_VREDRAW;
	WndClassEx.lpfnWndProc = WndMsgProc;
	WndClassEx.cbClsExtra = 0L;
	WndClassEx.cbWndExtra = 0L;
	WndClassEx.hInstance = hInst;
	WndClassEx.hIcon = NULL;
	WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClassEx.hbrBackground = NULL;
	WndClassEx.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClassEx.lpszClassName = DIRECTX9_CLASS;
	WndClassEx.hIconSm = NULL;

	if(!RegisterClassEx(&WndClassEx))
	{
		return false;
	}

	RECT rc = {0, 0, 640, 480};
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	int iXWnd = (int)((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2.0f);
	int iYWnd = (int)((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2.0f);

	if((m_hWnd = CreateWindow(DIRECTX9_CLASS, DIRECTX9_CLASS, WS_OVERLAPPEDWINDOW, iXWnd, iYWnd, rc.right - rc.left, rc.bottom - rc.top, GetDesktopWindow(), NULL, m_hInst, NULL)) == NULL)
	{
		return false;
	}

	return true;
}

int CDirectX9Window::Run()
{
	HRESULT hr;

	if(FAILED(hr = m_cDirectX9Manager.InitDirectX(m_hWnd, m_hInst)))
	{
		m_cDirectX9Manager.ReleaseDirectX();
		return -1;
	}

	RECT rc = {0, 0, (LONG)m_cDirectX9Manager.GetImageWidth(), (LONG)m_cDirectX9Manager.GetImageHeight()};
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);

	int iXWnd = (int)((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2.0f);
	int iYWnd = (int)((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2.0f);

	SetWindowPos(m_hWnd, NULL, iXWnd, iYWnd, rc.right - rc.left, rc.bottom - rc.top, 0);

	SetWindowText(m_hWnd, GetNameTechnique(m_iTechnique));

	ShowWindow(m_hWnd, SW_NORMAL);
	UpdateWindow(m_hWnd);

	//GetClientRect(m_hWnd, &rc);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_cDirectX9Manager.RenderDirectX();
			Sleep(80);
		}
	}

	m_cDirectX9Manager.ReleaseDirectX();

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK CDirectX9Window::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_KEYDOWN:

			switch(wParam)
			{
				case VK_SPACE:
				{
					if(++m_iTechnique == NOMORETECHNIQUE)
						m_iTechnique = SHOWRGB;

					m_cDirectX9Manager.SetTechnique(m_iTechnique);
					SetWindowText(hWnd, GetNameTechnique(m_iTechnique));
				}
				break;

				case VK_ESCAPE:
					PostQuitMessage(0);
					break;

				default:
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}
			break;

		case WM_COMMAND:

			if(OnCommand(LOWORD(wParam)))
			{
				return DefWindowProc(hWnd, msg, wParam, lParam);
			}
			break;

		case WM_DESTROY:
		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		case WM_QUERYENDSESSION:
			return DefWindowProc(hWnd, msg, TRUE, lParam);

		default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0L;
}

BOOL CDirectX9Window::OnCommand(const DWORD dwCmd)
{
	BOOL bNoCmd = FALSE;

	switch(dwCmd)
	{
		case ID_PROGRAM_EXIT:
			PostQuitMessage(0);
			break;

		case ID_SHADER_SHOWRGB: SetTechnique(SHOWRGB); break;
		case ID_SHADER_SHOWLUMA: SetTechnique(SHOWLUMA); break;
		case ID_SHADER_SHOWCHROMAB: SetTechnique(SHOWCHROMAB); break;
		case ID_SHADER_SHOWCHROMAR: SetTechnique(SHOWCHROMAR); break;
		case ID_SHADER_SHOWYUV: SetTechnique(SHOWYUV); break;
		case ID_SHADER_SHOWNV12: SetTechnique(SHOWNV12); break;
		case ID_SHADER_SHOWSHIFTW: SetTechnique(SHOWWIDTHSHIFT); break;
		case ID_SHADER_SHOWSHIFTH: SetTechnique(SHOWHEIGHTSHIFT); break;
		case ID_SHADER_SHOWY: SetTechnique(SHOWY); break;
		case ID_SHADER_SHOWU: SetTechnique(SHOWU); break;
		case ID_SHADER_SHOWV: SetTechnique(SHOWV); break;
		case ID_SHADER_SHOWYUV444YUV: SetTechnique(SHOWDECODEDYUV444); break;
		case ID_SHADER_SHOWYUV420YUV: SetTechnique(SHOWDECODEDYUV420); break;
		case ID_SHADER_SHOWDECODEDYUV: SetTechnique(SHOWDECODEDYUV); break;
		case ID_SHADER_SHOWDECODEDNV12: SetTechnique(SHOWDECODEDNV12); break;

		default:
			bNoCmd = TRUE;
			break;
	}

	return bNoCmd;
}