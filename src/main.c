#include <stdio.h>
#include <string.h>
#include "diangit.h"

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
    else {
        printf("Unknown command: %s\n", argv[1]);
    }

    return 0;
}
