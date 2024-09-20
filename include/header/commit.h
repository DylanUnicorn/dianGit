#ifndef _COMMIT_H_
#define _COMMIT_H_

#include <header/hash_object.h>
#include <header/gettime.h>
#include <openssl/sha.h>
#include <zlib.h>
#include <header/gitlog.h>

// 创建 tree 对象，并将目录下的所有文件和子目录转化为 tree 项
char* create_tree_object(const char* dirpath);

// 创建 commit 对象,，传入 tree 对象的哈希值和提交信息
void create_commit_object(const char* commit_msg, const char* tree_hash);

// 完整的 commit 操作
void commit_all(const char* commit_msg);
#endif