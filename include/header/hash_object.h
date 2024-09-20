#ifndef __HASH_OBJECT_H__
#define __HASH_OBJECT_H__

#include "init.h"

#define HASH_LEN 20 // SHA-1 输出 20 字节
#define CHUNK 16384

char* read_file(const char* filename, size_t* file_len);

void compute_sha1(const char* data, size_t data_len, unsigned char* hash_out);

int compress_data(const char* input, size_t input_len, char** output, size_t* output_len);

// 存储压缩的 Git 对象
void store_object(const char* hash, const char* compressed_data, size_t compressed_len);

void hashData(const char* filename, char* hash_out);
/**
* @brief 创建 blob 对象
* @param filename 文件名
* @return 返回文件的哈希值,注意返回的是指针，请手动释放
*/
char* create_blob_object(const char* filename);

void hash_object(const char* filename);

#endif
