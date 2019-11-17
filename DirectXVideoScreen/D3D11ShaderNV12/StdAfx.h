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
#pragma comment(lib, "D3D11")
#pragma comment(lib, "D3DCompiler")

//----------------------------------------------------------------------------------------------
// Microsoft Windows SDK for Windows 7
#include <WinSDKVer.h>
#include <new>
#include <windows.h>
#include <assert.h>
#include <wincodec.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>

//----------------------------------------------------------------------------------------------
// Project Files
#include "MacroTools.h"
#include "FormatConversionTools.h"
#include "WICBitmap.h"
#include "D3D11ShaderNV12.h"

#endif