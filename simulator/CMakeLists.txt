include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.28)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(SRC
    eegsimulator.cpp
)

set(INC
    eegsimulator.h
)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_executable(${PROJECT_NAME}-simulator ${SRC} ${INC} main.cpp)

target_include_directories(${PROJECT_NAME}-simulator PRIVATE ${INC_DIR})

target_link_libraries(${PROJECT_NAME}-simulator PRIVATE)