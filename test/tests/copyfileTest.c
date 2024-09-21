#include <header/dirPro.h>

int main() {
    const char* src = "G:\\Txt\\Txt\\Mouse_bite\\01_mouse_bite_01.txt";
    const char* dest = "D:\\hello.txt";

    if (copy_file(src, dest) == 0) {
        printf("File copied successfully.\n");
    }
    else {
        printf("Failed to copy file.\n");
    }

    return 0;
}
