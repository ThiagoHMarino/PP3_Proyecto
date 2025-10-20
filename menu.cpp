#include <ncurses.h>
#include <string>
#include <iostream>

using namespace std;

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

    const char* confirmacion[] = {
        "Si",
        "No"
    };

    int n_opciones = sizeof(opciones) / sizeof(opciones[0]);
    int seleccion = 0;

    int n_opciones2 = sizeof(confirmacion) / sizeof(confirmacion[0]);
    int seleccion2 = 0;

    //bool terminate = false;
    bool terminate2 = false;

    while (true) {
        clear();
        mvprintw(0, 0, "Usa las flechas para navegar y Enter para seleccionar:");
        for (int i = 0; i < n_opciones; ++i) {
            if (i == seleccion) {
                attron(A_REVERSE); // Resalta la opción seleccionada
            }
            mvprintw(i + 2, 2, "%s", opciones[i]);
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
                while (true){
                    clear();
                    mvprintw(0, 0, "esperando confirmacion ");
                    for (int i = 0; i < n_opciones2; ++i) {
                        if (i == seleccion2) {
                            attron(A_REVERSE); // Resalta la opción seleccionada
                        }
                        mvprintw(i + 2, 2, "%s", confirmacion[i]);
                        if (i == seleccion2) {
                            attroff(A_REVERSE);
                        }
                    }
                    int ch2 = getch();
                    switch(ch2){
                        case KEY_UP:
                            seleccion2 = (seleccion2 - 1 + n_opciones2) % n_opciones2;
                            break;
                        case KEY_DOWN:
                            seleccion2 = (seleccion2 + 1) % n_opciones2;
                            break;
                        case '\n': // Enter
                            terminate2 = true;
                            break;
                    }
                    if (terminate2){
                        terminate2 = false;
                        break;}
                }
                if (seleccion2 == 1){
                    continue;
                }else{
                    switch(seleccion){
                        case 0:
                            clear();
                            mvprintw(0, 0, "Has seleccionado: %s", opciones[seleccion]);
                            mvprintw(2, 0, "Presiona cualquier tecla para continuar...");
                            getch();
                            break;
                        case 1:
                            clear();
                            mvprintw(0, 0, "Has seleccionado: %s", opciones[seleccion]);
                            mvprintw(2, 0, "Presiona cualquier tecla para continuar...");
                            getch();
                            break;
                        case 2:
                            clear();
                            mvprintw(0, 0, "Has seleccionado: %s", opciones[seleccion]);
                            mvprintw(2, 0, "Presiona cualquier tecla para continuar...");
                            getch();
                            break;
                        case 3: // Si es "Salir"
                            endwin();
                            //terminate = true;
                            return;
                }
        }
        /*if (terminate){
            break;
    }*/

    endwin(); // Finaliza el modo ncurses
}
}
}
