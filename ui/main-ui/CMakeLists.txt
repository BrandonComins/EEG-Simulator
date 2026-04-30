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
    ${SRC_DIR}/connectiondialog.h
    ${SRC_DIR}/connectiondialog.cpp
    ${SRC_DIR}/connectiondialog.ui
    ${SRC_DIR}/controltab.cpp
    ${SRC_DIR}/controltab.h
    ${SRC_DIR}/controltab.ui
    ${SRC_DIR}/controlwidget.cpp
    ${SRC_DIR}/controlwidget.h
    ${SRC_DIR}/controlwidget.ui
    ${SRC_DIR}/packetparser.cpp
    ${SRC_DIR}/packetparser.h
)

target_link_libraries(${PROJECT_NAME}-ui PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    plot-widget-lib
    communication-lib
)

target_compile_options(${PROJECT_NAME}-ui PRIVATE -Wall -Wextra)

target_include_directories(${PROJECT_NAME}-ui PRIVATE 
    ${SRC_DIR}
)
