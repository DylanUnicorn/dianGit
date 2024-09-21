#ifndef __CAT_FILE_H__
#define __CAT_FILE_H__

#include <header/hash_object.h>
#include <openssl/sha.h>
#include <zlib.h>

// @brief 读取压缩的对象并解压缩
// @param input 输入数据
// @param input_len 输入数据长度
// @param output 输出数据
// @param output_len 输出数据长度
// @return 0:成功，-1:失败
int decompress_data(const char* input, size_t input_len, char** output, size_t* output_len);

// 重写一个和cat_file功能相似的read_compressed_object，只不过只需要传入路径和解压后的文件和文件长度
// @param file 文件路径
// @param decompressed_data 解压后的文件
// @param decompressed_len 解压后的文件长度	
// @return 0:成功，-1:失败
int read_compressed_object(const char* file, char** decompressed_data, size_t* decompressed_len);

// 读取文件并返回其内容
void cat_file(const char* hash);


#endif