cmake_minimum_required(VERSION 3.12)

# 设置项目名称和版本
project(Rotation_LayerAssignment)

# 指定C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 递归地自动包含src目录下所有的.cpp文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 添加可执行文件
add_executable(LayerAssignment ${SOURCES})
target_include_directories(LayerAssignment PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)