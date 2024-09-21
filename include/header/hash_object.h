#ifndef __HASH_OBJECT_H__
#define __HASH_OBJECT_H__

#include "init.h"

#define HASH_LEN 20 // SHA-1 输出 20 字节
#define CHUNK 16384

// 读取文件并返回其内容
char* read_file(const char* filename, size_t* file_len);

// 计算 SHA-1 哈希值
void compute_sha1(const char* data, size_t data_len, unsigned char* hash_out);

// @brief 提供将哈希值转换为字符串的函数
// @brief 传入哈希值和字符串指针
// @brief 配合compute_sha1使用
void hash_to_str(const unsigned char* hash, char* hash_str);

// 压缩文件内容
// @param input 输入数据
// @param input_len 输入数据长度 
// @param output 输出数据
// @param output_len 输出数据长度
// @return 0:成功，-1:失败
int compress_data(const char* input, size_t input_len, char** output, size_t* output_len);

// 存储压缩的 Git 对象
void store_object(const char* hash, const char* compressed_data, size_t compressed_len);

// 计算文件的哈希值,给与返回值,假如文件不存在,返回NULL
int hashData(const char* filename, char* hash_out);
/**
* @brief 创建 blob 对象
* @param filename 文件名
* @return 返回文件的哈希值,注意返回的是指针，请手动释放
*/
char* create_blob_object(const char* filename);

void hash_object(const char* filename);

#endif
