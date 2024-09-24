#include <header/checkout.h>
#include <diangit.h>
#include <header/dirPro.h>
#include <header/hash_object.h>
#include <header/cat_file.h>
#include <header/init.h>
#include <header/mytree.h>

// 检查文件是否存在冲突,姑且保留work_dir这个形参
// @param work_dir 工作目录 一般是. 
// @param file_path 文件路径 在工作目录是‘.’的情况下就是文件名
// @param target_hash 目标文件哈希值
// @return 0 无冲突 1 有冲突
int check_for_conflicts(const char* work_dir, const char* file_path, const char* target_hash) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", work_dir, file_path);

    // 调用之前实现的 hashData 计算当前文件的哈希值
    char current_hash[41] = { 0 };
    hashData(full_path, current_hash);
    // 文件不存在视为冲突
	if (current_hash[0] == '\0') {
		return 1;
	}

    // 如果文件哈希不一致，说明存在冲突
    return strcmp(current_hash, target_hash) != 0;
}

// 备份冲突文件
// @param work_dir 工作目录 一般是.
// @param file_path 文件路径 在工作目录是‘.’的情况下就是文件名
void backup_file(const char* work_dir, const char* file_path) {
    char backup_dir[1024];
    snprintf(backup_dir, sizeof(backup_dir), "%s/.git/backups", work_dir);

    // 创建备份目录
	create_directory(backup_dir);

    char src_path[1024], dest_path[1024];
    snprintf(src_path, sizeof(src_path), "%s/%s", work_dir, file_path);
    snprintf(dest_path, sizeof(dest_path), "%s/%s.bak", backup_dir, file_path);

    // 使用之前的文件复制函数 copy_file
    copy_file(src_path, dest_path);
    printf("Backed up file: %s -> %s\n", src_path, dest_path);
}

// 处理冲突
// @brief 如果文件存在冲突，提示用户选择操作
// @param work_dir 工作目录 一般是.
// @param file_path 文件路径 在工作目录是‘.’的情况下就是文件名
// @param target_hash 目标文件哈希值
// @return 0 :跳过文件 1 :继续处理文件
int handle_conflicts(const char* work_dir, const char* file_path, const char* target_hash) {
    if (check_for_conflicts(work_dir, file_path, target_hash)) {
        printf("Conflict detected for file: %s\n", file_path);

        // 提示用户选择操作
        char choice;
        printf("Choose action for %s: (b)ackup, (o)verwrite, (s)kip: ", file_path);
        scanf(" %c", &choice);

        switch (choice) {
        case 'b':
            backup_file(work_dir, file_path);
            return 1;  // 继续覆盖文件
        case 'o':
            return 1;  // 直接覆盖文件
        case 's':
            return 0;  // 跳过此文件
        default:
            printf("Invalid choice, skipping file.\n");
            return 0;  // 默认跳过
        }
    }

    return 1;  // 如果没有冲突，继续处理
}

// 还原 blob 对象
// @param blob_hash blob 对象哈希值
// @param restore_dir 还原目录
// @param filename 文件名
void restore_blob(const char* blob_hash, const char* restore_dir, const char* filename) {
	char blob_file_path[256];
	snprintf(blob_file_path, sizeof(blob_file_path), ".git/objects/%s", blob_hash);

	char* blob_data = NULL;
	size_t blob_len = 0;
	if (read_compressed_object(blob_file_path, &blob_data, &blob_len) == -1) {
		printf("Failed to read blob object.\n");
		return;
	}

	char* content = strstr(blob_data, "\0");
	if (!content) {
		printf("Invalid blob object.\n");
		free(blob_data);
		return;
	}

	content += 1;
	char restore_path[1024];
	snprintf(restore_path, sizeof(restore_path), "%s/%s", restore_dir, filename);

	// 使用之前的 write_file 函数写入文件
    create_binary_file(restore_path, content + strlen(content) +1, blob_len - (content - blob_data) - strlen(content) -1);
	free(blob_data);
}


// 递归还原 tree 对象，同时处理冲突(old)
void restore_tree_with_conflicts(const char* tree_hash, const char* restore_dir) {
    // 读取 tree 对象，使用之前的 read_compressed_object 函数
    char tree_file_path[256];
    snprintf(tree_file_path, sizeof(tree_file_path), ".git/objects/%s",tree_hash);

    char* tree_data = NULL;
    size_t tree_len = 0;
    if (read_compressed_object(tree_file_path, &tree_data, &tree_len) == -1) {
        printf("Failed to read tree object.\n");
        return;
    }

#ifdef TRY1
    // 解析 tree 对象，逐行读取
    char* line = strtok(tree_data, "\n");
    while (line) {
        char mode[7], type[5], hash[41], filename[1024];
        if (sscanf(line, "%s %s %40s\t%s", mode, type, hash, filename) != 4) {
            printf("sscanf error in restore_tree_with_conflicts function\n");
        }

        if (strcmp(type, "blob") == 0) {
            if (handle_conflicts(restore_dir, filename, hash)) {
#ifdef _DEBUG
				printf("!!!Restoring blob: %s\n", filename);
#endif
                restore_blob(hash, restore_dir, filename);
            }
#ifdef _DEBUG
			else {
				printf("!!!Conflict Detected,Skipped file: %s\n", filename);

			}
#endif
        }
        else if (strcmp(type, "tree") == 0) {
            char subdir[1024];
            snprintf(subdir, sizeof(subdir), "%s/%s", restore_dir, filename);
            create_directory(subdir);
            restore_tree_with_conflicts(hash, subdir);
        }

        line = strtok(NULL, "\n");  // 使用 strtok_r 来解析下一行
    }
#else
    char* line = tree_data;
    while (line < tree_data + tree_len) {
        char* newline_pos = strchr(line, '\n');
        if (!newline_pos) {
            break;  // 没有更多的换行符，退出循环
        }
        *newline_pos = '\0';  // 将换行符替换为 null 终止符，形成完整的一行

        char mode[7], type[5], hash[41], filename[1024];
        if (sscanf(line, "%s %s %40s %s", mode, type, hash, filename) != 4) {
            printf("sscanf error in restore_tree_with_conflicts function\n");
        }

        if (strcmp(type, "blob") == 0) {
            if (handle_conflicts(restore_dir, filename, hash)) {
                restore_blob(hash, restore_dir, filename);
            }
        }
        else if (strcmp(type, "tree") == 0) {
            char subdir[1024];
            snprintf(subdir, sizeof(subdir), "%s/%s", restore_dir, filename);
            create_directory(subdir);
            restore_tree_with_conflicts(hash, subdir);
        }

        line = newline_pos + 1;  // 移动到下一行
    }

#endif
    free(tree_data);
}

//// new
//void restore_tree_with_conflicts(const char* tree_hash, const char* restore_dir) {
//    TreeState* stack = NULL;
//
//    char* tree_data = NULL;
//    size_t tree_len = 0;
//    char tree_file_path[256];
//    snprintf(tree_file_path, sizeof(tree_file_path), ".git/objects/%s", tree_hash);
//
//    if (read_compressed_object(tree_file_path, &tree_data, &tree_len) == -1) {
//        printf("Failed to read tree object.\n");
//        return;
//    }
//
//    char* line = tree_data;
//    char* end = tree_data + tree_len;
//
//    while (1) {
//        while (line < end) {
//            char mode[7], type[5], hash[41], filename[1024];
//            int read_count = sscanf(line, "%s %s %40s\t%s", mode, type, hash, filename);
//            if (read_count != 4) {
//                printf("Failed to parse tree line: %s\n", line);
//                line += strlen(line) + 1;
//                continue;
//            }
//
//            if (strcmp(type, "blob") == 0) {
//                if (handle_conflicts(restore_dir, filename, hash)) {
//                    restore_blob(hash, restore_dir, filename);
//                }
//            }
//            else if (strcmp(type, "tree") == 0) {
//                char subdir[1024];
//                snprintf(subdir, sizeof(subdir), "%s/%s", restore_dir, filename);
//                create_directory(subdir);
//
//                // 保存当前状态并进入子目录
//                push_tree_state(&stack, tree_data, line + strlen(line) + 1, end, restore_dir);
//                tree_data = NULL;
//                tree_len = 0;
//                snprintf(tree_file_path, sizeof(tree_file_path), ".git/objects/%s", hash);
//                if (read_compressed_object(tree_file_path, &tree_data, &tree_len) == -1) {
//                    printf("Failed to read tree object.\n");
//                    return;
//                }
//                line = tree_data;
//                end = tree_data + tree_len;
//                restore_dir = subdir;
//                break;
//            }
//
//            line += strlen(line) + 1;
//        }
//
//        if (line >= end) {
//            if (stack == NULL) break;
//            pop_tree_state(&stack, &tree_data, &line, &end, restore_dir);
//        }
//    }
//
//    free(tree_data);
//}

char* get_tree_hash_from_commit(const char* commit_hash) {
	// 读取 commit 对象，使用之前的 read_compressed_object 函数
	char commit_file_path[256];
	snprintf(commit_file_path, sizeof(commit_file_path), ".git/objects/%s", commit_hash);

	char* commit_data = NULL;
	size_t commit_len = 0;
    // C语言很特别的地方
	if (read_compressed_object(commit_file_path, &commit_data, &commit_len) == -1) {
		printf("Failed to read commit object.\n");
		return NULL;
	}

	// 解析 commit 对象，找到 tree 对象的哈希值
	char* tree_hash = strstr(commit_data, "tree ");
	if (!tree_hash) {
		printf("Invalid commit object.\n");
		free(commit_data);
		return NULL;
	}

	tree_hash += 5;
	char* end = strchr(tree_hash, '\n');
	if (!end) {
		printf("Invalid commit object.\n");
		free(commit_data);
		return NULL;
	}

	*end = '\0';
	char* result = strdup(tree_hash);
	free(commit_data);
	return result;
}


// checkout 实现，利用之前的接口函数
void checkout(const char* commit_hash) {
    printf("Checking out commit %s...\n", commit_hash);

    char* tree_hash = get_tree_hash_from_commit(commit_hash);
    if (tree_hash) {
#ifdef _DEBUG
		printf("Restoring files... treehash: %s\n", tree_hash);
#endif

        restore_tree_with_conflicts(tree_hash, ".");
        printf("Checkout complete.\n");
        free(tree_hash);
    }
    else {
        printf("Failed to find tree for commit %s.\n", commit_hash);
    }
}
