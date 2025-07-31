cmake_minimum_required(VERSION 3.10)
project(RISC_V_CPU_Simulator)

set(CMAKE_CXX_STANDARD 17)

# 包含头文件目录
include_directories(Headers)

# 收集所有头文件和源文件（这里只需要 main.cpp）
add_executable(code main.cpp)
