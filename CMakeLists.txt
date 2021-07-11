
# You are free to modify this file

cmake_minimum_required(VERSION 3.0)
project(game)
set(CMAKE_CXX_STANDARD 17)
find_package(X11 REQUIRED)
set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
endif()

file(GLOB SRC *.cpp)
add_executable(game ${SRC})
target_link_libraries(game m X11)
