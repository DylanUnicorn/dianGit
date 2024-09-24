#include <header/hash_object.h>
#include <header/cat_file.h>
#include <header/commit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ʹ��cat_file��ȡ�ļ�����

// ��дһ���򻯰��create_tree_object��ֻ��Ҫ����·��������tree����Ĺ�ϣֵ,����Ҫ�洢

// �򻯰��get_tree_hash_from_commit
char* get_tree_hash_from_commit(const char* commit_hash) {
	// ��ȡ commit ����ʹ��֮ǰ�� read_compressed_object ����
	char commit_file_path[256];
	char initdir[256] = "E:/Hust/Memo/oneDianDian/test/";
	snprintf(commit_file_path, sizeof(commit_file_path), "%s.git/objects/%s", initdir, commit_hash);

	char* commit_data = NULL;
	size_t commit_len = 0;
	// C���Ժ��ر�ĵط�
	if (read_compressed_object(commit_file_path, &commit_data, &commit_len) == -1) {
		printf("Failed to read commit object.\n");
		return NULL;
	}

	printf("commit_data: %s\n", commit_data);

	// ���� commit �����ҵ� tree ����Ĺ�ϣֵ
	char* tree_hash = strstr(commit_data, "tree ");
	if (!tree_hash) {
		printf("Invalid commit object.\n");
		free(commit_data);
		return NULL;
	}

	tree_hash += 5;
	char* end = strchr(tree_hash, '\n');
	if (!end) {
		printf("Invalid commit object.\n");
		free(commit_data);
		return NULL;
	}

	*end = '\0';
	char* result = strdup(tree_hash);
	free(commit_data);
	return result;
}

int main()
{
	//char* commit_data = NULL;
	//size_t data_len = 0;   
	//read_compressed_object("E:\\Hust\\Memo\\oneDianDian\\test\\.git\\objects\\b8c6334d68298940469732269e555e5138d5ba5e", &commit_data, &data_len);
	//read_compressed_object("E:/Hust/Memo/oneDianDian/test/.git/objects/269af8b43d3d964dd14e7dddc95ac0539046fcf7", &commit_data, &data_len);
	//printf("commit_data: %s\n", commit_data);
	//char commit_hash[41] = "f3edc2b0443321419289c41f8e39409fd0e90014";
	//char* tree_hash = get_tree_hash_from_commit(commit_hash);
	//printf("tree_hash: %s\n", tree_hash);
	//free(commit_data);
	//free(tree_hash);
	char* object_data = NULL;
	size_t object_len = 0;
	read_compressed_object("E:/Hust/Memo/oneDianDian/test/.git/objects/690dec27c4a7d4f48a4dd2b84944382e2b8a31df", &object_data, &object_len);
	printf("commit_data: %s\n", object_data);
	return 0;
}