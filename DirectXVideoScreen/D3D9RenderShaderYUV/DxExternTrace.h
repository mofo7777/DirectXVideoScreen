//----------------------------------------------------------------------------------------------
// DxExternTrace.h
//----------------------------------------------------------------------------------------------
#ifndef DXEXTERNTRACE_H
#define DXEXTERNTRACE_H

#ifdef _DEBUG

class DebugLog{

public:

	static void Initialize(){ _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG); }

	static void Trace(const WCHAR *sFormatString, ...)
	{
		HRESULT hr = S_OK;
		va_list va;

		const DWORD TRACE_STRING_LEN = 512;

		WCHAR message[TRACE_STRING_LEN];

		va_start(va, sFormatString);
		hr = StringCchVPrintf(message, TRACE_STRING_LEN, sFormatString, va);
		va_end(va);

		if(SUCCEEDED(hr))
		{
			_CrtDbgReport(_CRT_WARN, NULL, NULL, NULL, "%S", message);
		}
	}

	static void Close()
	{
		int bLeak = _CrtDumpMemoryLeaks();
		assert(bLeak == FALSE);
	}
};

#define TRACE_INIT() DebugLog::Initialize()
#define TRACE(x) DebugLog::Trace x
#define TRACE_CLOSE() DebugLog::Close()

inline HRESULT _LOG_HRESULT(HRESULT hr, const char* sFileName, long lLineNo)
{
	if(FAILED(hr))
	{
		TRACE((L"%S - Line: %d hr=0x%X\n", sFileName, lLineNo, hr));
	}
	return hr;
}

#define LOG_HRESULT(hr) _LOG_HRESULT(hr, __FILE__, __LINE__)

class CExternTrace{

public:

	CExternTrace(){ TRACE_INIT(); };
	~CExternTrace(){ TRACE_CLOSE(); };
};

CExternTrace cExternTrace;

#endif

#endif