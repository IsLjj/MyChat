#the lowest version of cmake
cmake_minimum_required(VERSION 3.0)
#project name
project(Chat)
#set compile options
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
#set the save path of executable file
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the search path of head file
#include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/MySQL)
add_subdirectory(src)
