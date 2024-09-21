#ifndef _INIT_H_
#define _INIT_H_

#include <stddef.h>
// 创建文件并写入内容
// @param path 文件路径
// @param content 要写入的内容-文本格式
void create_file(const char* path, const char* content);

// 由于w模式无法满足创建二进制文件的需求，增加一个函数，使用wb模式
// 覆盖写入，要求完整写入，否则返回失败
// 创建文件并写入二进制内容
// @param path 文件路径
// @param data 要写入的数据
// @param len 数据长度
// @return 0 成功 -1 失败
int create_binary_file(const char* path, const void* data, size_t len);

// 检查目录是否存在
int directory_exists(const char* dir);

//创建目录并确保所有父目录都存在
int create_directory(const char* dir);

// 初始化仓库
void init_repository(const char* dir);

#endif