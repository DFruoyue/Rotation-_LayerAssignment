cmake_minimum_required(VERSION 3.12)

# 设置项目名称和版本
project(LayerAssignment)

# 指定C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 递归地自动包含src目录下所有的.cpp文件
file(GLOB_RECURSE ObjectSources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM ObjectSources "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

file(GLOB_RECURSE TestSources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM TestSources "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

message("ObjectSources: ${ObjectSources}")
message("TestSources: ${TestSources}")

# 添加头文件搜索路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/db
)

# 添加可执行文件
add_executable(LayerAssignment ${ObjectSources})
add_executable(Test ${TestSources})