//----------------------------------------------------------------------------------------------
// WindowTools.cpp
//----------------------------------------------------------------------------------------------
#include "Stdafx.h"

HWND InitWindow(LPCWSTR wszClassName, const LONG lWidth, const LONG lHeight)
{
	HWND hWnd = NULL;
	WNDCLASSEX wcex = {0};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.lpszClassName = wszClassName;

	if(!RegisterClassEx(&wcex))
		return hWnd;

	RECT rc = {0, 0, lWidth, lHeight};
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindow(wszClassName, wszClassName, WS_OVERLAPPED, 0, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, wcex.hInstance, nullptr);

	GetClientRect(hWnd, &rc);
	assert(rc.right == lWidth && rc.bottom == lHeight);

	return hWnd;
}

void EndWindow(LPCWSTR wszClassName, const HWND hWnd)
{
	DestroyWindow(hWnd);
	UnregisterClass(wszClassName, GetModuleHandle(NULL));
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_DESTROY: PostQuitMessage(0); return 0L;
		default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}