# 添加源文件
set(SOURCES
    init.c
    hash_object.c
    cat_file.c
    gettime.c
    dirPro.c
    commit.c
    gitlog.c
    checkout.c
    mytree.c
    ls_tree.c
    TagAndBranch.c
)

# 创建静态库
add_library(my_modules STATIC ${SOURCES})

# 链接头文件目录
target_include_directories(my_modules PUBLIC ${PROJECT_SOURCE_DIR}/include)

# target_link_libraries(main PUBLIC my_modules)
