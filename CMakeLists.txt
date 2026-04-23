cmake_minimum_required(VERSION 3.28)
project(EEGSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(QWT_ROOT "/usr/local/qwt-6.3.0")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

find_path(QWT_INCLUDE_DIR NAMES qwt_plot.h PATHS ${QWT_ROOT}/include NO_DEFAULT_PATH)
find_library(QWT_LIBRARY NAMES qwt PATHS ${QWT_ROOT}/lib NO_DEFAULT_PATH)

add_subdirectory(simulator)
add_subdirectory(ui)