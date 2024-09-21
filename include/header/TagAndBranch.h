#ifndef _TAGANDBRANCH_H_
#define _TAGANDBRANCH_H_

// ����������������֧���ǩ
void create_ref(const char* ref_type, const char* ref_name, const char* commit_hash);

// ��logs�ļ��ж�ȡ���һ���ύ��¼
// @param commit_hash ���ڴ洢��ȡ�����ύ��ϣֵ
// @return 1 ��ȡ�ɹ� 0 ��ȡʧ��
int load_current_commit_hash(char* commit_hash);

// ������֧��diangit branch <branch_name>
void create_branch(const char* branch_name);

// ������ǩ��diangit tag <tag_name>
void create_tag(const char* tag_name);

// �����������г� .git/refs/<type> Ŀ¼�е���������
void list_refs(const char* ref_type);

// �г����з�֧��diangit branch
void list_branches();

// �г����б�ǩ��diangit tag
void list_tags();



#endif