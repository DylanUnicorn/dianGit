#ifndef _CHECKOUT_H_
#define _CHECKOUT_H_

// 检查文件是否存在冲突,姑且保留work_dir这个形参
// @param work_dir 工作目录 一般是. 
// @param file_path 文件路径 在工作目录是‘.’的情况下就是文件名
// @param target_hash 目标文件哈希值
// @return 0 无冲突 1 有冲突
int check_for_conflicts(const char* work_dir, const char* file_path, const char* target_hash);

// 备份冲突文件
// @param work_dir 工作目录 一般是.
// @param file_path 文件路径 在工作目录是‘.’的情况下就是文件名
void backup_file(const char* work_dir, const char* file_path);

// 处理冲突
// @brief 如果文件存在冲突，提示用户选择操作
// @param work_dir 工作目录 一般是.
// @param file_path 文件路径 在工作目录是‘.’的情况下就是文件名
// @param target_hash 目标文件哈希值
// @return 0 :跳过文件 1 :继续处理文件
int handle_conflicts(const char* work_dir, const char* file_path, const char* target_hash);

// 还原 blob 对象
// @param blob_hash blob 对象哈希值
// @param restore_dir 还原目录
// @param filename 文件名
void restore_blob(const char* blob_hash, const char* restore_dir, const char* filename);

// 递归还原 tree 对象，同时处理冲突
void restore_tree_with_conflicts(const char* tree_hash, const char* restore_dir);

// 字面意思
// 一定要注意解压
// @param 输入commit_hash值
// @return 返回tree_hash值,注意返回的是指针，请手动释放
char* get_tree_hash_from_commit(const char* commit_hash);

// checkout 实现，利用之前的接口函数
void checkout(const char* commit_hash);

#endif