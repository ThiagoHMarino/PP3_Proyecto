#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <vector>
#include "Archivo.h"
#include "menu.h"
#include "database.h"
#include "sqlite3.h"
#include <curses.h>
#include <algorithm>
#include <cctype>
#include <thread>
#include <string.h>

using namespace std;

int main() {
    // Fix para caracteres especiales en Windows
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    int altura = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    int ancho = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    cout << "Tamanio actual de ventana: " << ancho << "x" << altura << endl;

    if (altura < 25 || ancho < 80) {
        cout << "Ventana muy pequena. Ajustando..." << endl;

        COORD bufferSize = {80, 30};
        SetConsoleScreenBufferSize(hConsole, bufferSize);

        SMALL_RECT windowSize = {0, 0, 79, 29};
        SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

        Sleep(1000);

        GetConsoleScreenBufferInfo(hConsole, &csbi);
        altura = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        cout << "Nuevo tamanio: " << (csbi.srWindow.Right - csbi.srWindow.Left + 1) << "x" << altura << endl;

        if (altura < 25) {
            cout << "\n================================================" << endl;
            cout << "No se pudo ajustar la ventana automaticamente." << endl;
            cout << "Por favor MAXIMIZA esta ventana manualmente" << endl;
            cout << "y presiona Enter para continuar..." << endl;
            cout << "================================================\n" << endl;
            cin.get();

            GetConsoleScreenBufferInfo(hConsole, &csbi);
            altura = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

            if (altura < 10) {
                cout << "La ventana sigue siendo muy pequena." << endl;
                Sleep(2000);
                return;
            }
        }
    }
#endif

    WINDOW* mainwin = initscr();
    if (mainwin == NULL) {
        cout << "\n================================================" << endl;
        cout << "No se pudo inicializar la interfaz curses." << endl;
        cout << "================================================\n" << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        DataBase* db = new DataBase("alquiler.db");


        delete db;  // Cierra la conexión antes de salir

        std::cout << "Presiona Enter para continuar..." << std::endl;
        std::cin.get();

        cout << endl;
        cout << "==================================================" << endl;
        cout << "   Gracias por usar el sistema!" << endl;
        cout << "==================================================" << endl;

        return 0;
    }

    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int menu_ancho = 50;
    int menu_alto = 17;

    int y_inicio = (max_y - menu_alto) / 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    const char* titulo = "=== SISTEMA DE ALQUILER DE VEHICULOS ===";
    int titulo_len = strlen(titulo);
    int titulo_x = x_inicio + (menu_ancho - titulo_len) / 2;

    mvprintw(y_inicio, titulo_x, "%s", titulo);
    mvprintw(y_inicio+2, x_inicio, "%s", "Iniciando sistema...");
    mvprintw(y_inicio+3, x_inicio, "%s", "Cargando base de datos...");
    mvprintw(y_inicio+4, x_inicio, "%s", "Presiona Enter para continuar...");
    refresh();

    getch();

    // Inicializar el menú interactivo
    funcion_menu();

    DataBase* db = new DataBase("alquiler.db");


    delete db;  // Cierra la conexión antes de salir

    mvprintw(0, 0,"Presiona enter para continuar");

    return 0;
}