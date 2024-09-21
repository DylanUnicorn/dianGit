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
#include <fcntl.h>  //文件控制定义
#include <unistd.h> //unix标准函数定义
#endif

/**
 * @brief 复制文件 from src to dest
 * @param src 源文件路径
 * @param dest 目标文件路径，注意文件名需要包含在路径中
 * @return int 0 if success, -1 if failed
*/
int copy_file(const char* src, const char* dst);

#endif // !_DIRPRO_H_
