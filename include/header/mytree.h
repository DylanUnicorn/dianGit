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

// 创建 tree 对象，并将目录下的所有文件和子目录转化为 tree 项
// @param stack 保存当前状态的栈
// @param data 保存 tree 对象的字符串
// @param current_line 当前处理的行
// @param end 最后一行
// @param restore_dir 保存当前目录
void push_tree_state(TreeState** stack, char* data, char* current_line, char* end, const char* restore_dir);

// 弹出栈
// @param stack 保存当前状态的栈
// @param data 保存 tree 对象的字符串
// @param current_line 当前处理的行
// @param end 最后一行
void pop_tree_state(TreeState** stack, char** data, char** current_line, char** end, char* restore_dir);

#endif