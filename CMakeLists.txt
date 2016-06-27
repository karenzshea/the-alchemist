cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(alchemy VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")


set(PROTOZERO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/protozero/include")

find_package(Boost 1.54 REQUIRED)

add_executable(alchemy-write encode.cc)
add_executable(alchemy-read decode.cc)
target_include_directories(alchemy-write SYSTEM PRIVATE ${PROTOZERO_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
target_include_directories(alchemy-read SYSTEM PRIVATE ${PROTOZERO_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
