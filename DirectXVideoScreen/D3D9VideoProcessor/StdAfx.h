//----------------------------------------------------------------------------------------------
// StdAfx.h
//----------------------------------------------------------------------------------------------
#ifndef STDAFX_H
#define STDAFX_H

#pragma once
#define WIN32_LEAN_AND_MEAN
#define STRICT

//----------------------------------------------------------------------------------------------
// Pragma
#pragma comment(lib, "d3d9")
#pragma comment(lib, "dxva2")

//----------------------------------------------------------------------------------------------
// Microsoft DirectX SDK (June 2010)
#ifdef _WIN64
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x64\\d3dx9")
#else
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d3dx9")
#endif

//----------------------------------------------------------------------------------------------
// Microsoft Windows SDK for Windows 7
#include <WinSDKVer.h>
#include <new>
#include <windows.h>
#include <assert.h>
#include <initguid.h>
#include <d3d9.h>
#include <Evr9.h>
#include <dxva.h>
#include <dxvahd.h>

//----------------------------------------------------------------------------------------------
// Microsoft DirectX SDK (June 2010)
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\D3dx9tex.h"

//----------------------------------------------------------------------------------------------
// Project Files
#include "MacroTools.h"
#include "WindowTools.h"
#include "FormatConversionTools.h"
#include "D3D9VideoProcessor.h"

#endif