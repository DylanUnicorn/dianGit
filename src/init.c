#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "init.h"

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

// �����ļ���д������
void create_file(const char* path, const char* content) {
    FILE* file = fopen(path, "w");
    if (file) {
        if (content) {
            fputs(content, file);
        }
        fclose(file);
    }
    else {
        perror("Failed to create file");
    }
}

// ���Ŀ¼�Ƿ����
int directory_exists(const char* dir) {
    struct stat st;
    return stat(dir, &st) == 0 && IS_DIR(st.st_mode);
}

// ��ʼ�� Git �ֿ�
void init_repository(const char* dir) {
    char path[256];

    // ���Ŀ¼�Ƿ����
    if (directory_exists(dir)) {
        // ����Ƿ��Ѿ����� .git Ŀ¼
        snprintf(path, sizeof(path), "%s/.git", dir);
        if (directory_exists(path)) {
            printf("Reinitialized existing Git repository in %s/.git/\n", dir);
            return;
        }
        else {
            printf("Directory %s already exists, initializing it as a Git repository...\n", dir);
        }
    }
    else {
        // ���Դ�������Ŀ¼
        if (MKDIR(dir) != 0) {
            perror("Failed to create directory");
            return;
        }
        printf("Initialized empty Git repository in %s/.git/\n", dir);
    }

    // ���� .git Ŀ¼
    snprintf(path, sizeof(path), "%s/.git", dir);
    if (MKDIR(path) != 0) {
        perror("Failed to create .git directory");
        return;
    }

    // ���� hooks, info, objects/info, objects/pack, refs/heads, refs/tags
    const char* subdirs[] = { "hooks", "info", "objects/info", "objects/pack", "refs/heads", "refs/tags" };
    for (int i = 0; i < 6; i++) {
        snprintf(path, sizeof(path), "%s/.git/%s", dir, subdirs[i]);
        if (MKDIR(path) != 0) {
            perror("Failed to create subdirectory");
            return;
        }
    }

    // �����ļ�: HEAD, config, description, info/exclude
    snprintf(path, sizeof(path), "%s/.git/HEAD", dir);
    create_file(path, "ref: refs/heads/master\n");

    snprintf(path, sizeof(path), "%s/.git/config", dir);
    create_file(path,
        "[core]\n"
        "\trepositoryformatversion = 0\n"
        "\tfilemode = true\n"
        "\tbare = false\n"
        "\tlogallrefupdates = true\n");

    snprintf(path, sizeof(path), "%s/.git/description", dir);
    create_file(path, "Unnamed repository; edit this file to name it for gitweb.\n");

    snprintf(path, sizeof(path), "%s/.git/info/exclude", dir);
    create_file(path, "# gitignore patterns\n");

    printf("Git repository initialized successfully in %s/.git/\n", dir);
}
