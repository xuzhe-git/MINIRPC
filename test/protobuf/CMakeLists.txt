cmake_minimum_required(VERSION 3.16)
project(MNRPC)
set(CMAKE_CXX_STANDARD 17)

# 查找 Protobuf 库
find_package(Protobuf REQUIRED)
if(PROTOBUF_FOUND)
    message(STATUS "Protobuf library found")
    message(STATUS "Protobuf include directory: ${PROTOBUF_INCLUDE_DIRS}")
    message(STATUS "Protobuf libraries: ${PROTOBUF_LIBRARIES}")
else()
    message(FATAL_ERROR "Protobuf library not found")
endif()

# 定义源文件
set(COMMON_SOURCES main.cpp test.pb.cc)

# 定义包含目录
set(COMMON_INCLUDE_DIRS
        /usr/include
        /usr/include/x86_64-linux-gnu
        ${PROTOBUF_INCLUDE_DIRS}  # 添加 Protobuf 包含目录
)

add_executable(test_protobuf ${COMMON_SOURCES})

target_include_directories(test_protobuf PRIVATE
        ${COMMON_INCLUDE_DIRS}
)

target_link_libraries(test_protobuf PRIVATE
        ${PROTOBUF_LIBRARIES}  # 链接 Protobuf 库
)