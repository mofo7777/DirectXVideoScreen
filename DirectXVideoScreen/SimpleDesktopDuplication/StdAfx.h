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

//----------------------------------------------------------------------------------------------
// Microsoft Windows SDK for Windows 7
#include <WinSDKVer.h>
#include <new>
#include <windows.h>
#include <assert.h>
#include <d3d11_1.h>

//----------------------------------------------------------------------------------------------
// Project Files
#include "MacroTools.h"
#include "FormatConversionTools.h"
#include "SimpleDesktopDuplication.h"

#endif