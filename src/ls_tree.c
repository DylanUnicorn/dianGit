#include <header/ls_tree.h>
#include <header/commit.h>
#include <header/checkout.h>
#include <diangit.h>
#include <header/cat_file.h>


// 显示 tree 对象内容,
void display_tree(const char* tree_hash) {

    char tree_file_path[256];
    snprintf(tree_file_path, sizeof(tree_file_path), ".git/objects/%s",tree_hash);


    char* tree_data = NULL;
    size_t tree_len = 0;
    if (read_compressed_object(tree_file_path, &tree_data, &tree_len) == -1) {
        printf("Failed to read tree object.\n");
        return;
    }

    char* line = strtok(tree_data, "\n");
    char* next_line = NULL;
    while (line) {
        char mode[7], type[5], hash[41], filename[1024];
        
        if (sscanf(line, "%s %s %40s\t%s", mode, type, hash, filename) != 4) {
			printf("sscanf error in display_tree function\n");
			free(tree_data);
			break;
        }
        printf("%s %s %s %s\n", mode, type, hash, filename);

		next_line = strtok(NULL, "\n");

        if (strcmp(type, "tree") == 0) {
            printf("hhhhh\n");
            // 递归显示子目录
            display_tree(hash);
            printf("wwwwwww\n");
            line = next_line;
			continue;
        }

        line = next_line;
    }
    free(tree_data);
}


// 执行 ls-tree
void ls_tree(const char* commit_hash) {
    char* tree_hash = get_tree_hash_from_commit(commit_hash);
	printf("tree_hash: %s\n", tree_hash);
    if (tree_hash) {
        printf("Tree for commit %s:\n", commit_hash);
        display_tree(tree_hash);
        free(tree_hash);
    }
    else {
        printf("Failed to find tree for commit %s.\n", commit_hash);
    }
}