#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <zlib.h>
#include <header/hash_object.h>
#include <diangit.h>

#define HASH_LEN 20 // SHA-1 输出 20 字节
#define CHUNK 16384

// 读取文件并返回其内容
char* read_file(const char* filename, size_t* file_len) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *file_len = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = malloc(*file_len + 1);
    if (!content) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    fread(content, 1, *file_len, file);
    content[*file_len] = '\0';
    fclose(file);
    return content;
}

// 计算 SHA-1 哈希值
void compute_sha1(const char* data, size_t data_len, unsigned char* hash_out) {
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, data, data_len);
    SHA1_Final(hash_out, &ctx);
}

// 压缩文件内容
int compress_data(const char* input, size_t input_len, char** output, size_t* output_len) {
    z_stream strm;
    int ret;
    *output_len = compressBound(input_len); // 获取压缩后最大可能的大小
    *output = malloc(*output_len);

    if (*output == NULL) {
        perror("Failed to allocate memory");
        return -1;
    }

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK) {
        return ret;
    }

    strm.avail_in = input_len;
    strm.next_in = (unsigned char*)input;
    strm.avail_out = *output_len;
    strm.next_out = (unsigned char*)*output;

    ret = deflate(&strm, Z_FINISH);
    if (ret != Z_STREAM_END) {
        deflateEnd(&strm);
        return ret == Z_OK ? Z_BUF_ERROR : ret;
    }

    *output_len = *output_len - strm.avail_out;
    deflateEnd(&strm);

    return Z_OK;
}

// 存储压缩的 Git 对象
void store_object(const char* hash, const char* compressed_data, size_t compressed_len) {
    char path[256], dir[256], file[256];
    snprintf(dir, sizeof(dir), ".git/objects");
    snprintf(file, sizeof(file), "%s/%s", dir, hash);

    // 创建目录
    if (create_directory(dir) != 0 && errno != EEXIST) {
        perror("Failed to create object directory");
        return;
    }

    // 创建对象文件
    create_file(file, compressed_data);
    //FILE* f = fopen(file, "wb");
    //if (!f) {
    //    perror("Failed to create object file");
    //    return;
    //}

    //// fwrite(compressed_data, 1, compressed_len, f);
    //if (compressed_data)
    //    fput(compressed_data, f);
    //fclose(f);
    printf("Object stored: %s\n", file);
}

// hash-object 功能
void hash_object(const char* filename) {
    size_t file_len;
    char* file_content = read_file(filename, &file_len);
    if (!file_content) {
        return;
    }

    // 计算哈希值
    char header[64];
    snprintf(header, sizeof(header), "blob %zu", file_len);
    size_t header_len = strlen(header) + 1;

    size_t total_len = header_len + file_len;
    char* data = malloc(total_len);
    memcpy(data, header, header_len);
    memcpy(data + header_len, file_content, file_len);

    unsigned char hash[HASH_LEN];
    compute_sha1(data, total_len, hash);

    // 输出哈希值
    char hash_str[41];
    for (int i = 0; i < HASH_LEN; ++i) {
        snprintf(hash_str + i * 2, 3, "%02x", hash[i]);
    }
    printf("SHA-1 hash: %s\n", hash_str);

    // 压缩数据
    char* compressed_data = NULL;
    size_t compressed_len = 0;
    if (compress_data(data, total_len, &compressed_data, &compressed_len) == Z_OK) {
        store_object(hash_str, compressed_data, compressed_len);
    }

    free(file_content);
    free(data);
    free(compressed_data);
}
