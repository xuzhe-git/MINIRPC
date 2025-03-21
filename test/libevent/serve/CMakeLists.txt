set(SRC_LIST serve.cpp)

add_executable(libevent_serve ${SRC_LIST})

target_link_libraries(libevent_serve event mprpc ${PROTOBUF_LIBRARIES} )