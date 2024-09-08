cmake_minimum_required(VERSION 3.30.3)
project(main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB SOURCES ${SOURCE_DIR}/*.cpp)
add_executable(main ${SOURCES})

target_include_directories(main PRIVATE include)
target_link_libraries(main PRIVATE SDL2::SDL2 SDL2_ttf::SDL2_ttf OpenSSL::SSL OpenSSL::Crypto)
