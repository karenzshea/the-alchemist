cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(alchemy VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)
set(PROTOZERO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/protozero/include")

add_executable(alchemy main.cc)
target_include_directories(alchemy SYSTEM PUBLIC ${PROTOZERO_INCLUDE_DIR})
