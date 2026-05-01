include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.28)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SRC
    ${SRC_DIR}/commandparser.cpp
    ${SRC_DIR}/eeg.cpp
    ${SRC_DIR}/commandparser.h
    ${SRC_DIR}/eeg.h
    ${SRC_DIR}/settings_defs.h
)

add_executable(${PROJECT_NAME}-simulator ${SRC} main.cpp)

target_include_directories(${PROJECT_NAME}-simulator PRIVATE ${SRC_DIR})

target_link_libraries(${PROJECT_NAME}-simulator PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    communication-lib
    fmt
)

target_compile_options(${PROJECT_NAME}-simulator PRIVATE -Wall -Wextra)