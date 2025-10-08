// cabeceras.cpp

#include <ncurses.h>
#include <string>
#include "menu.h"

void funcion_menu(){
    initscr();              // Inicia el modo ncurses
    cbreak();               // Desactiva el buffer de línea
    noecho();               // No muestra los caracteres ingresados
    keypad(stdscr, TRUE);   // Habilita las teclas especiales (flechas, F1, etc.)

    const char *opciones[] = {
        "Nuevo",
        "Abrir",
        "Guardar",
        "Salir"
    };
    int n_opciones = sizeof(opciones) / sizeof(opciones[0]);
    int seleccion = 0;

    while (true) {
        clear();
        mvprintw(0, 0, "Usa las flechas para navegar y Enter para seleccionar:");
        for (int i = 0; i < n_opciones; ++i) {
            if (i == seleccion) {
                attron(A_REVERSE); // Resalta la opción seleccionada
            }
            mvprintw(i + 2, 2,"%s", opciones[i]);
            if (i == seleccion) {
                attroff(A_REVERSE);
            }
        }
        int ch = getch();
        switch (ch) {
            case KEY_UP:
                seleccion = (seleccion - 1 + n_opciones) % n_opciones;
                break;
            case KEY_DOWN:
                seleccion = (seleccion + 1) % n_opciones;
                break;
            case '\n': // Enter
                if (seleccion == n_opciones - 1) { // Si es "Salir"
                    endwin();
                    return;
                } else {
                    clear();
                    mvprintw(0, 0, "Has seleccionado: %s", opciones[seleccion]);
                    mvprintw(2, 0, "Presiona cualquier tecla para continuar...");
                    getch();
                }
                break;
        }
    }

    endwin(); // Finaliza el modo ncurses
}