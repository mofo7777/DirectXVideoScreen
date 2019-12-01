//----------------------------------------------------------------------------------------------
// WinMain.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	CDirectX9Window cDirectX9Window;

	if(!cDirectX9Window.Init(hInst))
		return -1;

	return cDirectX9Window.Run();
}