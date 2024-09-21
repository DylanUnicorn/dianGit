#include <header/dirPro.h>
#include <stdio.h>

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


int copy_file(const char* src, const char* dest) {
#ifdef _WIN32
    if (!CopyFile(src, dest, FALSE)) {
        fprintf(stderr, "Failed to copy file: %lu\n", GetLastError());
        return -1;
    }
#else
    int src_fd = open(src, O_RDONLY);
    if (src_fd < 0) {
        perror("Failed to open source file");
        return -1;
    }

    int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        perror("Failed to open destination file");
        close(src_fd);
        return -1;
    }

    char buffer[4096];
    ssize_t bytes_read, bytes_written;
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Failed to write to destination file");
            close(src_fd);
            close(dest_fd);
            return -1;
        }
    }

    if (bytes_read < 0) {
        perror("Failed to read from source file");
    }

    close(src_fd);
    close(dest_fd);
#endif

    return 0;
}