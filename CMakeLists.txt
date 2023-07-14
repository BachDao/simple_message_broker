cmake_minimum_required(VERSION 3.11)

# vcpkg install & build
include(FetchContent)
FetchContent_Declare(vcpkg
        GIT_REPOSITORY https://github.com/microsoft/vcpkg/
        GIT_TAG 2023.06.20
        )
FetchContent_MakeAvailable(vcpkg)
set(CMAKE_TOOLCHAIN_FILE "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "")

# project config
project(simple_broker VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 20)

# Protobuf
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/protobuf/message.proto)

# Src tree
file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(simple_broker ${SRCS} ${PROTO_SRCS})
target_include_directories(simple_broker
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${PROTO_HDRS})

# 3rd dependencies

# Boost
find_package(Boost REQUIRED)
target_include_directories(simple_broker PUBLIC ${Boost_INCLUDE_DIRS})

#Protobuf
target_link_libraries(simple_broker ${Protobuf_LIBRARIES})
# testing stuff
add_subdirectory(examples)
add_subdirectory(test)