#ifndef _DIANGIT_H_
#define _DIANGIT_H_


#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#define ACCESS _access
#include<Windows.h>
#define IS_DIR(attrs) (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY))
#else
#include <unistd.h>
#define MKDIR(dir) mkdir(dir, 0755)
#define ACCESS access
#define IS_DIR(mode) (attrs != -1 && S_ISDIR(mode))
#endif
#include<header/hash_object.h>
#include<header/init.h>

#endif // !_DIANGIT_H_

