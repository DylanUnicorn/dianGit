#include <header/staging.h>
#include <header/hash_object.h>
#include <diangit.h>
#include <header/commit.h>
#include <header/dirPro.h>


// 从 index 文件读取路径和 hash 值
// @param index_file_path index 文件路径
// @param entries 保存读取结果的结构体数组
// @param max_entries 结构体数组的最大长度
int read_index_file(const char* index_file_path, struct IndexEntry* entries, int max_entries) {
    FILE* index_file = fopen(index_file_path, "r");
    if (!index_file) {
        perror("Failed to open .git/index");
        return -1;
    }

    int count = 0;
    while (fscanf(index_file, "%40s %255s", entries[count].hash, entries[count].file_path) != EOF) {
        count++;
        if (count >= max_entries) {
            break;
        }
    }

    fclose(index_file);
    return count;
}

// 递归生成树对象
// @param entries: 根据 read_index_file获得的entries结构体数组
// @param num_entries : entries数组的长度，由read_index_file返回
// @param dir_path :目录路径 默认为"./"
// @param processed_dirs 已处理的目录的哈希表
// @return 生成的树对象的哈希值,手动释放
char* create_tree_object_from_entries(struct IndexEntry* entries, int num_entries, const char* dir_path, struct ProcessedDir** processed_dirs) {
    FILE* tree_file = tmpfile();
    if (!tree_file) {
        perror("Failed to create temporary tree object");
        return;
    }
#ifdef _DEBUG
	printf(" ... Creating tree object for directory: %s\n", dir_path);
#endif
    for (int i = 0; i < num_entries; ++i) {
        if (strncmp(entries[i].file_path, dir_path, strlen(dir_path)) == 0) {
            // 判断是文件还是子目录
            char* remaining_path = entries[i].file_path + strlen(dir_path);
            if (strchr(remaining_path, '/') == NULL) {
                // 处理 blob 对象（文件）
                fprintf(tree_file, "100644 blob %s\t%s\n", entries[i].hash, remaining_path);
            }
            else {
                // 处理 tree 对象（子目录）
                char subdir[MAX_PATH] = { 0 };
                sscanf(remaining_path, "%[^/]", subdir);
				char subdir_path[MAX_PATH];
				snprintf(subdir_path, sizeof(subdir_path), "%s%s/", dir_path, subdir);


#ifdef _DEBUG
				printf("heeeeee");
#endif
#ifndef NOOO
				// 检查目录是否已经处理过
				struct ProcessedDir* pd = NULL;
				if (!*processed_dirs) {
					*processed_dirs = NULL;
				}
				HASH_FIND_STR(*processed_dirs, subdir_path, pd);

				if (!pd) {
					// 保存已处理的目录
					pd = (struct ProcessedDir*)malloc(sizeof(struct ProcessedDir));
					if (!pd) {
						perror("Failed to allocate memory");
						return NULL;
					}

					strncpy(pd->Path, subdir_path, sizeof(pd->Path));
					HASH_ADD_STR(*processed_dirs, Path, pd);
					printf("Added dir: %s\n", pd->Path);

					// 递归处理子目录
					char* subtree_hash = create_tree_object_from_entries(entries, num_entries, subdir_path, processed_dirs);
					fprintf(tree_file, "040000 tree %s\t%s\n", subtree_hash, subdir);
					free(subtree_hash);

					struct ProcessedDir* temp = NULL;
					HASH_ITER(hh, *processed_dirs, temp, pd) {
						printf("Processed dir: %s\n", temp->Path);
					}

				}
#else
				// 递归处理子目录
				char* subtree_hash = create_tree_object_from_entries(entries, num_entries, subdir_path, processed_dirs);
				fprintf(tree_file, "040000 tree %s\t%s\n", subtree_hash, subdir);
				free(subtree_hash);
#endif
            }
        }
    }

    // 获取 tree 文件的长度
    fseek(tree_file, 0, SEEK_END);
    size_t tree_data_len = ftell(tree_file);
    fseek(tree_file, 0, SEEK_SET);

    char* tree_data = malloc(tree_data_len);
    fread(tree_data, 1, tree_data_len, tree_file);
    fclose(tree_file);

	// 计算树对象的哈希值
	char tree_hash[HASH_LEN];
	compute_sha1(tree_data, tree_data_len, tree_hash);

    // 压缩并存储tree对象
    char* compressed_data = NULL;
    size_t compressed_len = 0;
    char hash_str[41];
    if (compress_data(tree_data, tree_data_len, &compressed_data, &compressed_len) == Z_OK) {
        hash_to_str(tree_hash, hash_str);
        store_object(hash_str, compressed_data, compressed_len);
        free(compressed_data);
    }

    free(tree_data);

	return strdup(hash_str); // 返回树对象的哈希值字符串
}


// 将指定文件的哈希值及文件路径添加到 .git/index 文件中
// 主函数在add_file_to_index中，这只是辅助函数
void update_index_with_file(const char* file_path, const char* data_hash) {
    
	FILE* index_file = NULL;
	int found = 0;

	if (file_exists(".git/index")) {
		index_file = fopen(".git/index", "r+");
		if (!index_file) {
			printf("Error: Could not open index file.\n");
			return;
		}

	}
	else {
		index_file = fopen(".git/index", "a");
		if (!index_file) {
			printf("Error: Could not create index file.\n");
			return;
		}
	}
	// 发现重复的就覆盖
	char line[256];
	while (fgets(line, sizeof(line), index_file)) {
		char stored_file[256];
		sscanf(line, "%*s %s", stored_file);
		char stored_file_normalized[256];
		char file_name_normalized[256];
		normalize_path(stored_file, stored_file_normalized);
		normalize_path(file_path, file_name_normalized);
		if (strcmp(stored_file_normalized, file_name_normalized) == 0) {
			printf("File '%s' already exists in the index, updating hash.\n", file_path);
			fseek(index_file, -strlen(line)-1, SEEK_CUR);
			fprintf(index_file, "%s ./%s\n", data_hash, file_path);
			found = 1;
			break;
		}
	}

	if (!found) {
		fseek(index_file, 0, SEEK_END);
		// 写入文件路径和哈希值到 .git/index
		fprintf(index_file, "%s ./%s\n", data_hash, file_path);
		printf("File '%s' added to the index.\n", file_path);
	}
	else {
		printf("File '%s' updated in the index.\n", file_path);
	}

    // 关闭文件
    fclose(index_file);

}


// 辅助函数：标准化文件路径，去除路径中的 "./"
// @brief 配合add_file_to_index使用
// @param input_path 输入路径
// @param normalized_path 输出路径
void normalize_path(const char* input_path, char* normalized_path) {
	const char* src = input_path;
	char* dst = normalized_path;

	while (*src != '\0') {
		if (src[0] == '.' && src[1] == '/') {
			src += 2; // 跳过 "./"
		}
		else {
			*dst++ = *src++;
		}
	}
	*dst = '\0'; // 终止符
}


// 将指定文件的哈希值及文件路径添加到 .git/index 文件中，
void add_file_to_index(const char* file_name) {

	if (checkignore(file_name)) return; // 检查是否被忽略

    char* file_hash = create_blob_object(file_name);

	if (file_hash == NULL) return;

	update_index_with_file(file_name, file_hash);

	free(file_hash);
}

// 从 .git/index 中移除指定文件的记录,并且移除.Git/objects中的文件
// @ brief 这个函数很危险，因为它会删除文件，一般不用
// @ brief 但是这个函数只有在index存在的情况下才会被调用
void remove_file_from_index(const char* file_name) {

	FILE* index_file = fopen(".git/index", "r");
	FILE* temp_file = fopen(".git/index.tmp", "w");
	char line[1024];

	if (!index_file || !temp_file) {
		printf("Error: Could not open index or temp file.\n");
		return;
	}

	while (fgets(line, sizeof(line), index_file)) {
		char stored_file[256];
		sscanf(line, "%*s %s", stored_file);

		char stored_file_normalized[256];
		char file_name_normalized[256];

		normalize_path(stored_file, stored_file_normalized);
		normalize_path(file_name, file_name_normalized);
#ifdef _DEBUG
		printf("Comparing '%s' with '%s'\n", stored_file_normalized, file_name_normalized);
#endif
		if (strcmp(stored_file_normalized, file_name_normalized) != 0) {
			fputs(line, temp_file);
		}
#ifdef _DEBUG
		else {
			printf("skipping file: %s\n", stored_file);
		}
#endif
	}
	//const char* work_dir = "E:/Hust/Memo/oneDianDian/test";
	//char filep[256];
	//snprintf(filep, sizeof(filep), "%s/%s", work_dir, file_name);

    char file_hash[41] = { 0 };
	hashData(file_name, file_hash); //就不添加检验了

	fclose(index_file);
	fclose(temp_file);
	remove(".git/index");
	rename(".git/index.tmp", ".git/index");
	const char filedir[256] = ".git/objects/";
	// 拼接目录
	char file_path[256];
	snprintf(file_path, sizeof(file_path), "%s%s", filedir, file_hash);

	remove(file_path);

	printf("File '%s' removed from the index.\n", file_name);
}

// 将所有暂存区的文件提交的完整实现
void commit_stash(const char* commit_msg) {

	struct IndexEntry index_entries[50];
	size_t num_entries = read_index_file(".git/index", index_entries, 100);
	if (num_entries <= 0) {
		printf("Error: No files in the index. or failed to read.\n");
		return;
	}

	struct ProcessedDir* processed_dirs = NULL;

	printf("Committing %d files...\n", num_entries);

	// 创建树对象，表示 index 中的工作树,需要传入哈希表
	char* tree_hash = create_tree_object_from_entries(index_entries, num_entries, "./", &processed_dirs);
    
    if (tree_hash != NULL) {
        // 基于 tree 对象创建提交对象
        create_commit_object(commit_msg, tree_hash);
        free(tree_hash);
    }
    else {
        printf("Failed to create tree object.\n");
    }

	printf("Committing all files in stash!\n");

#ifndef _DEBUG
    // 清空暂存区（index文件）
	FILE* index_file = fopen(".git/index", "w");
	if (!index_file) {
		perror("Failed to open index file");
		return;
	}
	fclose(index_file);
#endif

	// 释放哈希表
	struct ProcessedDir* current_dir, * tmp;
	HASH_ITER(hh, processed_dirs, current_dir, tmp) {
		HASH_DEL(processed_dirs, current_dir);
		free(current_dir);
	}
	// 不需要释放processed_dirs，因为processed_dirs是一个指针，指向的是一个地址，这个地址是在函数外部分配的，所以不需要释放

}

// 列出暂存区文件,以及verbose参数的处理
void list_staged_files(int verbose) {
	FILE* index_file = fopen(".git/index", "r");
	char line[1024];
	if (!index_file) {
		printf("No files in the index.\n");
		return;
	}

	while (fgets(line, sizeof(line), index_file)) {
		if (verbose) {
			printf("Verbose: %s", line);  // 输出详细信息
		}
		else {
			char hash[41], file_name[256];
			sscanf(line, "%s %s", hash, file_name);
			printf("%s\n", file_name);  // 只输出文件名
		}
	}

	fclose(index_file);
}

// 递归遍历目录及其子目录中的所有文件
void read_directory_files_recursive(const char* dir_path, char** files, int* count, int max_files) {
	DIR* dir = opendir(dir_path);
	if (!dir) {
		perror("Failed to open directory");
		return;
	}

	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_REG) {
			if (*count >= max_files) {
				break;
			}
			char file_path[MAX_PATH];
			snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, entry->d_name);
			files[*count] = strdup(file_path);
			(*count)++;
		}
		else if (entry->d_type == DT_DIR) {
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name,".git")) {
				char subdir_path[MAX_PATH];
				snprintf(subdir_path, sizeof(subdir_path), "%s/%s", dir_path, entry->d_name);
				read_directory_files_recursive(subdir_path, files, count, max_files);
			}
		}
	}

	closedir(dir);
}

// 列出工作区文件
// @brief 这个函数只是列出工作区及子目录文件，不包括暂存区文件
// @param dir_path 工作区路径
// @param files 保存文件名的指针数组
// @param max_files 数组的最大长度
int read_directory_files(const char* dir_path, char** files, int max_files) {
	int count = 0;
	read_directory_files_recursive(dir_path, files, &count, max_files);
	return count;
}


// 查看工作区状态
// status 命令需要显示哪些文件已经暂存、哪些文件没有被提交
// 会用上normalize_path函数
void status() {
	struct IndexEntry index_entries[100];
	size_t num_entries = read_index_file(".git/index", index_entries, 100);
	if (num_entries <= 0) {
		printf("Error: No files in the index.\n");
		return;
	}

	// 读取工作区文件
	char* files[100];
	size_t num_files = read_directory_files(".", files, 100);
	if (num_files <= 0) {
		printf("Error: No files in the working directory.\n");
		return;
	}

	printf("Staged files:\n");
	for (int i = 0; i < num_entries; ++i) {
		printf("%s\n", index_entries[i].file_path);
	}

	printf("\nUnstaged files:\n");
	for (int i = 0; i < num_files; ++i) {
		int found = 0;
		char normalized_file[256];
		normalize_path(files[i], normalized_file);
		for (int j = 0; j < num_entries; ++j) {
			char normalized_index_file[256];
			normalize_path(index_entries[j].file_path, normalized_index_file);

			if (strcmp(normalized_file, normalized_index_file) == 0 ||strcmp(normalized_file, ".gitignore") == 0 || checkignore(normalized_file)) {
				found = 1;
				break;
			}
		}
		if (!found) {
			printf("%s\n", files[i]);
		}
	}

	// 释放文件名
	for (int i = 0; i < num_files; ++i) {
		free(files[i]);
	}
}

// 检查忽略文件
// @brief 检查某个文件是否被.gitignore文件忽略
// @return 1表示被忽略，0表示未被忽略
int checkignore(const char* file_name) {
	FILE* ignore_file = fopen(".gitignore", "r");
	if (!ignore_file) {
		printf("No .gitignore file found.\n");
		return 0;
	}

	char line[256];
	char normalized_file_name[256];
	normalize_path(file_name, normalized_file_name);

	while (fgets(line, sizeof(line), ignore_file)) {
		line[strcspn(line, "\n")] = 0; // 去除换行符

		char normalized_pattern[256];
		normalize_path(line, normalized_pattern);

#ifdef _DEBUG
		printf("Comparing '%s' with pattern '%s' !!!\n", normalized_file_name, normalized_pattern);
#endif

		if (fnmatch(normalized_pattern, normalized_file_name, FNM_PATHNAME) == 0) {
#ifdef _DEBUG
			printf("File '%s' is ignored.\n", file_name);
#endif
			fclose(ignore_file);
			return 1;
		}
	}

	fclose(ignore_file);
	return 0;
}

