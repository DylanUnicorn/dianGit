#include <header/gitlog.h>
#include <diangit.h>

// 更新.git/logs文件，追加提交记录
void update_commit_log(const char* commit_hash, const char* commit_msg, const char* timestamp) {
    char log_path[256];
    snprintf(log_path, sizeof(log_path), ".git/logs");

    FILE* log_file = fopen(log_path, "a");
    if (!log_file) {
        perror("Failed to open or create logs file");
        return;
    }

    // 追加提交记录
    fprintf(log_file, "Commit Hash: %s\nTime: %s\nMessage: %s\n\n", commit_hash, timestamp, commit_msg);
    fclose(log_file);
}

void display_log() {
    char log_path[256];
    snprintf(log_path, sizeof(log_path), ".git/logs");

    FILE* log_file = fopen(log_path, "r");
    if (!log_file) {
        perror("Failed to open logs file");
        return;
    }

    // 输出日志内容
    char line[256];
    while (fgets(line, sizeof(line), log_file)) {
        printf("%s", line);
    }

    fclose(log_file);
}
