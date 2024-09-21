#include <header/init.h>
#include <diangit.h>


// 创建文件并写入内容
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

// 创建二进制文件并写入内容
int create_binary_file(const char* path, const void* data, size_t len) {
    FILE* file = fopen(path, "wb");
    if (!file) {
        perror("Failed to create file");
        return -1;
    }

    if (data) {
        size_t written = fwrite(data, 1, len, file);
        if (written != len) {
            perror("Failed to write complete data to file");
            fclose(file);
            return -1;
        }
    }

    if (fclose(file) != 0) {
        perror("Failed to close file");
        return -1;
    }

    return 0;
}


// 检查目录是否存在
int directory_exists(const char* dir) {
    struct stat st;
    return stat(dir, &st) == 0 && IS_DIR(st.st_mode);
}

//创建目录并确保所有父目录都存在
int create_directory(const char* dir) {
    char path[256] = { 0 };
	char* p = path;
	const char* q = dir;

	while (*q) {
		if (*q == '/' || *q == '\\') {
			*p = '\0';
			if (MKDIR(path) != 0) {
				if (errno != EEXIST) {
					perror("Failed to create directory");
					return -1;
				}
			}
		}
		*p++ = *q++;
	}

	if (MKDIR(path) != 0) {
		if (errno != EEXIST) {
			perror("Failed to create directory");
			return -1;
		}
	}

	return 0;
}

// 初始化 Git 仓库
void init_repository(const char* dir) {
    char path[256];

    // 检查目录是否存在
    if (directory_exists(dir)) {
        // 检查是否已经存在 .git 目录
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
        // 尝试创建顶级目录
        if (MKDIR(dir) != 0) {
            perror("Failed to create directory");
            return;
        }
        printf("Initialized empty Git repository in %s/.git/\n", dir);
    }

    // 创建 .git 目录
    snprintf(path, sizeof(path), "%s/.git", dir);
    if (MKDIR(path) != 0) {
        perror("Failed to create .git directory");
        return;
    }

    // 创建 hooks, info, objects/info, objects/pack, refs/heads, refs/tags
    const char* subdirs[] = { "hooks", "info", "objects/info", "objects/pack", "refs/heads", "refs/tags" };
    for (int i = 0; i < 6; i++) {
        snprintf(path, sizeof(path), "%s/.git/%s", dir, subdirs[i]);
        if (create_directory(path) != 0) {
            perror("Failed to create subdirectory");
            return;
        }
    }

    // 创建文件: HEAD, config, description, info/exclude
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
