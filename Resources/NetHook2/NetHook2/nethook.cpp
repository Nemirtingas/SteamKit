
#include "os.h"

#if defined(NETHOOK2_OS_WINDOWS)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>

  #define STEAMCLIENT "steamclient.dll"
#elif defined(NETHOOK2_OS_LINUX)

  #define STEAMCLIENT "steamclient.so"

#endif

#include "logger.h"
#include "crypto.h"
#include "net.h"

#include "nh2_string.h"

#include "steammessages_base.pb.h"
#include "version.h"

CLogger *g_pLogger = NULL;
CCrypto* g_pCrypto = NULL;
NetHook::CNet *g_pNet = NULL;

bool g_bOwnsConsole = false;

void PrintVersionInfo()
{
	g_pLogger->LogConsole("Initializing NetHook2...\n");

	if (*g_szBuiltFromCommitSha == '\0')
	{
		g_pLogger->LogConsole("Built at %s. No further build information available.\n", g_szBuildDate);
	}
	else
	{
		g_pLogger->LogConsole("Built at %s from %s", g_szBuildDate, g_szBuiltFromCommitSha);

		if (g_bBuiltFromDirty)
		{
			g_pLogger->LogConsole("/dirty");
		}

		g_pLogger->LogConsole(" (%s)\n", g_szBuiltFromCommitDate);
	}
}

static void nethook_attach()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	g_pLogger = new CLogger();

    void* hFile = g_pLogger->OpenFile("attached.log", true);

    g_pLogger->LogOpenFile(hFile, "%s", "Attached to steam.\n");

    g_pLogger->CloseFile(hFile);

	PrintVersionInfo();

	g_pCrypto = new CCrypto();
	g_pNet = new NetHook::CNet();
}

static void nethook_detach()
{
    void* hFile = g_pLogger->OpenFile("detached.log", true);

    g_pLogger->LogOpenFile(hFile, "%s", "Detached from steam.\n");

    g_pLogger->CloseFile(hFile);

    delete g_pNet;
	delete g_pCrypto;

	delete g_pLogger;
}

#if defined(NETHOOK2_OS_WINDOWS)
bool IsRunDll32()
{
	char szMainModulePath[MAX_PATH];
	DWORD dwMainModulePathLength = GetModuleFileNameA(NULL, szMainModulePath, sizeof(szMainModulePath));

	return stringCaseInsensitiveEndsWith(szMainModulePath, "\\rundll32.exe");
}


BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
	if (IsRunDll32())
	{
		return TRUE;
	}

	if ( fdwReason == DLL_PROCESS_ATTACH )
	{
		g_bOwnsConsole = AllocConsole();

		LoadLibrary( STEAMCLIENT );

        nethook_attach();
	}
	else if ( fdwReason == DLL_PROCESS_DETACH )
	{
        nethook_detach();

		if (g_bOwnsConsole)
		{
			FreeConsole();
		}
	}

	return TRUE;
}

#elif defined(NETHOOK2_OS_LINUX)

static __attribute__((constructor)) void nethook_constructor()
{
    nethook_attach();
}

static __attribute__((destructor)) void nethook_destructor()
{
    nethook_detach();
}

#endif
