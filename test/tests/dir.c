#include <header/dirPro.h>

int main() {
    const char* path = "E:\\Hust\\Memo\\oneDianDian\\test";
    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}
