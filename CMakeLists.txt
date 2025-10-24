cmake_minimum_required(VERSION 3.27)
project(PP3_Proyecto)

set(CMAKE_CXX_STANDARD 17)

add_executable(PP3_Proyecto
        main.cpp
        Archivo.h
        Archivo.cpp
        menu.cpp
        menu.h
        database.h
        database.cpp
        sqlite3.c

)

target_include_directories(PP3_Proyecto PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(unofficial-pdcurses CONFIG REQUIRED)
target_link_libraries(PP3_Proyecto PRIVATE unofficial::pdcurses::pdcurses)