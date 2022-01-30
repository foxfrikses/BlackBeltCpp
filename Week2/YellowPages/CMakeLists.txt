cmake_minimum_required(VERSION 3.21)
project(yellow_pages)

set(CMAKE_CXX_STANDARD 20)

find_package(Protobuf REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(
        PROTO_SRCS
        PROTO_HDRS
        protos/address.proto
        protos/company.proto
        protos/name.proto
        protos/phone.proto
        protos/provider.proto
        protos/signal.proto
        protos/url.proto
        protos/working_time.proto
)
add_executable(yellow_pages ${PROTO_SRCS} ${PROTO_HDRS} test.cpp merge.cpp test_runner.h)
target_link_libraries(yellow_pages ${Protobuf_LIBRARIES})
