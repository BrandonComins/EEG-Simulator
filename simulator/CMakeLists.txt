include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.28)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SRC
    ${SRC_DIR}/commandparser.cpp
    ${SRC_DIR}/eeg.cpp
)

set(INC
    ${INC_DIR}/commandparser.h
    ${INC_DIR}/eeg.h
)

add_executable(${PROJECT_NAME}-simulator ${SRC} ${INC} main.cpp)

target_include_directories(${PROJECT_NAME}-simulator PRIVATE ${INC_DIR})

target_link_libraries(${PROJECT_NAME}-simulator PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    communication-lib
)

target_compile_options(${PROJECT_NAME}-simulator PRIVATE -Wall -Wextra)