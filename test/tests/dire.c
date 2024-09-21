#include <header/init.h>

//测试create_directory函数

int main()
{	// 任意目录
	create_directory("E:\\Hust\\Memo\\oneDianDian\\hello");
	// 当前可执行文件所在目录
	create_directory("./world");
	return 0;
}