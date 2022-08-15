#include "os.h"

#if defined(NETHOOK2_OS_WINDOWS)
    #define STRICMP(src, needle) _stricmp(src, needle)
#elif defined(NETHOOK2_OS_LINUX)
    #define STRICMP(src, needle) strcasecmp(src, needle)
#endif

bool stringCaseInsensitiveEndsWith(const char * szHaystack, const char * szNeedle)
{
	int iHaystackLen = strlen(szHaystack);
	int iNeedleLen = strlen(szNeedle);

	if (iHaystackLen < iNeedleLen)
	{
		return false;
	}

	const char * szHaystackFromNeedleStartPosition = szHaystack + iHaystackLen - iNeedleLen;

	return STRICMP(szHaystackFromNeedleStartPosition, szNeedle) == 0;
}
