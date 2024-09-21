#ifndef _MYTREE_H_
#define _MYTREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TreeState {
    char* data;
    char* current_line;
    char* end;
    char restore_dir[1024];
    struct TreeState* next;
} TreeState;

// ���� tree ���󣬲���Ŀ¼�µ������ļ�����Ŀ¼ת��Ϊ tree ��
// @param stack ���浱ǰ״̬��ջ
// @param data ���� tree ������ַ���
// @param current_line ��ǰ�������
// @param end ���һ��
// @param restore_dir ���浱ǰĿ¼
void push_tree_state(TreeState** stack, char* data, char* current_line, char* end, const char* restore_dir);

// ����ջ
// @param stack ���浱ǰ״̬��ջ
// @param data ���� tree ������ַ���
// @param current_line ��ǰ�������
// @param end ���һ��
void pop_tree_state(TreeState** stack, char** data, char** current_line, char** end, char* restore_dir);

#endif