# include <header/cat_file.h>

// 解压数据
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
