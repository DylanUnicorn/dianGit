#ifndef _COMMIT_H_
#define _COMMIT_H_

#include <header/hash_object.h>
#include <header/gettime.h>
#include <openssl/sha.h>
#include <zlib.h>
#include <header/gitlog.h>

// ���� tree ���󣬲���Ŀ¼�µ������ļ�����Ŀ¼ת��Ϊ tree ��
char* create_tree_object(const char* dirpath);

// ���� commit ����,������ tree ����Ĺ�ϣֵ���ύ��Ϣ
void create_commit_object(const char* commit_msg, const char* tree_hash);

// ������ commit ����
void commit_all(const char* commit_msg);
#endif