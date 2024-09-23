#ifndef _DIRPRO_H_
#define _DIRPRO_H_

#ifdef _WIN32
#include <windows.h>

#define DT_DIR 4
#define DT_REG 8

typedef struct {
	HANDLE hFind;
	WIN32_FIND_DATA findFileData;
	int first;
}DIR;
struct dirent {
	char d_name[MAX_PATH];
	unsigned char d_type;
};
DIR* opendir(const char* name);
struct dirent* readdir(DIR* dirp);
int closedir(DIR* dirp);
#else
#include <dirent.h>
#include <fcntl.h>  //�ļ����ƶ���
#include <unistd.h> //unix��׼��������
#include <fnmatch.h> //�ļ���ƥ��
#endif

/**
 * @brief �����ļ� from src to dest
 * @param src Դ�ļ�·��
 * @param dest Ŀ���ļ�·����ע���ļ�����Ҫ������·����
 * @return int 0 if success, -1 if failed
*/
int copy_file(const char* src, const char* dst);

// �ж��ļ��Ƿ����
int file_exists(const char* path);

#endif // !_DIRPRO_H_

#ifdef _WIN32
#define FNM_PATHNAME 0x01
#define FNM_NOMATCH 1
int fnmatch(const char* pattern, const char* string, int flags);
#else
#endif