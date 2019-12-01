//----------------------------------------------------------------------------------------------
// MacroTools.h
//----------------------------------------------------------------------------------------------
#ifndef MACROTOOLS_H
#define MACROTOOLS_H

template <class T> inline void SAFE_RELEASE(T*& p)
{
	if(p)
	{
		p->Release();
		p = NULL;
	}
}

template<class T> inline void SAFE_DELETE(T*& p)
{
	if(p)
	{
		delete p;
		p = NULL;
	}
}

template<class T> inline void SAFE_DELETE_ARRAY(T*& p)
{
	if(p)
	{
		delete[] p;
		p = NULL;
	}
}

#define IF_FAILED_RETURN(X) if(FAILED(hr = (X))){ return hr; }

#define IF_FAILED_THROW(X) if(FAILED(hr = (X))){ throw hr; }

#endif
