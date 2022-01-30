cmake_minimum_required(VERSION 3.21)
project(PhoneBook)

set(CMAKE_CXX_STANDARD 20)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS contact.proto)
add_executable(PhoneBook main.cpp ${PROTO_SRCS} ${PROTO_HDRS}
        phone_book.h
        iterator_range.h
        phone_book.cpp
        string_stringview_equal.h
        string_stringview_hash.h)
target_link_libraries(PhoneBook ${Protobuf_LIBRARIES})
