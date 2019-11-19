//----------------------------------------------------------------------------------------------
// Main.cpp
//----------------------------------------------------------------------------------------------
#include "StdAfx.h"

#define SCREENSHOT L"ScreenShot.bmp"

void main()
{
	CSimpleDesktopDuplication cSimpleDesktopDuplication;

	if(cSimpleDesktopDuplication.InitDesktopDuplication() == S_OK)
	{
		cSimpleDesktopDuplication.ProcessDesktopDuplication(SCREENSHOT);
	}

	cSimpleDesktopDuplication.OnRelease();
}