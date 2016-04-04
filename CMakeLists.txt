cmake_minimum_required(VERSION 3.5)
project(proco)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES main.c include/store.h src/store.c include/log.h src/log.c include/manager.h src/manager.c include/const.h)

add_executable(proco ${SOURCE_FILES})