cmake_minimum_required(VERSION 2.8)
project(helloworld C CXX)
#enable_language(CXX)

set(CMAKE_CXX_FLAGS "-g -Wall -w -O3 -fpermissive -std=c++11 -fPIC -fvisibility=hidden")

include_directories(./)

set(SRC_LIST ./helloworld.cpp)
set(TARGET helloworld)

add_library(${TARGET} SHARED ${SRC_LIST})
set_target_properties(${TARGET} PROPERTIES LINKER_LANGUAGE CXX)


