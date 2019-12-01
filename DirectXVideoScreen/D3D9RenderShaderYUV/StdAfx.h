//----------------------------------------------------------------------------------------------
// StdAfx.h
//----------------------------------------------------------------------------------------------
#ifndef STDAFX_H
#define STDAFX_H

#pragma once
#define STRICT

//----------------------------------------------------------------------------------------------
// Microsoft DirectX SDK (June 2010)
#pragma comment(lib, "d3d9")
#pragma comment(lib, "Gdiplus")

#ifdef _WIN64
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x64\\d3dx9")
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x64\\DxErr")
#else
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d3dx9")
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\DxErr")
#endif

//----------------------------------------------------------------------------------------------
// Microsoft Windows SDK for Windows 7
#include <WinSDKVer.h>
#include <new>
#include <windows.h>

//----------------------------------------------------------------------------------------------
// STL
#include <string>
using std::wstring;

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#include <crtdbg.h>
#include <strsafe.h>
#include <assert.h>
#endif

//----------------------------------------------------------------------------------------------
// GdiPlus
#pragma warning(push)
#pragma warning(disable:4458)
#include <gdiplus.h>
#pragma warning(pop)
using namespace Gdiplus;

//----------------------------------------------------------------------------------------------
// Microsoft DirectX SDK (June 2010)
#include <d3d9.h>
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h"
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\dxerr.h"

//----------------------------------------------------------------------------------------------
// Project Files
#include "Definition.h"
#include "DxExternTrace.h"
#include "MacroTools.h"
#include "resource.h"
#include "Quad.h"
#include "YUVQuad.h"
#include "DirectX9Manager.h"
#include "DirectX9Window.h"

#endif