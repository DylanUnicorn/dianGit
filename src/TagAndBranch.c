#include <header/TagAndBranch.h>
#include <diangit.h>
#include <header/dirPro.h>

// ����������������֧���ǩ
void create_ref(const char* ref_type, const char* ref_name, const char* commit_hash) {
    char ref_path[256];
    snprintf(ref_path, sizeof(ref_path), ".git/refs/%s/%s", ref_type, ref_name);

    // ����Ƿ��Ѿ����ڸ÷�֧���ǩ
    FILE* ref_file = fopen(ref_path, "r");
    if (ref_file != NULL) {
        printf("%s %s already exists.\n", ref_type, ref_name);
        fclose(ref_file);
        return;
    }

    // �����ļ���д���ύ��ϣֵ
    ref_file = fopen(ref_path, "w");
    if (ref_file == NULL) {
        perror("Error creating ref");
        return;
    }
    fprintf(ref_file, "%s\n", commit_hash);
    fclose(ref_file);

    printf("Created %s: %s -> %s\n", ref_type, ref_name, commit_hash);
}

//��ȡ��ǰ��ָ֧������һ���ύ��ϣֵ�������ϣֵ������logs�ļ������ҵ�
//logs�洢���ļ���ʽ��
//Commit Hash: 612aae8b40704c8520200f6e3c4cd674c624fe3c
//Time: 2024 - 09 - 21 10:19 : 35
//Message : ���ߴ��ύ�������Ŵ������������dir�ļ�����������
//
//Commit Hash : c70678ca2f27c20e12bbe837743fcd280fdd7386
//Time : 2024 - 09 - 21 10:42 : 40
//Message : ����˫�ļ��п����в���

// ��logs�ļ��ж�ȡ���һ���ύ��¼
// @param commit_hash ���ڴ洢��ȡ�����ύ��ϣֵ
// @return 1 ��ȡ�ɹ� 0 ��ȡʧ��
int load_current_commit_hash(char* commit_hash) {
	char log_path[256];
	snprintf(log_path, sizeof(log_path), ".git/logs");

	FILE* log_file = fopen(log_path, "r");
	if (!log_file) {
		perror("Failed to open logs file");
		return 0;
	}

	// ��ȡ���һ���ύ��¼
	char line[256];
	while (fgets(line, sizeof(line), log_file)) {
		if (strncmp(line, "Commit Hash: ", 13) == 0) {
			strncpy(commit_hash, line + 13, sizeof(commit_hash) - 1);
			commit_hash[strlen(commit_hash) - 1] = '\0'; // ȥ�����з�
		}
	}

	fclose(log_file);
	return 1;
}


// ������֧��diangit branch <branch_name>
void create_branch(const char* branch_name) {
    char commit_hash[41] = { 0 };

    // ��ȡ��ǰ��ָ֧������һ���ύ��ϣֵ
    if (!load_current_commit_hash(commit_hash)) {
        printf("Error: Could not load current commit hash.\n");
        return;
    }

    // �����µķ�֧
    create_ref("heads", branch_name, commit_hash);
}


// ������ǩ��diangit tag <tag_name>
void create_tag(const char* tag_name) {
    char commit_hash[41] = { 0 };

    // ��ȡ��ǰ��ָ֧������һ���ύ��ϣֵ
    if (!load_current_commit_hash(commit_hash)) {
        printf("Error: Could not load current commit hash.\n");
        return;
    }

    // �����µı�ǩ
    create_ref("tags", tag_name, commit_hash);
}


// �����������г� .git/refs/<type> Ŀ¼�е���������
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

// �г����з�֧��diangit branch
void list_branches() {
    printf("Branches:\n");
    list_refs("heads");
}

// �г����б�ǩ��diangit tag
void list_tags() {
    printf("Tags:\n");
    list_refs("tags");
}