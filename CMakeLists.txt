cmake_minimum_required(VERSION 3.16)
project(MNRPC)

set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
if(PROTOBUF_FOUND)
    message(STATUS "Protobuf library found")
    message(STATUS "Protobuf include directory: ${PROTOBUF_INCLUDE_DIRS}")
    message(STATUS "Protobuf libraries: ${PROTOBUF_LIBRARIES}")
else()
    message(FATAL_ERROR "Protobuf library not found")
endif()



set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/src/include
        ${PROJECT_SOURCE_DIR}/example
        ${PROTOBUF_INCLUDE_DIRS}
)

link_directories(${PROJECT_SOURCE_DIR}/lib  /usr/local/lib)

add_subdirectory(./src)
add_subdirectory(./test/protobuf)
add_subdirectory(./example)
add_subdirectory(./test/libevent/serve)
add_subdirectory(./test/libevent/client)