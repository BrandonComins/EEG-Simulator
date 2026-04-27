cmake_minimum_required(VERSION 3.28)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(${PROJECT_NAME}-ui 
    main.cpp
    ${SRC_DIR}/mainwindow.cpp
    ${SRC_DIR}/mainwindow.h
    ${SRC_DIR}/mainwindow.ui
    src/connectiondialog.h src/connectiondialog.cpp src/connectiondialog.ui
)

target_link_libraries(${PROJECT_NAME}-ui PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    plot-widget-lib
    Communication-lib
)

target_compile_options(${PROJECT_NAME}-ui PRIVATE -Wall -Wextra)

target_include_directories(${PROJECT_NAME}-ui PRIVATE 
    ${SRC_DIR}
)
