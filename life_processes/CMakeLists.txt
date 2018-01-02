cmake_minimum_required(VERSION 3.7)
project(L)
set(CMAKE_C_STANDARD 11)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread -lrt")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lrt")

set(SOURCE_FILES live.c)

add_executable(L ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(L ${Boost_LIBRARIES} rt)
