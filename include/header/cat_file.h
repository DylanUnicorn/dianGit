#ifndef __CAT_FILE_H__
#define __CAT_FILE_H__

#include <header/hash_object.h>
#include <openssl/sha.h>
#include <zlib.h>
int decompress_data(const char* input, size_t input_len, char** output, size_t* output_len);
void cat_file(const char* hash);


#endif