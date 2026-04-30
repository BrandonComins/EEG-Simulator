set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(NAME plot-widget)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(${NAME}-lib STATIC
    ${SRC_DIR}/multiseriestracker.cpp
    ${SRC_DIR}/multiseriestracker.h
    ${SRC_DIR}/plothelper.cpp
    ${SRC_DIR}/plothelper.h
    ${SRC_DIR}/scaledialog.cpp
    ${SRC_DIR}/scaledialog.h
    ${SRC_DIR}/scaledialog.ui
    ${SRC_DIR}/stylecurvediag.cpp
    ${SRC_DIR}/stylecurvediag.h
    ${SRC_DIR}/stylecurvediag.ui
    ${SRC_DIR}/plotdefs.h
)

qt_add_resources(${NAME}-lib "styles"
    PREFIX "/ui"
    FILES
        "resources/modern.qss"
)

target_include_directories(${NAME}-lib PUBLIC
    ${SRC_DIR}
    ${QWT_INCLUDE_DIR}
)

target_link_libraries(${NAME}-lib PUBLIC
    Qt${Qt_VERSION_MAJOR}::Widgets
    ${QWT_LIBRARY}
)

target_compile_options(${NAME}-lib PRIVATE -Wall -Wextra)

add_executable(${NAME}-runner
    ${TEST_DIR}/plotwidget_runner.cpp
)

target_link_libraries(${NAME}-runner PRIVATE
    ${NAME}-lib
)
