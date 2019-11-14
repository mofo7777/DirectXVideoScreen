//----------------------------------------------------------------------------------------------
// FormatConversionTools.h
//----------------------------------------------------------------------------------------------
#ifndef FORMATCONVERSIONTOOLS_H
#define FORMATCONVERSIONTOOLS_H

BYTE GetR(const int, int const);
BYTE GetG(const int, const int, const int);
BYTE GetB(const int, const int);

HRESULT ProcessNV12ToBmpFile(LPCWSTR, BYTE*, const INT, const UINT, const UINT);
HRESULT ProcessYUY2ToBmpFile(LPCWSTR, BYTE*, const INT, const UINT, const UINT);
HRESULT CreateBmpFile(LPCWSTR, BYTE*, const UINT, const UINT, const UINT);

#endif