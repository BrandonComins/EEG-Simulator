include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.28)

set(NAME communication)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(SRC
    ${SRC_DIR}/packettransceiver.cpp
)

set(INC
    ${INC_DIR}/packets.h
    ${INC_DIR}/packettransceiver.h
)

add_library(${NAME}-lib ${SRC} ${INC})

target_include_directories(${NAME}-lib PUBLIC ${INC_DIR})

target_link_libraries(${NAME}-lib PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)