#ifndef _DIRPRO_H_
#define _DIRPRO_H_

#ifdef _WIN32
#include <windows.h>
typedef struct {
	HANDLE hFind;
	WIN32_FIND_DATA findFileData;
	int first;
}DIR;
struct dirent {
	char d_name[MAX_PATH];
};
DIR* opendir(const char* name);
struct dirent* readdir(DIR* dirp);
int closedir(DIR* dirp);
#else
#include <dirent.h>
#endif

#endif // !_DIRPRO_H_
