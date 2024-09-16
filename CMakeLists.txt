cmake_minimum_required(VERSION 3.28)

project(DianGit
		VERSION 1.0.1
		DESCRIPTION "A simple git implementation"
		LANGUAGES C
)

set(CMAKE_C_STANDARD 99)

#  包含头文件目录
include_directories(include)


add_executable(DianGit src/main.c src/init.c)
