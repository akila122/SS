cmake_minimum_required(VERSION 2.8)
project(ss)

include_directories(src/helpers/h)
include_directories(src/assembler/h)
include_directories(src/emulator/h)

file(GLOB SOURCE_FILES_HELPERS
        src/helpers/h/*.h
        src/helpers/src/*.cpp
        )

file(GLOB SOURCE_FILES_ASSEMBLER
        src/assembler/h/*.h
        src/assembler/src/*.cpp
        )

file(GLOB SOURCE_FILES_EMULATOR
        src/emulator/h/*.h
        src/emulator/src/*.cpp
        )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

add_executable(bin/assembler ${SOURCE_FILES_ASSEMBLER} ${SOURCE_FILES_HELPERS})
add_executable(bin/emulator ${SOURCE_FILES_EMULATOR} ${SOURCE_FILES_HELPERS})
