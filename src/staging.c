#include <header/staging.h>
#include <header/hash_object.h>
#include <diangit.h>
#include <header/commit.h>
#include <header/dirPro.h>


// �� index �ļ���ȡ·���� hash ֵ
// @param index_file_path index �ļ�·��
// @param entries �����ȡ����Ľṹ������
// @param max_entries �ṹ���������󳤶�
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

// �ݹ�����������
// @param entries: ���� read_index_file��õ�entries�ṹ������
// @param num_entries : entries����ĳ��ȣ���read_index_file����
// @param dir_path :Ŀ¼·�� Ĭ��Ϊ"./"
// @param processed_dirs �Ѵ����Ŀ¼�Ĺ�ϣ��
// @return ���ɵ�������Ĺ�ϣֵ,�ֶ��ͷ�
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
            // �ж����ļ�������Ŀ¼
            char* remaining_path = entries[i].file_path + strlen(dir_path);
            if (strchr(remaining_path, '/') == NULL) {
                // ���� blob �����ļ���
                fprintf(tree_file, "100644 blob %s\t%s\n", entries[i].hash, remaining_path);
            }
            else {
                // ���� tree ������Ŀ¼��
                char subdir[MAX_PATH] = { 0 };
                sscanf(remaining_path, "%[^/]", subdir);
				char subdir_path[MAX_PATH];
				snprintf(subdir_path, sizeof(subdir_path), "%s%s/", dir_path, subdir);


#ifdef _DEBUG
				printf("heeeeee");
#endif
#ifndef NOOO
				// ���Ŀ¼�Ƿ��Ѿ������
				struct ProcessedDir* pd = NULL;
				if (!*processed_dirs) {
					*processed_dirs = NULL;
				}
				HASH_FIND_STR(*processed_dirs, subdir_path, pd);

				if (!pd) {
					// �����Ѵ����Ŀ¼
					pd = (struct ProcessedDir*)malloc(sizeof(struct ProcessedDir));
					if (!pd) {
						perror("Failed to allocate memory");
						return NULL;
					}

					strncpy(pd->Path, subdir_path, sizeof(pd->Path));
					HASH_ADD_STR(*processed_dirs, Path, pd);
					printf("Added dir: %s\n", pd->Path);

					// �ݹ鴦����Ŀ¼
					char* subtree_hash = create_tree_object_from_entries(entries, num_entries, subdir_path, processed_dirs);
					fprintf(tree_file, "040000 tree %s\t%s\n", subtree_hash, subdir);
					free(subtree_hash);

					struct ProcessedDir* temp = NULL;
					HASH_ITER(hh, *processed_dirs, temp, pd) {
						printf("Processed dir: %s\n", temp->Path);
					}

				}
#else
				// �ݹ鴦����Ŀ¼
				char* subtree_hash = create_tree_object_from_entries(entries, num_entries, subdir_path, processed_dirs);
				fprintf(tree_file, "040000 tree %s\t%s\n", subtree_hash, subdir);
				free(subtree_hash);
#endif
            }
        }
    }

    // ��ȡ tree �ļ��ĳ���
    fseek(tree_file, 0, SEEK_END);
    size_t tree_data_len = ftell(tree_file);
    fseek(tree_file, 0, SEEK_SET);

    char* tree_data = malloc(tree_data_len);
    fread(tree_data, 1, tree_data_len, tree_file);
    fclose(tree_file);

	// ����������Ĺ�ϣֵ
	char tree_hash[HASH_LEN];
	compute_sha1(tree_data, tree_data_len, tree_hash);

    // ѹ�����洢tree����
    char* compressed_data = NULL;
    size_t compressed_len = 0;
    char hash_str[41];
    if (compress_data(tree_data, tree_data_len, &compressed_data, &compressed_len) == Z_OK) {
        hash_to_str(tree_hash, hash_str);
        store_object(hash_str, compressed_data, compressed_len);
        free(compressed_data);
    }

    free(tree_data);

	return strdup(hash_str); // ����������Ĺ�ϣֵ�ַ���
}


// ��ָ���ļ��Ĺ�ϣֵ���ļ�·����ӵ� .git/index �ļ���
// ��������add_file_to_index�У���ֻ�Ǹ�������
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
	// �����ظ��ľ͸���
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
		// д���ļ�·���͹�ϣֵ�� .git/index
		fprintf(index_file, "%s ./%s\n", data_hash, file_path);
		printf("File '%s' added to the index.\n", file_path);
	}
	else {
		printf("File '%s' updated in the index.\n", file_path);
	}

    // �ر��ļ�
    fclose(index_file);

}


// ������������׼���ļ�·����ȥ��·���е� "./"
// @brief ���add_file_to_indexʹ��
// @param input_path ����·��
// @param normalized_path ���·��
void normalize_path(const char* input_path, char* normalized_path) {
	const char* src = input_path;
	char* dst = normalized_path;

	while (*src != '\0') {
		if (src[0] == '.' && src[1] == '/') {
			src += 2; // ���� "./"
		}
		else {
			*dst++ = *src++;
		}
	}
	*dst = '\0'; // ��ֹ��
}


// ��ָ���ļ��Ĺ�ϣֵ���ļ�·����ӵ� .git/index �ļ��У�
void add_file_to_index(const char* file_name) {

	if (checkignore(file_name)) return; // ����Ƿ񱻺���

    char* file_hash = create_blob_object(file_name);

	if (file_hash == NULL) return;

	update_index_with_file(file_name, file_hash);

	free(file_hash);
}

// �� .git/index ���Ƴ�ָ���ļ��ļ�¼,�����Ƴ�.Git/objects�е��ļ�
// @ brief ���������Σ�գ���Ϊ����ɾ���ļ���һ�㲻��
// @ brief �����������ֻ����index���ڵ�����²Żᱻ����
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
	hashData(file_name, file_hash); //�Ͳ���Ӽ�����

	fclose(index_file);
	fclose(temp_file);
	remove(".git/index");
	rename(".git/index.tmp", ".git/index");
	const char filedir[256] = ".git/objects/";
	// ƴ��Ŀ¼
	char file_path[256];
	snprintf(file_path, sizeof(file_path), "%s%s", filedir, file_hash);

	remove(file_path);

	printf("File '%s' removed from the index.\n", file_name);
}

// �������ݴ������ļ��ύ������ʵ��
void commit_stash(const char* commit_msg) {

	struct IndexEntry index_entries[50];
	size_t num_entries = read_index_file(".git/index", index_entries, 100);
	if (num_entries <= 0) {
		printf("Error: No files in the index. or failed to read.\n");
		return;
	}

	struct ProcessedDir* processed_dirs = NULL;

	printf("Committing %d files...\n", num_entries);

	// ���������󣬱�ʾ index �еĹ�����,��Ҫ�����ϣ��
	char* tree_hash = create_tree_object_from_entries(index_entries, num_entries, "./", &processed_dirs);
    
    if (tree_hash != NULL) {
        // ���� tree ���󴴽��ύ����
        create_commit_object(commit_msg, tree_hash);
        free(tree_hash);
    }
    else {
        printf("Failed to create tree object.\n");
    }

	printf("Committing all files in stash!\n");

#ifndef _DEBUG
    // ����ݴ�����index�ļ���
	FILE* index_file = fopen(".git/index", "w");
	if (!index_file) {
		perror("Failed to open index file");
		return;
	}
	fclose(index_file);
#endif

	// �ͷŹ�ϣ��
	struct ProcessedDir* current_dir, * tmp;
	HASH_ITER(hh, processed_dirs, current_dir, tmp) {
		HASH_DEL(processed_dirs, current_dir);
		free(current_dir);
	}
	// ����Ҫ�ͷ�processed_dirs����Ϊprocessed_dirs��һ��ָ�룬ָ�����һ����ַ�������ַ���ں����ⲿ����ģ����Բ���Ҫ�ͷ�

}

// �г��ݴ����ļ�,�Լ�verbose�����Ĵ���
void list_staged_files(int verbose) {
	FILE* index_file = fopen(".git/index", "r");
	char line[1024];
	if (!index_file) {
		printf("No files in the index.\n");
		return;
	}

	while (fgets(line, sizeof(line), index_file)) {
		if (verbose) {
			printf("Verbose: %s", line);  // �����ϸ��Ϣ
		}
		else {
			char hash[41], file_name[256];
			sscanf(line, "%s %s", hash, file_name);
			printf("%s\n", file_name);  // ֻ����ļ���
		}
	}

	fclose(index_file);
}

// �ݹ����Ŀ¼������Ŀ¼�е������ļ�
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

// �г��������ļ�
// @brief �������ֻ���г�����������Ŀ¼�ļ����������ݴ����ļ�
// @param dir_path ������·��
// @param files �����ļ�����ָ������
// @param max_files �������󳤶�
int read_directory_files(const char* dir_path, char** files, int max_files) {
	int count = 0;
	read_directory_files_recursive(dir_path, files, &count, max_files);
	return count;
}


// �鿴������״̬
// status ������Ҫ��ʾ��Щ�ļ��Ѿ��ݴ桢��Щ�ļ�û�б��ύ
// ������normalize_path����
void status() {
	struct IndexEntry index_entries[100];
	size_t num_entries = read_index_file(".git/index", index_entries, 100);
	if (num_entries <= 0) {
		printf("Error: No files in the index.\n");
		return;
	}

	// ��ȡ�������ļ�
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

	// �ͷ��ļ���
	for (int i = 0; i < num_files; ++i) {
		free(files[i]);
	}
}

// �������ļ�
// @brief ���ĳ���ļ��Ƿ�.gitignore�ļ�����
// @return 1��ʾ�����ԣ�0��ʾδ������
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
		line[strcspn(line, "\n")] = 0; // ȥ�����з�

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

