#include<header/mytree.h>

// 创建 tree 对象，并将目录下的所有文件和子目录转化为 tree 项
void push_tree_state(TreeState** stack, char* data, char* current_line, char* end, const char* restore_dir) {
    TreeState* new_state = (TreeState*)malloc(sizeof(TreeState));
    new_state->data = data;
    new_state->current_line = current_line;
    new_state->end = end;
    strncpy(new_state->restore_dir, restore_dir, sizeof(new_state->restore_dir));
    new_state->next = *stack;
    *stack = new_state;
}

void pop_tree_state(TreeState** stack, char** data, char** current_line, char** end, char* restore_dir) {
    if (*stack == NULL) return;
    TreeState* top = *stack;
    *data = top->data;
    *current_line = top->current_line;
    *end = top->end;
    strncpy(restore_dir, top->restore_dir, 1024);
    *stack = top->next;
    free(top);
}