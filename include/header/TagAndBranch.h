#ifndef _TAGANDBRANCH_H_
#define _TAGANDBRANCH_H_

// 辅助函数：创建分支或标签
void create_ref(const char* ref_type, const char* ref_name, const char* commit_hash);

// 从logs文件中读取最后一次提交记录
// @param commit_hash 用于存储读取到的提交哈希值
// @return 1 读取成功 0 读取失败
int load_current_commit_hash(char* commit_hash);

// 创建分支：diangit branch <branch_name>
void create_branch(const char* branch_name);

// 创建标签：diangit tag <tag_name>
void create_tag(const char* tag_name);

// 辅助函数：列出 .git/refs/<type> 目录中的所有引用
void list_refs(const char* ref_type);

// 列出所有分支：diangit branch
void list_branches();

// 列出所有标签：diangit tag
void list_tags();



#endif