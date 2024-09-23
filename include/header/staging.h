#ifndef _STAGING_H_
#define _STAGING_H_

#define MAX_PATH 256
#define HASH_SIZE 41

#include <uthash.h>

struct IndexEntry {
    char hash[HASH_SIZE];
    char file_path[MAX_PATH];
};

// 定义哈希表结构 
struct ProcessedDir {
	char Path[MAX_PATH];
	UT_hash_handle hh;
};


// 从 index 文件读取路径和 hash 值
// @param index_file_path index 文件路径
// @param entries 保存读取结果的结构体数组
// @param max_entries 结构体数组的最大长度
int read_index_file(const char* index_file_path, struct IndexEntry* entries, int max_entries);


// 递归生成树对象
// @param entries: 根据 read_index_file获得的entries结构体数组
// @param num_entries : entries数组的长度，由read_index_file返回
// @param dir_path :目录路径 默认为"./"
// @param processed_dirs 已处理的目录的哈希表
// @return 生成的树对象的哈希值,手动释放
char* create_tree_object_from_entries(struct IndexEntry* entries, int num_entries, const char* dir_path, struct ProcessedDir** processed_dirs);


// 将指定文件的哈希值及文件路径添加到 .git/index 文件中
// 主函数在add_file_to_index中，这只是辅助函数
void update_index_with_file(const char* file_path, const char* data_hash);

// 将指定文件的哈希值及文件路径添加到 .git/index 文件中，
void add_file_to_index(const char* file_name);

// 辅助函数：标准化文件路径，去除路径中的 "./"
// @brief 配合add_file_to_index使用
// @param input_path 输入路径
// @param normalized_path 输出路径
void normalize_path(const char* input_path, char* normalized_path);

// 从 .git/index 中移除指定文件的记录,并且移除.Git/objects中的文件
// @ brief 这个函数很危险，因为它会删除文件，一般不用
// @ brief 但是这个函数只有在index存在的情况下才会被调用
void remove_file_from_index(const char* file_name);

// 将所有暂存区的文件提交的完整实现
void commit_stash(const char* commit_msg);

// 列出暂存区文件
void list_staged_files(int verbose);

// 递归遍历目录及其子目录中的所有文件
// @brief 辅助函数，用于递归遍历目录及其子目录中的所有文件
// @brief 主函数在read_directory_files那边
void read_directory_files_recursive(const char* dir_path, char** files, int* count, int max_files);

// 列出工作区文件
// @brief 这个函数只是列出工作区及子目录文件，不包括暂存区文件
// @param dir_path 工作区路径
// @param files 保存文件名的指针数组
// @param max_files 数组的最大长度
// @return 返回文件个数
int read_directory_files(const char* dir_path, char** files, int max_files);

// 查看工作区状态
// status 命令需要显示哪些文件已经暂存、哪些文件没有被提交
// status 命令需要显示哪些文件已经暂存、哪些文件没有被提交
// 会用上normalize_path函数
void status();

// 检查忽略文件
// @brief 检查某个文件是否被.gitignore文件忽略
// @return 1表示被忽略，0表示未被忽略
int checkignore(const char* file_name);

#endif