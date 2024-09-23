#include <header/init.h>
#include <stdio.h>

int main() {

	char dir[100] = "E:/Hust/Memo/oneDianDian/test/.git/index";
	//char dir[100] = "hello/hello.txt";
	FILE* file = NULL;
	if (file_exists(dir)) {
		file = fopen(dir, "r");
		if (file == NULL) {
			printf("文件打开失败\n");
			return 1;
		}
		printf("文件打开成功\n");
		fclose(file);

	}
	else {
		printf("文件不存在\n");
		return 1;
	}
	printf("文件存在\n");
	return 0;
}