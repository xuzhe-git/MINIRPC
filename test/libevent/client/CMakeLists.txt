set(SRC_LIST client.cpp)

add_executable(libevent_client ${SRC_LIST})

target_link_libraries(libevent_client mprpc)