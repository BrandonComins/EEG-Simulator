cmake_minimum_required(VERSION 3.28)
project(EEGSimulator VERSION 1.0 LANGUAGES CXX)

# ----Tell Gtest To Ignore C Compiler----
set(CMAKE_C_COMPILER ${CMAKE_CXX_COMPILER})
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_C_ABI_COMPILED 1)
set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
# ----Tell Gtest To Ignore C Compiler----

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries")
set(QWT_ROOT "${LIB_DIR}/qwt-6.3.0")
set(FMT_ROOT "${LIB_DIR}/fmt-12.1.0")
set(SPD_LOG_ROOT "${LIB_DIR}/spdlog-1.17.0")
set(GTEST_ROOT "${LIB_DIR}/googletest-1.17.0")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Test)

find_path(QWT_INCLUDE_DIR NAMES qwt_plot.h PATHS ${QWT_ROOT}/include NO_DEFAULT_PATH)
find_library(QWT_LIBRARY NAMES qwt PATHS ${QWT_ROOT}/lib NO_DEFAULT_PATH)

add_subdirectory(${FMT_ROOT} cmake_fmt)
add_subdirectory(${SPD_LOG_ROOT} cmake_spd_log)
add_subdirectory(${GTEST_ROOT} cmake_gtest)

add_subdirectory(simulator)
add_subdirectory(ui)
add_subdirectory(comms)
