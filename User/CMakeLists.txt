cmake_minimum_required(VERSION 3.22)

project(UserModule)
add_library(UserModule INTERFACE)

# 启用 C++、C 与 ASM 语言支持
enable_language(CXX C ASM)

# 添加用户模块需要的编译选项或宏定义（可以根据需要修改或添加）
target_compile_definitions(UserModule INTERFACE
    USER_FEATURE
    $<$<CONFIG:Debug>:DEBUG>
)

# 设置用户模块的头文件搜索路径（假设用户自己的头文件在 Inc 目录下）
target_include_directories(UserModule INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

# 自动搜索当前目录 Src 下及其所有子目录中的 .cpp, .c 以及汇编(.s和.S) 文件
file(GLOB_RECURSE USER_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.cpp")
file(GLOB_RECURSE USER_C_FILES   "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.c")
file(GLOB_RECURSE USER_ASM_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.s" "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.S")

# 把搜索到的文件添加到 UserModule 目标
target_sources(UserModule
    INTERFACE
    ${USER_CPP_FILES}
    ${USER_C_FILES}
    ${USER_ASM_FILES}
)

# 如有需要，可在此设置链接目录
target_link_directories(UserModule INTERFACE
    # 例如 ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

# 确保使用的 C++ 标准满足要求
if(CMAKE_CXX_STANDARD LESS 11)
    message(ERROR "User module requires C++11 or higher")
endif() 