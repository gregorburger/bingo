cmake_minimum_required(VERSION 2.6)

add_subdirectory(3rdparty/qjson)

set(CMAKE_AUTOMOC TRUE)

find_package(Qt4 REQUIRED QtCore QtGui QtSvg QtDeclarative)
include(${QT_USE_FILE})

find_package(Boost REQUIRED COMPONENTS regex)

file(GLOB SRCS *.cpp)
file(GLOB UI_FILES *.ui)

qt4_wrap_ui(UI_SRCS ${UI_FILES})
qt4_add_resources(RCSR_SRCS bingo.qrc)


include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(bingo ${SRCS} ${UI_SRCS} ${RCSR_SRCS})
target_link_libraries(bingo qjson ${QT_LIBRARIES} ${Boost_REGEX_LIBRARY})
