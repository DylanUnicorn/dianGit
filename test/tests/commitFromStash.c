#include <header/staging.h>
#include <stddef.h>
#include <stdio.h>

int main()
{
	// ����һ��stash
	// stash();
	// ����һ��commitFromStash
	const char* dir = "E:/Hust/Memo/oneDianDian/test/";
	char index_file_path[256];
	snprintf(index_file_path, sizeof(index_file_path), "%s.git/index", dir);
	struct IndexEntry entries[20];
	size_t entries_len = 0;

	entries_len = read_index_file(index_file_path, entries, 20);

	for (size_t i = 0; i < entries_len; i++) {
		printf("entry: %s hash: %s\n", entries[i].file_path, entries[i].hash);
	}


	return 0;
}