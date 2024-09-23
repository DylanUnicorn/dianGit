#ifndef _DIANGIT_H_
#define _DIANGIT_H_


#ifdef _WIN32
	#include <direct.h>
	#define MKDIR(dir) _mkdir(dir)
	#define ACCESS _access
	#include <Windows.h>
	#include <io.h>
	#include <tchar.h>
	#define IS_DIR(attrs) (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY))
	#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
	#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
	#define IS_REG(attrs) (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0)
#else
	#include <unistd.h>
	#include <sys/stat.h>
	#include <direct.h>
	#define MKDIR(dir) mkdir(dir, 0755)
	#define ACCESS access
	#define IS_DIR(mode) (attrs != -1 && S_ISDIR(mode))
#endif


#ifdef _WIN32
	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/stat.h>
	#include <errno.h>
	#include <string.h>
	// #include <dirent.h>  //实际上Windows也有这东西，不过我们已经实现了就不管他了
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/stat.h>
	#include <errno.h>
	#include <string.h>
#endif

#endif // !_DIANGIT_H_

