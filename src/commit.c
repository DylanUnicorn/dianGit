#include <header/commit.h>
#include <header/hash_object.h>
#include <header/dirPro.h>
#include <diangit.h>


char* create_tree_object(const char* dirpath) {
    DIR* dir;
    struct dirent* entry;
    char tree_content[4096] = { 0 };  // �����ݴ�Ŀ¼������

    if ((dir = opendir(dirpath)) == NULL) {
        perror("opendir");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".git") == 0) {
            continue;
        }

        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);

        struct stat entry_stat;
        if (stat(filepath, &entry_stat) == 0) {
            if (S_ISDIR(entry_stat.st_mode)) {
                // �������Ŀ¼���ݹ�����������
                char* subtree_hash = create_tree_object(filepath);
                // ׷������Ŀ
                char entry_buffer[512];
                snprintf(entry_buffer, sizeof(entry_buffer), "040000 tree %s\t%s\n", subtree_hash, entry->d_name);
                strcat(tree_content, entry_buffer);
                free(subtree_hash);
            }
            else if (S_ISREG(entry_stat.st_mode)) {
                // ������ļ������� blob ����
                char* file_hash = create_blob_object(filepath);
                // ׷�� blob ��Ŀ
                char entry_buffer[512];
                snprintf(entry_buffer, sizeof(entry_buffer), "100644 blob %s\t%s\n", file_hash, entry->d_name);
                strcat(tree_content, entry_buffer);
                free(file_hash);
            }
        }
    }
    closedir(dir);

    // ����������Ĺ�ϣֵ���洢
    char tree_hash[41];
    compute_sha1(tree_content, strlen(tree_content), tree_hash);
    store_object(tree_hash, tree_content, strlen(tree_content));

    return strdup(tree_hash);  // ����������Ĺ�ϣֵ
}

void create_commit_object(const char* commit_msg, const char* tree_hash) {
    char commit_data[1024];
    char time_buffer[64];
    get_current_time(time_buffer, sizeof(time_buffer));

    // �����ύ��������
    snprintf(commit_data, sizeof(commit_data), "tree %s\nTime: %s\nMessage: %s\n", tree_hash, time_buffer, commit_msg);

    // ���ύ������й�ϣ����
    unsigned char commit_hash[HASH_LEN];
    compute_sha1(commit_data, strlen(commit_data), commit_hash);

    // ѹ�����洢�ύ����
    char* compressed_data = NULL;
    size_t compressed_len = 0;
    if (compress_data(commit_data, strlen(commit_data), &compressed_data, &compressed_len) == Z_OK) {
        char hash_str[41];
        for (int i = 0; i < HASH_LEN; ++i) {
            snprintf(hash_str + i * 2, 3, "%02x", commit_hash[i]);
        }

        store_object(hash_str, compressed_data, compressed_len);
        free(compressed_data);

        // �����ύ��־
        update_commit_log(hash_str, commit_msg, time_buffer);
    }
}

void commit_all(const char* commit_msg) {

    // ���� tree ���󣬱�ʾ��ǰĿ¼���ļ�����Ŀ¼
    char* tree_hash = create_tree_object(".");

    if (tree_hash != NULL) {
        // ���� tree ���󴴽��ύ����
        create_commit_object(commit_msg, tree_hash);
        free(tree_hash);
    }
    else {
        printf("Failed to create tree object.\n");
    }

    printf("Committing all files!\n");
}
