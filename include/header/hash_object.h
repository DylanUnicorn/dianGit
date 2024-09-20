#ifndef __HASH_OBJECT_H__
#define __HASH_OBJECT_H__

#include "init.h"

#define HASH_LEN 20 // SHA-1 ��� 20 �ֽ�
#define CHUNK 16384

char* read_file(const char* filename, size_t* file_len);

void compute_sha1(const char* data, size_t data_len, unsigned char* hash_out);

int compress_data(const char* input, size_t input_len, char** output, size_t* output_len);

// �洢ѹ���� Git ����
void store_object(const char* hash, const char* compressed_data, size_t compressed_len);

void hashData(const char* filename, char* hash_out);
/**
* @brief ���� blob ����
* @param filename �ļ���
* @return �����ļ��Ĺ�ϣֵ,ע�ⷵ�ص���ָ�룬���ֶ��ͷ�
*/
char* create_blob_object(const char* filename);

void hash_object(const char* filename);

#endif
