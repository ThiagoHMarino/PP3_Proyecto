cmake_minimum_required(VERSION 3.27)
project(PP3_Proyecto)

set(CMAKE_CXX_STANDARD 17)

# Archivos fuente del proyecto
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

# ============================================
# CONFIGURACIÓN MULTIPLATAFORMA PARA CURSES
# ============================================

if(WIN32)
    # Windows: usar PDCurses desde vcpkg
    message(STATUS "Sistema detectado: Windows - usando PDCurses")

    find_package(unofficial-pdcurses CONFIG REQUIRED)
    target_link_libraries(PP3_Proyecto PRIVATE unofficial::pdcurses::pdcurses)

    # Definir macro para compilación condicional
    target_compile_definitions(PP3_Proyecto PRIVATE USE_PDCURSES)

elseif(UNIX AND NOT APPLE)
    # Linux: usar ncurses del sistema
    message(STATUS "Sistema detectado: Linux - usando ncurses")

    find_package(Curses REQUIRED)
    target_include_directories(PP3_Proyecto PRIVATE ${CURSES_INCLUDE_DIR})
    target_link_libraries(PP3_Proyecto PRIVATE ${CURSES_LIBRARIES})

    # Definir macro para compilación condicional
    target_compile_definitions(PP3_Proyecto PRIVATE USE_NCURSES)

elseif(APPLE)
    # macOS: usar ncurses del sistema
    message(STATUS "Sistema detectado: macOS - usando ncurses")

    find_package(Curses REQUIRED)
    target_include_directories(PP3_Proyecto PRIVATE ${CURSES_INCLUDE_DIR})
    target_link_libraries(PP3_Proyecto PRIVATE ${CURSES_LIBRARIES})

    # Definir macro para compilación condicional
    target_compile_definitions(PP3_Proyecto PRIVATE USE_NCURSES)

else()
    message(FATAL_ERROR "Sistema operativo no soportado")
endif()

# ============================================
# CONFIGURACIÓN ADICIONAL
# ============================================

# Opciones de compilación según plataforma
if(MSVC)
    # Visual Studio
    target_compile_options(PP3_Proyecto PRIVATE /W4)
else()
    # GCC/Clang
    target_compile_options(PP3_Proyecto PRIVATE -Wall -Wextra)
endif()