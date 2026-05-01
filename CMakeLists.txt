cmake_minimum_required(VERSION 3.28)
project(EEGSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries")
set(QWT_ROOT "${LIB_DIR}/qwt-6.3.0")
set(FMT_ROOT "${LIB_DIR}/fmt-12.1.0")
set(SPD_LOG_ROOT "${LIB_DIR}/spdlog-1.17.0")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

find_path(QWT_INCLUDE_DIR NAMES qwt_plot.h PATHS ${QWT_ROOT}/include NO_DEFAULT_PATH)
find_library(QWT_LIBRARY NAMES qwt PATHS ${QWT_ROOT}/lib NO_DEFAULT_PATH)

add_subdirectory(${FMT_ROOT} cmake_fmt)
add_subdirectory(${SPD_LOG_ROOT} cmake_spd_log)
add_subdirectory(simulator)
add_subdirectory(ui)
add_subdirectory(comms)
