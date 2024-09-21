#include <header/TagAndBranch.h>
#include <diangit.h>
#include <header/dirPro.h>

// 辅助函数：创建分支或标签
void create_ref(const char* ref_type, const char* ref_name, const char* commit_hash) {
    char ref_path[256];
    snprintf(ref_path, sizeof(ref_path), ".git/refs/%s/%s", ref_type, ref_name);

    // 检查是否已经存在该分支或标签
    FILE* ref_file = fopen(ref_path, "r");
    if (ref_file != NULL) {
        printf("%s %s already exists.\n", ref_type, ref_name);
        fclose(ref_file);
        return;
    }

    // 创建文件并写入提交哈希值
    ref_file = fopen(ref_path, "w");
    if (ref_file == NULL) {
        perror("Error creating ref");
        return;
    }
    fprintf(ref_file, "%s\n", commit_hash);
    fclose(ref_file);

    printf("Created %s: %s -> %s\n", ref_type, ref_name, commit_hash);
}

//获取当前分支指向的最后一个提交哈希值，这个哈希值可以在logs文件里面找到
//logs存储的文件格式：
//Commit Hash: 612aae8b40704c8520200f6e3c4cd674c624fe3c
//Time: 2024 - 09 - 21 10:19 : 35
//Message : 第七次提交，尝试着创建副本来解决dir文件夹名称问题
//
//Commit Hash : c70678ca2f27c20e12bbe837743fcd280fdd7386
//Time : 2024 - 09 - 21 10:42 : 40
//Message : 试试双文件夹看看行不行

// 从logs文件中读取最后一次提交记录
// @param commit_hash 用于存储读取到的提交哈希值
// @return 1 读取成功 0 读取失败
int load_current_commit_hash(char* commit_hash) {
	char log_path[256];
	snprintf(log_path, sizeof(log_path), ".git/logs");

	FILE* log_file = fopen(log_path, "r");
	if (!log_file) {
		perror("Failed to open logs file");
		return 0;
	}

	// 读取最后一次提交记录
	char line[256];
	while (fgets(line, sizeof(line), log_file)) {
		if (strncmp(line, "Commit Hash: ", 13) == 0) {
			strncpy(commit_hash, line + 13, sizeof(commit_hash) - 1);
			commit_hash[strlen(commit_hash) - 1] = '\0'; // 去掉换行符
		}
	}

	fclose(log_file);
	return 1;
}


// 创建分支：diangit branch <branch_name>
void create_branch(const char* branch_name) {
    char commit_hash[41] = { 0 };

    // 获取当前分支指向的最后一个提交哈希值
    if (!load_current_commit_hash(commit_hash)) {
        printf("Error: Could not load current commit hash.\n");
        return;
    }

    // 创建新的分支
    create_ref("heads", branch_name, commit_hash);
}


// 创建标签：diangit tag <tag_name>
void create_tag(const char* tag_name) {
    char commit_hash[41] = { 0 };

    // 获取当前分支指向的最后一个提交哈希值
    if (!load_current_commit_hash(commit_hash)) {
        printf("Error: Could not load current commit hash.\n");
        return;
    }

    // 创建新的标签
    create_ref("tags", tag_name, commit_hash);
}


// 辅助函数：列出 .git/refs/<type> 目录中的所有引用
void list_refs(const char* ref_type) {
    char ref_dir[256];
    snprintf(ref_dir, sizeof(ref_dir), ".git/refs/%s", ref_type);

    DIR* dir = opendir(ref_dir);
    if (dir == NULL) {
        perror("Error opening ref directory");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
}

// 列出所有分支：diangit branch
void list_branches() {
    printf("Branches:\n");
    list_refs("heads");
}

// 列出所有标签：diangit tag
void list_tags() {
    printf("Tags:\n");
    list_refs("tags");
}