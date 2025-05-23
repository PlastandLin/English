cmake_minimum_required(VERSION 3.14)
project(AlKo)

set(CMAKE_CXX_STANDARD 17)
set(Qt6_DIR "D:/Qt/6.9.0/mingw_64/lib/cmake/Qt6")
find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia Core Gui Network)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB SOURCES
    "*.cpp"
    "grammar/*.cpp"
    "writing/*.cpp"
    "listening/*.cpp"
    "speaking/*.cpp"
    "level/*.cpp"
)

file(GLOB HEADERS
    "*.h"
    "grammar/*.h"
    "writing/*.h"
    "listening/*.h"
    "speaking/*.h"
    "level/*.h"
)

file(GLOB FORMS
    "*.ui"
)

add_executable(English
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
)

target_link_libraries(English
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Core
    Qt6::Gui
    Qt6::Network
)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/listening/audio")
file(COPY "listening/audio/05Meeting a new friend in class--.mp3" DESTINATION "${CMAKE_BINARY_DIR}/listening/audio")
file(COPY "level/images/photo.jpg" DESTINATION "${CMAKE_BINARY_DIR}/level/images")
