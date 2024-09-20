#include <header/dirPro.h>

#ifdef _WIN32

DIR* opendir(const char* name) {
    DIR* dir = (DIR*)malloc(sizeof(DIR));
    if (dir == NULL) {
        return NULL;
    }

    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*", name);

    dir->hFind = FindFirstFile(search_path, &dir->findFileData);
    if (dir->hFind == INVALID_HANDLE_VALUE) {
        free(dir);
        return NULL;
    }

    dir->first = 1;
    return dir;
}

struct dirent* readdir(DIR* dirp) {
    static struct dirent entry;

    if (dirp->first) {
        dirp->first = 0;
    }
    else {
        if (!FindNextFile(dirp->hFind, &dirp->findFileData)) {
            return NULL;
        }
    }

    snprintf(entry.d_name, sizeof(entry.d_name), "%s", dirp->findFileData.cFileName);
    return &entry;
}

int closedir(DIR* dirp) {
    if (dirp == NULL) {
        return -1;
    }

    FindClose(dirp->hFind);
    free(dirp);
    return 0;
}

#endif