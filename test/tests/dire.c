#include <header/init.h>
#include <header/hash_object.h>
#include <stdio.h>

//����create_directory����

int main()
{	// ����Ŀ¼
	//create_directory("E:\\Hust\\Memo\\oneDianDian\\hello");
	// ��ǰ��ִ���ļ�����Ŀ¼
	//create_directory("./world");
	// ����һ��HashData
	char hush[41];
	// hashData("hello/hello.txt",hush);
	if (hashData("E:/Hust/Memo/oneDianDian/test/deepTask/adding.cpp", hush) == NULL)
	{
		printf("�ļ�������\n");
		return 1;
	}
	printf("%s\n", hush);
	return 0;
}