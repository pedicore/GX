cmake_minimum_required(VERSION 3.16)

project(GX VERSION 0.5 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/UI_Forms")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(GX 
    src/main.cpp
    src/WebViewManager.cpp
    src/PasswordManager.cpp)


target_include_directories(GX PRIVATE include)
target_link_libraries(GX PRIVATE Qt6::Widgets)








