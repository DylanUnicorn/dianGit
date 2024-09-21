cmake_minimum_required(VERSION 3.28)

project(diangit
        VERSION 1.0.1
        DESCRIPTION "A simple git implementation"
        LANGUAGES C
)

# 设置C标准
set(CMAKE_C_STANDARD 99)

# 设置vcpkg路径
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
find_path(DIRENT_INCLUDE_DIRS "dirent.h")

# 包含include目录
include_directories(${PROJECT_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIRS})

# 添加子目录
add_subdirectory(src)
# add_subdirectory(lib/third_party_lib)

# 添加可执行文件
add_executable(diangit src/main.c "include/header/checkout.h" "include/header/mytree.h" "include/header/TagAndBranch.h")

# 链接第三方库
target_link_libraries(diangit PUBLIC my_modules ZLIB::ZLIB ${OPENSSL_LIBRARIES} PRIVATE ${DIRENT_INCLUDE_DIRS})

add_subdirectory(test)
