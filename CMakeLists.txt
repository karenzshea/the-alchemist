cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(alchemy VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)
set(PROTOZERO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/protozero/include")

FIND_PACKAGE(Boost 1.58 REQUIRED)

add_executable(alchemy-write encode.cc)
add_executable(alchemy-read decode.cc)
target_include_directories(alchemy-write SYSTEM PUBLIC ${PROTOZERO_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
target_include_directories(alchemy-read SYSTEM PUBLIC ${PROTOZERO_INCLUDE_DIR})
