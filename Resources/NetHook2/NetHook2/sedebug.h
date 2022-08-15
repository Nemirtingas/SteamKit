
#include "os.h"

#if defined(NETHOOK2_OS_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

HANDLE SeDebugAcquire();
#elif defined(NETHOOK2_OS_LINUX)
#endif
