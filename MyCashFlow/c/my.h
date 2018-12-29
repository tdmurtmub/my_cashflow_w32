#ifndef MODULE_MY
#define MODULE_MY

#define MY_DOMAIN _T("www.wesleysteiner.com")
#define MY_DOMAIN_URL _T("http://") MY_DOMAIN 

#ifdef __cplusplus

	#ifdef DOMAIN
	#undef DOMAIN
	#endif

	#ifdef DOMAIN_URL
	#undef DOMAIN_URL
	#endif

	namespace my 
		{ 
		static TCHAR const * DOMAIN = MY_DOMAIN;
		static TCHAR const * DOMAIN_URL = MY_DOMAIN_URL;
		static TCHAR const * HOMEPAGE = MY_DOMAIN_URL _T("/index.html");
		namespace win { namespace app { static const TCHAR * REGISTRY_KEY = _T("Wesley Steiner"); }}
		}

#endif

#endif

