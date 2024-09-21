#ifndef _CHECKOUT_H_
#define _CHECKOUT_H_

// ����ļ��Ƿ���ڳ�ͻ,���ұ���work_dir����β�
// @param work_dir ����Ŀ¼ һ����. 
// @param file_path �ļ�·�� �ڹ���Ŀ¼�ǡ�.��������¾����ļ���
// @param target_hash Ŀ���ļ���ϣֵ
// @return 0 �޳�ͻ 1 �г�ͻ
int check_for_conflicts(const char* work_dir, const char* file_path, const char* target_hash);

// ���ݳ�ͻ�ļ�
// @param work_dir ����Ŀ¼ һ����.
// @param file_path �ļ�·�� �ڹ���Ŀ¼�ǡ�.��������¾����ļ���
void backup_file(const char* work_dir, const char* file_path);

// �����ͻ
// @brief ����ļ����ڳ�ͻ����ʾ�û�ѡ�����
// @param work_dir ����Ŀ¼ һ����.
// @param file_path �ļ�·�� �ڹ���Ŀ¼�ǡ�.��������¾����ļ���
// @param target_hash Ŀ���ļ���ϣֵ
// @return 0 :�����ļ� 1 :���������ļ�
int handle_conflicts(const char* work_dir, const char* file_path, const char* target_hash);

// ��ԭ blob ����
// @param blob_hash blob �����ϣֵ
// @param restore_dir ��ԭĿ¼
// @param filename �ļ���
void restore_blob(const char* blob_hash, const char* restore_dir, const char* filename);

// �ݹ黹ԭ tree ����ͬʱ�����ͻ
void restore_tree_with_conflicts(const char* tree_hash, const char* restore_dir);

// ������˼
// һ��Ҫע���ѹ
// @param ����commit_hashֵ
// @return ����tree_hashֵ,ע�ⷵ�ص���ָ�룬���ֶ��ͷ�
char* get_tree_hash_from_commit(const char* commit_hash);

// checkout ʵ�֣�����֮ǰ�Ľӿں���
void checkout(const char* commit_hash);

#endif