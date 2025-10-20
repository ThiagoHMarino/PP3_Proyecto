cmake_minimum_required(VERSION 3.27)
project(PP3_Proyecto)

set(CMAKE_CXX_STANDARD 17)

add_executable(PP3_Proyecto main.cpp
        Archivo.h
        Archivo.cpp
        menu.cpp
        menu.h)

target_link_libraries(PP3_Proyecto ncurses)
