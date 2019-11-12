//----------------------------------------------------------------------------------------------
// WindowTools.h
//----------------------------------------------------------------------------------------------
#ifndef WINDOWSTOOLS_H
#define WINDOWSTOOLS_H

HWND InitWindow(LPCWSTR, const LONG, const LONG);
void EndWindow(LPCWSTR, const HWND);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif