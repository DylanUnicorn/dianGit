#include <header/gettime.h>

// ��ȡ��ǰʱ��
void get_current_time(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}