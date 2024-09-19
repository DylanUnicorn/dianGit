#ifndef __HASH_OBJECT_H__
#define __HASH_OBJECT_H__

#include "init.h"
char* read_file(const char* filename, size_t* file_len);
void compute_sha1(const char* data, size_t data_len, unsigned char* hash_out);
int compress_data(const char* input, size_t input_len, char** output, size_t* output_len);
void store_object(const char* hash, const char* compressed_data, size_t compressed_len);
void hash_object(const char* filename);

#endif
