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

# Installation rules
install(TARGETS bingo DESTINATION bin)
install(DIRECTORY imgs DESTINATION bin)

# macOS Bundle configuration
if(APPLE)
    set_target_properties(bingo PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.bingo.app
        MACOSX_BUNDLE_BUNDLE_NAME "Bingo"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    )

    # Install imgs into the bundle
    set_source_files_properties(imgs PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

# CPack configuration for DMG
set(CPACK_PACKAGE_NAME "Bingo")
set(CPACK_PACKAGE_VENDOR "Bingo")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Bingo Application")
set(CPACK_PACKAGE_VERSION "1.0.0")

if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "Bingo")
    set(CPACK_DMG_FORMAT "UDBZ")
endif()

include(CPack)
