#include <header/checkout.h>
#include <diangit.h>
#include <header/dirPro.h>
#include <header/hash_object.h>
#include <header/cat_file.h>
#include <header/init.h>
#include <header/mytree.h>

// ����ļ��Ƿ���ڳ�ͻ,���ұ���work_dir����β�
// @param work_dir ����Ŀ¼ һ����. 
// @param file_path �ļ�·�� �ڹ���Ŀ¼�ǡ�.��������¾����ļ���
// @param target_hash Ŀ���ļ���ϣֵ
// @return 0 �޳�ͻ 1 �г�ͻ
int check_for_conflicts(const char* work_dir, const char* file_path, const char* target_hash) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", work_dir, file_path);

    // ����֮ǰʵ�ֵ� hashData ���㵱ǰ�ļ��Ĺ�ϣֵ
    char current_hash[41] = { 0 };
    hashData(full_path, current_hash);
    // �ļ���������Ϊ��ͻ
	if (current_hash[0] == '\0') {
		return 1;
	}

    // ����ļ���ϣ��һ�£�˵�����ڳ�ͻ
    return strcmp(current_hash, target_hash) != 0;
}

// ���ݳ�ͻ�ļ�
// @param work_dir ����Ŀ¼ һ����.
// @param file_path �ļ�·�� �ڹ���Ŀ¼�ǡ�.��������¾����ļ���
void backup_file(const char* work_dir, const char* file_path) {
    char backup_dir[1024];
    snprintf(backup_dir, sizeof(backup_dir), "%s/.git/backups", work_dir);

    // ��������Ŀ¼
	create_directory(backup_dir);

    char src_path[1024], dest_path[1024];
    snprintf(src_path, sizeof(src_path), "%s/%s", work_dir, file_path);
    snprintf(dest_path, sizeof(dest_path), "%s/%s.bak", backup_dir, file_path);

    // ʹ��֮ǰ���ļ����ƺ��� copy_file
    copy_file(src_path, dest_path);
    printf("Backed up file: %s -> %s\n", src_path, dest_path);
}

// �����ͻ
// @brief ����ļ����ڳ�ͻ����ʾ�û�ѡ�����
// @param work_dir ����Ŀ¼ һ����.
// @param file_path �ļ�·�� �ڹ���Ŀ¼�ǡ�.��������¾����ļ���
// @param target_hash Ŀ���ļ���ϣֵ
// @return 0 :�����ļ� 1 :���������ļ�
int handle_conflicts(const char* work_dir, const char* file_path, const char* target_hash) {
    if (check_for_conflicts(work_dir, file_path, target_hash)) {
        printf("Conflict detected for file: %s\n", file_path);

        // ��ʾ�û�ѡ�����
        char choice;
        printf("Choose action for %s: (b)ackup, (o)verwrite, (s)kip: ", file_path);
        scanf(" %c", &choice);

        switch (choice) {
        case 'b':
            backup_file(work_dir, file_path);
            return 1;  // ���������ļ�
        case 'o':
            return 1;  // ֱ�Ӹ����ļ�
        case 's':
            return 0;  // �������ļ�
        default:
            printf("Invalid choice, skipping file.\n");
            return 0;  // Ĭ������
        }
    }

    return 1;  // ���û�г�ͻ����������
}

// ��ԭ blob ����
// @param blob_hash blob �����ϣֵ
// @param restore_dir ��ԭĿ¼
// @param filename �ļ���
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

	// ʹ��֮ǰ�� write_file ����д���ļ�
    create_binary_file(restore_path, content + strlen(content) +1, blob_len - (content - blob_data) - strlen(content) -1);
	free(blob_data);
}


// �ݹ黹ԭ tree ����ͬʱ�����ͻ(old)
void restore_tree_with_conflicts(const char* tree_hash, const char* restore_dir) {
    // ��ȡ tree ����ʹ��֮ǰ�� read_compressed_object ����
    char tree_file_path[256];
    snprintf(tree_file_path, sizeof(tree_file_path), ".git/objects/%s",tree_hash);

    char* tree_data = NULL;
    size_t tree_len = 0;
    if (read_compressed_object(tree_file_path, &tree_data, &tree_len) == -1) {
        printf("Failed to read tree object.\n");
        return;
    }

#ifdef TRY1
    // ���� tree �������ж�ȡ
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

        line = strtok(NULL, "\n");  // ʹ�� strtok_r ��������һ��
    }
#else
    char* line = tree_data;
    while (line < tree_data + tree_len) {
        char* newline_pos = strchr(line, '\n');
        if (!newline_pos) {
            break;  // û�и���Ļ��з����˳�ѭ��
        }
        *newline_pos = '\0';  // �����з��滻Ϊ null ��ֹ�����γ�������һ��

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

        line = newline_pos + 1;  // �ƶ�����һ��
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
//                // ���浱ǰ״̬��������Ŀ¼
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
	// ��ȡ commit ����ʹ��֮ǰ�� read_compressed_object ����
	char commit_file_path[256];
	snprintf(commit_file_path, sizeof(commit_file_path), ".git/objects/%s", commit_hash);

	char* commit_data = NULL;
	size_t commit_len = 0;
    // C���Ժ��ر�ĵط�
	if (read_compressed_object(commit_file_path, &commit_data, &commit_len) == -1) {
		printf("Failed to read commit object.\n");
		return NULL;
	}

	// ���� commit �����ҵ� tree ����Ĺ�ϣֵ
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


// checkout ʵ�֣�����֮ǰ�Ľӿں���
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
