include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.28)

set(NAME communication)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

set(SRC
    ${SRC_DIR}/packettransceiver.cpp
    ${SRC_DIR}/packets.h
    ${SRC_DIR}/packettransceiver.h
)

add_library(${NAME}-lib ${SRC} ${INC})

target_include_directories(${NAME}-lib PUBLIC ${SRC_DIR})

target_link_libraries(${NAME}-lib PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    fmt
)

add_executable(${NAME}-MWE ${TEST_DIR}/main.cpp)
target_include_directories(${NAME}-MWE PUBLIC ${TEST_DIR})
target_link_libraries(${NAME}-MWE PUBLIC ${NAME}-lib)
target_compile_options(${NAME}-MWE PRIVATE -Wall -Wextra)

add_executable(${NAME}-test ${TEST_DIR}/packettransceiver__test.cpp)
target_include_directories(${NAME}-test PUBLIC ${TEST_DIR})
target_link_libraries(${NAME}-test PUBLIC
    ${NAME}-lib
    Qt${QT_VERSION_MAJOR}::Test
    gtest_main
)
target_compile_options(${NAME}-test PRIVATE -Wall -Wextra)