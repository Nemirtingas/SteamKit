#ifndef VERSION_H
#define VERSION_H

#if defined(NETHOOK2_OS_WINDOWS)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#elif defined(NETHOOK2_OS_LINUX)

#endif

extern const char *g_szBuildDate;
extern const char *g_szBuiltFromCommitSha;
extern const char *g_szBuiltFromCommitDate;
extern const bool g_bBuiltFromDirty;

#endif
