#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#include <string>

#if defined(WIN64) || defined(_WIN64) || defined(__MINGW64__) || defined(WIN32) || defined(_WIN32) || defined(__MINGW32__)
  #define NETHOOK2_OS_WINDOWS
  #define NETHOOK_FASTCALL __fastcall

  #define STEAMCLIENT "steamclient.dll"
  #define PATH_SEPARATOR '\\'

#elif defined(__linux__) || defined(linux)
  #define NETHOOK2_OS_LINUX
  #define NETHOOK_FASTCALL

  #define STEAMCLIENT "steamclient.so"
  #define PATH_SEPARATOR '/'

  std::string expand_symlink(std::string file_path);

#else
  #error "Unknown OS"
#endif

#endif

void* nethook2_getlibraryhandle(const char* library_name);
void* nethook2_getprocaddress(void* hModule, const char* pName);
std::string get_exec_path();
void create_directory(std::string const& directory);
void delete_file(std::string const& path);
void move_file(std::string const& src, std::string const& dst);

#include <string.h>
#include <stdio.h>
