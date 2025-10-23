cmake_minimum_required(VERSION 3.27)
project(PP3_Proyecto)

set(CMAKE_CXX_STANDARD 17)

add_executable(PP3_Proyecto main.cpp
        Archivo.h
        Archivo.cpp
        menu.cpp
        menu.h
        database.h
        database.cpp
        main.cpp
)

# Encontrar SQLite3 para la BD
find_package(SQLite3 REQUIRED)

# Vincular librerías con la misma sintaxis (todas con PRIVATE)
target_link_libraries(PP3_Proyecto
        PRIVATE
        ncurses
        SQLite::SQLite3
)

