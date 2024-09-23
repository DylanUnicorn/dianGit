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
	else if ((strcmp(argv[1], "commit-now") == 0) && (strcmp(argv[2], "-m") == 0)) {
		if (argc < 4) {
			printf("Usage: %s commit-now -m <message>\n", argv[0]);
			return 1;
		}
		commit_all(argv[3]);
	}
	else if ((strcmp(argv[1], "commit") == 0) && (strcmp(argv[2], "-m") == 0)) {
		if (argc < 4) {
			printf("Usage: %s commit -m <message>\n", argv[0]);
			return 1;
		}
		commit_stash(argv[3]);
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
	else if (strcmp(argv[1], "branch") == 0) {
		if (argv[2] == NULL) {
			list_branches();
		}
		else {
			create_branch(argv[2]);
		}
	}
	else if (strcmp(argv[1], "tag") == 0) {
		if (argv[2] == NULL) {
			list_tags();
		}
		else {
			create_tag(argv[2]);
		}
	}
	else if (strcmp(argv[1], "add") == 0) {
		if (argc < 3) {
			printf("Usage: %s add <file>\n", argv[0]);
			return 1;
		}
		add_file_to_index(argv[2]);
	}
	else if (strcmp(argv[1], "status") == 0) {
		if (argc > 2) {
			printf("Usage: %s status\n", argv[0]);
			return 1;
		}
		status();
	}
	else if (strcmp(argv[1], "rm") == 0) {
		if (argc < 3) {
			printf("Usage: %s rm <file>\n", argv[0]);
			return 1;
		}
		remove_file_from_index(argv[2]);
	}
	else if (strcmp(argv[1], "ls-files") == 0) {
			int verbose = 0;
			if (argc == 3 && strcmp(argv[2], "--verbose") == 0) {
				verbose = 1;
			}
			else if (argc > 3) {
				printf("Usage: %s ls-files (--verbose)\n", argv[0]);
				return 1;
			}
			list_staged_files(verbose);
	}
	else if (strcmp(argv[1], "check-ignore") == 0) {
		if (argc < 3) {
			printf("Usage: %s check-ignore <file>\n", argv[0]);
			return 1;
		}
		checkignore(argv[2]);
	}
    else {
        printf("Unknown command: %s\n", argv[1]);
    }

    return 0;
}


