set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(NAME plot-widget)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

add_library(${NAME}-lib STATIC
    plotwidget.cpp
    plotwidget.h
    plotwidget.ui
    plothelper.cpp
    plothelper.h
)

qt_add_resources(${NAME}-lib "styles"
    PREFIX "/ui"
    FILES
        "resources/modern.qss"
)

target_include_directories(${NAME}-lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QWT_INCLUDE_DIR}
)

target_link_libraries(${NAME}-lib PUBLIC
    Qt6::Widgets
    ${QWT_LIBRARY}
)

target_compile_options(${NAME}-lib PRIVATE -Wall -Wextra)

add_executable(${NAME}-runner
    ${TEST_DIR}/plotwidget_runner.cpp
)

target_link_libraries(${NAME}-runner PRIVATE
    ${NAME}-lib
)
