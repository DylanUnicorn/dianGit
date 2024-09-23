#ifndef _STAGING_H_
#define _STAGING_H_

#define MAX_PATH 256
#define HASH_SIZE 41

#include <uthash.h>

struct IndexEntry {
    char hash[HASH_SIZE];
    char file_path[MAX_PATH];
};

// �����ϣ��ṹ 
struct ProcessedDir {
	char Path[MAX_PATH];
	UT_hash_handle hh;
};


// �� index �ļ���ȡ·���� hash ֵ
// @param index_file_path index �ļ�·��
// @param entries �����ȡ����Ľṹ������
// @param max_entries �ṹ���������󳤶�
int read_index_file(const char* index_file_path, struct IndexEntry* entries, int max_entries);


// �ݹ�����������
// @param entries: ���� read_index_file��õ�entries�ṹ������
// @param num_entries : entries����ĳ��ȣ���read_index_file����
// @param dir_path :Ŀ¼·�� Ĭ��Ϊ"./"
// @param processed_dirs �Ѵ����Ŀ¼�Ĺ�ϣ��
// @return ���ɵ�������Ĺ�ϣֵ,�ֶ��ͷ�
char* create_tree_object_from_entries(struct IndexEntry* entries, int num_entries, const char* dir_path, struct ProcessedDir** processed_dirs);


// ��ָ���ļ��Ĺ�ϣֵ���ļ�·����ӵ� .git/index �ļ���
// ��������add_file_to_index�У���ֻ�Ǹ�������
void update_index_with_file(const char* file_path, const char* data_hash);

// ��ָ���ļ��Ĺ�ϣֵ���ļ�·����ӵ� .git/index �ļ��У�
void add_file_to_index(const char* file_name);

// ������������׼���ļ�·����ȥ��·���е� "./"
// @brief ���add_file_to_indexʹ��
// @param input_path ����·��
// @param normalized_path ���·��
void normalize_path(const char* input_path, char* normalized_path);

// �� .git/index ���Ƴ�ָ���ļ��ļ�¼,�����Ƴ�.Git/objects�е��ļ�
// @ brief ���������Σ�գ���Ϊ����ɾ���ļ���һ�㲻��
// @ brief �����������ֻ����index���ڵ�����²Żᱻ����
void remove_file_from_index(const char* file_name);

// �������ݴ������ļ��ύ������ʵ��
void commit_stash(const char* commit_msg);

// �г��ݴ����ļ�
void list_staged_files(int verbose);

// �ݹ����Ŀ¼������Ŀ¼�е������ļ�
// @brief �������������ڵݹ����Ŀ¼������Ŀ¼�е������ļ�
// @brief ��������read_directory_files�Ǳ�
void read_directory_files_recursive(const char* dir_path, char** files, int* count, int max_files);

// �г��������ļ�
// @brief �������ֻ���г�����������Ŀ¼�ļ����������ݴ����ļ�
// @param dir_path ������·��
// @param files �����ļ�����ָ������
// @param max_files �������󳤶�
// @return �����ļ�����
int read_directory_files(const char* dir_path, char** files, int max_files);

// �鿴������״̬
// status ������Ҫ��ʾ��Щ�ļ��Ѿ��ݴ桢��Щ�ļ�û�б��ύ
// status ������Ҫ��ʾ��Щ�ļ��Ѿ��ݴ桢��Щ�ļ�û�б��ύ
// ������normalize_path����
void status();

// �������ļ�
// @brief ���ĳ���ļ��Ƿ�.gitignore�ļ�����
// @return 1��ʾ�����ԣ�0��ʾδ������
int checkignore(const char* file_name);

#endif