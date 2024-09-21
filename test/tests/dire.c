#include <header/init.h>
#include <header/hash_object.h>
#include <stdio.h>

//测试create_directory函数

int main()
{	// 任意目录
	//create_directory("E:\\Hust\\Memo\\oneDianDian\\hello");
	// 当前可执行文件所在目录
	//create_directory("./world");
	// 测试一下HashData
	char hush[41];
	// hashData("hello/hello.txt",hush);
	if (hashData("E:/Hust/Memo/oneDianDian/test/deepTask/adding.cpp", hush) == NULL)
	{
		printf("文件不存在\n");
		return 1;
	}
	printf("%s\n", hush);
	return 0;
}