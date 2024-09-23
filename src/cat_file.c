# include <header/cat_file.h>
#include <diangit.h>

// 解压数据
// @parm input 输入数据
// @parm input_len 输入数据长度
// @parm output 输出数据
// @parm output_len 输出数据长度
// @return 0:成功，-1:失败
int decompress_data(const char* input, size_t input_len, char** output, size_t* output_len) {
    z_stream strm;
    int ret;
    *output = malloc(CHUNK);
    if (*output == NULL) {
        perror("Failed to allocate memory");
        return -1;
    }

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    ret = inflateInit(&strm);
    if (ret != Z_OK) {
        return ret;
    }

    strm.avail_in = input_len;
    strm.next_in = (unsigned char*)input;

    *output_len = 0;
    do {
        strm.avail_out = CHUNK;
        strm.next_out = (unsigned char*)*output + *output_len;

        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR) {
			inflateEnd(&strm);
			free(*output);
			return ret;//解压失败，释放内存，报错
        }

		if (strm.avail_out == 0) {
			*output = realloc(*output, *output_len + CHUNK);
			if (*output == NULL) {
				perror("Failed to allocate memory");
				inflateEnd(&strm);
				return -1;
			}
		}

        *output_len += CHUNK - strm.avail_out;
        // *output = realloc(*output, *output_len + CHUNK);

    } while (strm.avail_out == 0);

	(*output)[*output_len] = '\0';

    inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : ret;
}

// 重写一个功能相似的read_compressed_object，只不过只需要传入路径和解压后的文件和文件长度
// @brief 记得手动释放decompressed_data
int read_compressed_object(const char* file, char** decompressed_data, size_t* decompressed_len) {
	FILE* f = fopen(file, "rb");
	if (!f) {
		perror("Failed to open object file");
		return -1;
	}

	fseek(f, 0, SEEK_END);
	size_t file_len = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* compressed_data = malloc(file_len);
	if (compressed_data == NULL) {
		perror("Failed to allocate memory");
		fclose(f);
		return -1;
	}

	fread(compressed_data, 1, file_len, f);
	fclose(f);

	if (decompress_data(compressed_data, file_len, decompressed_data, decompressed_len) == Z_OK) {
        // printf("%s success!!", file);
	}
#ifdef _DEBUG
    else {
        perror("Failed to decompress object\n");
        free(compressed_data);
        return -1;
    }
#endif

	free(compressed_data);
	return 0;
}



// cat-file 功能
void cat_file(const char* hash) {
    char path[256], file[256];
    snprintf(path, sizeof(path), ".git/objects");
    snprintf(file, sizeof(file), "%s/%s", path, hash);

    FILE* f = fopen(file, "rb");
    if (!f) {
        perror("Failed to open object file");
        return;
    }

    fseek(f, 0, SEEK_END);
    size_t file_len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* compressed_data = malloc(file_len);
    fread(compressed_data, 1, file_len, f);
    fclose(f);

    char* decompressed_data = NULL;
    size_t decompressed_len = 0;

    if (decompress_data(compressed_data, file_len, &decompressed_data, &decompressed_len) == Z_OK) {
        printf("Object content: %s\n", decompressed_data + strlen(decompressed_data) + 1);
    }
	else {
		perror("Failed to decompress object\n");
	}

    free(compressed_data);
    free(decompressed_data);
}
