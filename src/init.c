#include <header/init.h>
#include <diangit.h>


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

// �����������ļ���д������
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


// ���Ŀ¼�Ƿ����
int directory_exists(const char* dir) {
    struct stat st;
    return stat(dir, &st) == 0 && IS_DIR(st.st_mode);
}

//����Ŀ¼��ȷ�����и�Ŀ¼������
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
        if (create_directory(path) != 0) {
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
