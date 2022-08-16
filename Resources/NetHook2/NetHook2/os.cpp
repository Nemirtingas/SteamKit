#include "os.h"

#if defined(NETHOOK2_OS_WINDOWS)
#include <windows.h>

void* nethook2_getlibraryhandle(const char* library_name)
{
	return GetModuleHandleA( library_name );
}

void* nethook2_getprocaddress(void* hModule, const char* pName)
{
	return GetProcAddress((HMODULE)hModule, pName);
}

std::string get_exec_path()
{
	char tempName[ MAX_PATH ];
	GetModuleFileName( nullptr, tempName, MAX_PATH );
	return tempName;
}

void create_directory(std::string const& dir)
{
	CreateDirectoryA( dir.c_str(), nullptr );
}

void delete_file(std::string const& path)
{
	DeleteFileA( path.c_str() );
}

void move_file(std::string const& src, std::string const& dst)
{
	MoveFile( src.c_str(), dst.c_str() );
}


#elif defined(NETHOOK2_OS_LINUX)
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h> // to open directories
#include <dlfcn.h>

std::string expand_symlink(std::string file_path)
{
	struct stat file_stat;
	std::string link_target;
	ssize_t name_len = 128;
	while(lstat(file_path.c_str(), &file_stat) >= 0 && S_ISLNK(file_stat.st_mode) == 1)
	{
		do
		{
			name_len *= 2;
			link_target.resize(name_len);
			name_len = readlink(file_path.c_str(), &link_target[0], link_target.length());
		} while (name_len == link_target.length());
		link_target.resize(name_len);
		file_path = std::move(link_target);
	}

	return file_path;
}

void* nethook2_getlibraryhandle(const char* library_name)
{
	if (library_name == nullptr)
		return nullptr;

	std::string const self("/proc/self/map_files/");
	DIR* dir;
	struct dirent* dir_entry;
	void* res = nullptr;
	size_t library_name_len = strlen(library_name);

	dir = opendir(self.c_str());
	if (dir != nullptr)
	{
		std::string file_path;
		while ((dir_entry = readdir(dir)) != nullptr)
		{
			file_path = (self + dir_entry->d_name);
			if (dir_entry->d_type != DT_LNK)
			{// Not a link
				continue;
			}

			file_path = expand_symlink(file_path);

			auto pos = file_path.rfind('/');
			if (pos != std::string::npos)
			{
				++pos;
				if (strncmp(file_path.c_str() + pos, library_name, library_name_len) == 0)
				{
					res = dlopen(file_path.c_str(), RTLD_NOW);
					if (res != nullptr)
					{// Like Windows' GetModuleHandle, we don't want to increment the ref counter
						dlclose(res);
					}
					break;
				}
			}
		}

		closedir(dir);
	}

	return res;
}

void* nethook2_getprocaddress(void* hModule, const char* pName)
{
	return dlsym(hModule, pName);
}

std::string get_exec_path()
{
	return expand_symlink("/proc/self/exe");
}

void create_directory(std::string const& directory)
{
	mkdir(directory.c_str(), 0755);
}

void delete_file(std::string const& path)
{
    unlink(path.c_str());
}

void move_file(std::string const& src, std::string const& dst)
{
    rename(src.c_str(), dst.c_str());
}


#endif
