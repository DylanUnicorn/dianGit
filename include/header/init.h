#ifndef _INIT_H_
#define _INIT_H_

#include <stddef.h>
// �����ļ���д������
// @param path �ļ�·��
// @param content Ҫд�������-�ı���ʽ
void create_file(const char* path, const char* content);

// ����wģʽ�޷����㴴���������ļ�����������һ��������ʹ��wbģʽ
// ����д�룬Ҫ������д�룬���򷵻�ʧ��
// �����ļ���д�����������
// @param path �ļ�·��
// @param data Ҫд�������
// @param len ���ݳ���
// @return 0 �ɹ� -1 ʧ��
int create_binary_file(const char* path, const void* data, size_t len);

// ���Ŀ¼�Ƿ����
int directory_exists(const char* dir);

//����Ŀ¼��ȷ�����и�Ŀ¼������
int create_directory(const char* dir);

// ��ʼ���ֿ�
void init_repository(const char* dir);

#endif