#include <header/init.h>
#include <stdio.h>

int main() {

	char dir[100] = "E:/Hust/Memo/oneDianDian/test/.git/index";
	//char dir[100] = "hello/hello.txt";
	FILE* file = NULL;
	if (file_exists(dir)) {
		file = fopen(dir, "r");
		if (file == NULL) {
			printf("�ļ���ʧ��\n");
			return 1;
		}
		printf("�ļ��򿪳ɹ�\n");
		fclose(file);

	}
	else {
		printf("�ļ�������\n");
		return 1;
	}
	printf("�ļ�����\n");
	return 0;
}