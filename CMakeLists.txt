cmake_minimum_required(VERSION 3.16)
project(bingo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg Qml Quick QuickWidgets PrintSupport)

file(GLOB SRCS *.cpp)
file(GLOB UI_FILES *.ui)

add_executable(bingo ${SRCS} ${UI_FILES} bingo.qrc)

target_include_directories(bingo PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(bingo
    Qt6::Core
    Qt6::Widgets
    Qt6::Svg
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::PrintSupport
)
