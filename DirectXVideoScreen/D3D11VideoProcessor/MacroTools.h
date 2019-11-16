//----------------------------------------------------------------------------------------------
// MacroTools.h
//----------------------------------------------------------------------------------------------
#ifndef MACROTOOLS_H
#define MACROTOOLS_H

#ifndef MACRO_SAFE_RELEASE
#define MACRO_SAFE_RELEASE
template <class T> inline void SAFE_RELEASE(T*& p){

	if(p){
		p->Release();
		p = NULL;
	}
}
#endif

#ifndef MACRO_SAFE_DELETE
#define MACRO_SAFE_DELETE
template<class T> inline void SAFE_DELETE(T*& p){

	if(p){
		delete p;
		p = NULL;
	}
}
#endif

#ifndef MACRO_SAFE_DELETE_ARRAY
#define MACRO_SAFE_DELETE_ARRAY
template<class T> inline void SAFE_DELETE_ARRAY(T*& p){

	if(p){
		delete[] p;
		p = NULL;
	}
}
#endif

#ifndef MACRO_FAILED_RETURN
#define MACRO_FAILED_RETURN
#define IF_FAILED_RETURN(X) if(FAILED(hr = (X))){ return hr; }
#endif

#ifndef MACRO_FAILED_THROW
#define MACRO_FAILED_THROW
#define IF_FAILED_THROW(X) if(FAILED(hr = (X))){ throw hr; }
#endif

#endif
