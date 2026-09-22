cmake_minimum_required(VERSION 3.16)

project(ThrusterTester VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTORCC ON)

if(NOT DEFINED CMAKE_PREFIX_PATH AND EXISTS "$ENV{HOME}/Qt/6.11.2/macos")
    set(CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/6.11.2/macos")
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Svg SerialPort Charts)

qt_standard_project_setup()

qt_add_executable(appThrusterTester
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/MainWindow.ui
    src/SerialService.cpp
    src/SerialService.h
    src/SerialCommands.h
    src/SerialPortFilter.cpp
    src/SerialPortFilter.h
    src/LineChart.cpp
    src/LineChart.h
    resources/resources.qrc
)

set_target_properties(appThrusterTester PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "Thruster Tester 2.0"
    MACOSX_BUNDLE_GUI_IDENTIFIER "org.lbccrov.thrustertester"
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appThrusterTester PRIVATE src)
target_link_libraries(appThrusterTester PRIVATE Qt6::Widgets Qt6::Svg Qt6::SerialPort Qt6::Charts)
