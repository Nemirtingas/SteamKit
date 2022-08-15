
#include "os.h"

#include "csimplescan.h"
#include <assert.h>
#include <cstring>

#if defined(NETHOOK2_OS_LINUX)
#include <dlfcn.h>
#endif

#define CREATEINTERFACE_PROCNAME	"CreateInterface"

// load/unload components
//class CSysModule;

//-----------------------------------------------------------------------------
// Purpose: returns a pointer to a function, given a module
// Input  : pModuleName - module name
//			*pName - proc name
//-----------------------------------------------------------------------------
static void *Sys_GetProcAddress( const char *pModuleName, const char *pName ) noexcept
{
#if defined(NETHOOK2_OS_WINDOWS)
	HMODULE hModule = GetModuleHandle( pModuleName );
	assert(hModule != nullptr);
	return GetProcAddress( hModule, pName );
#elif defined(NETHOOK2_OS_LINUX)
    void* hModule = dlopen(pModuleName, RTLD_NOW);
    assert(hModule != nullptr);
    void* sym = dlsym(hModule, pName);
    dlclose(hModule);
    return sym;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: returns the instance of the named module
// Input  : *pModuleName - name of the module
// Output : interface_instance_t - instance of that module
//-----------------------------------------------------------------------------
CreateInterfaceFn Sys_GetFactory( const char *pModuleName ) noexcept
{
#if defined(NETHOOK2_OS_WINDOWS)
	return static_cast<CreateInterfaceFn>( Sys_GetProcAddress( pModuleName, CREATEINTERFACE_PROCNAME ) );
#elif defined(NETHOOK2_OS_LINUX)
	// see Sys_GetFactory( CSysModule *pModule ) for an explanation
	return (CreateInterfaceFn)( Sys_GetProcAddress( pModuleName, CREATEINTERFACE_PROCNAME ) );
#endif
}


CSimpleScan::CSimpleScan() noexcept
{
	m_bInterfaceSet = false;
	m_Interface = nullptr;
}

CSimpleScan::CSimpleScan( const char *filename ) noexcept
{
	SetDLL( filename );
}

bool CSimpleScan::SetDLL( const char *filename ) noexcept
{
	m_Interface = Sys_GetFactory( filename );

	CSigScan::sigscan_dllfunc = m_Interface;

	if (!CSigScan::GetDllMemInfo())
	{
		m_bInterfaceSet = false;
	}
	else
	{
		m_bInterfaceSet = (m_Interface != nullptr);
	}

	return m_bInterfaceSet;
}

bool CSimpleScan::FindFunction( const char * sig, const char *mask, void **func ) noexcept
{
	if ( !m_bInterfaceSet )
		return false;

	
	m_Signature.Init( ( unsigned char * )sig, ( char * )mask, strlen( mask ) );

	if ( !m_Signature.is_set )
		return false;

	*func = m_Signature.sig_addr;

	return true;
}
