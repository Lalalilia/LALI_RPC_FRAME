cmake_minimum_required(VERSION 3.10.2)

project(ziliProject)

set(CMAKE_CXX_STANDARD 17)

include_directories(".")

aux_source_directory(./base DIR_SRCS)
aux_source_directory(./net DIR_SRCS)
aux_source_directory(./jackson DIR_SRCS)
aux_source_directory(./zrpc/stub DIR_SRCS)
aux_source_directory(./zrpc/client DIR_SRCS)
aux_source_directory(./zrpc/server DIR_SRCS)

add_executable(
        stub_test
        test/stub_test.cpp
        ${DIR_SRCS}
)

add_executable(
        RpcServer_test
        test/RpcServer_test.cpp
        ${DIR_SRCS}
)
add_executable(
        RpcClient_test
        test/RpcClient_test.cpp
        ${DIR_SRCS}
)
add_executable(
        Http_test
        test/http_test.cpp
        ${DIR_SRCS}
)

find_package(Threads REQUIRED)
target_link_libraries (stub_test Threads::Threads)
target_link_libraries (RpcServer_test Threads::Threads)
target_link_libraries (RpcClient_test Threads::Threads)
target_link_libraries (Http_test Threads::Threads)
