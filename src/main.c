#include <stdio.h>
#include <string.h>
#include <main.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <command> [<args>]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        if (argc < 3) {
            printf("Usage: %s init <dir>\n", argv[0]);
            return 1;
        }
        init_repository(argv[2]);
	}
	else if (strcmp(argv[1], "hash-object") == 0) {
		if (argc < 3) {
			printf("Usage: %s hash-object <file>\n", argv[0]);
			return 1;
		}
		hash_object(argv[2]);
	}
    else if (strcmp(argv[1], "cat-file") == 0) {
        if (argc < 3) {
            printf("Usage: %s cat-file <hash>\n", argv[0]);
            return 1;
        }
        cat_file(argv[2]);
    }
	else if (strcmp(argv[1], "commit") == 0) {
		if (argc < 3) {
			printf("Usage: %s commit <message>\n", argv[0]);
			return 1;
		}
		commit_all(argv[2]);
	}
	else if (strcmp(argv[1], "log") == 0) {
		if (argc > 2) {
			printf("Usage: %s log\n", argv[0]);
			return 1;
		}
        display_log();
	}
	else if (strcmp(argv[1], "checkout") == 0) {
		if (argv[2] == NULL) {
			printf("Usage: %s checkout <hash>\n", argv[0]);
			return 1;
		}
		checkout(argv[2]);
	}
	else if (strcmp(argv[1], "ls-tree") == 0) {
		if (argv[2] == NULL) {
			printf("Usage: %s ls-tree <hash>\n", argv[0]);
			return 1;
		}
		ls_tree(argv[2]);
	}
    else {
        printf("Unknown command: %s\n", argv[1]);
    }

    return 0;
}


