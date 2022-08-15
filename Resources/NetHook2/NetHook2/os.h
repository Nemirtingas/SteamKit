#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED


#if defined(WIN64) || defined(_WIN64) || defined(__MINGW64__) || defined(WIN32) || defined(_WIN32) || defined(__MINGW32__)
  #define NETHOOK2_OS_WINDOWS
  #define NETHOOK_FASTCALL __fastcall


#elif defined(__linux__) || defined(linux)
  #define NETHOOK2_OS_LINUX
  #define NETHOOK_FASTCALL

#else
  #error "Unknown OS"
#endif

#endif

#include <string.h>
#include <stdio.h>
