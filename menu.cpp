#include "menu.h"
#include "Archivo.h"
#include "database.h"
#include <curses.h>
#include <string>
#include <iostream>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#ifndef NOGDI
#define NOGDI
#endif
#include <windows.h>
#undef NOGDI
#endif

using namespace std;

// ============= MENÚ SIMPLE (SIN CURSES) =============
void menu_simple() {
    DataBase db("alquiler.db");
    db.crearTablas();
    SistemaAlquiler sistema(&db);

    while (true) {
        system("cls");  // Limpiar pantalla en Windows
        cout << "\n========================================" << endl;
        cout << "  SISTEMA DE ALQUILER DE VEHICULOS" << endl;
        cout << "========================================" << endl;
        cout << "1. Registrar Cliente" << endl;
        cout << "2. Registrar Vehiculo" << endl;
        cout << "3. Listar Clientes" << endl;
        cout << "4. Listar Vehiculos Disponibles" << endl;
        cout << "5. Listar Todos los Vehiculos" << endl;
        cout << "6. Crear Contrato" << endl;
        cout << "7. Cerrar Contrato" << endl;
        cout << "8. Ver Contratos Activos" << endl;
        cout << "9. Ver Historial Completo" << endl;
        cout << "0. Salir" << endl;
        cout << "========================================" << endl;
        cout << "Selecciona una opcion: ";

        int opcion;
        cin >> opcion;
        cin.ignore();

        switch(opcion) {
            case 1: {
                cout << "\n=== REGISTRAR NUEVO CLIENTE ===" << endl;
                string nombre, apellido;
                int edad, dni;

                cout << "Nombre: ";
                getline(cin, nombre);
                cout << "Apellido: ";
                getline(cin, apellido);
                cout << "Edad: ";
                cin >> edad;
                cout << "DNI: ";
                cin >> dni;
                cin.ignore();

                if (sistema.registrarCliente(nombre, apellido, edad, dni)) {
                    cout << "\nCliente registrado exitosamente!" << endl;
                } else {
                    cout << "\nError: No se pudo registrar el cliente." << endl;
                }
                break;
            }
            case 2: {
                cout << "\n=== REGISTRAR NUEVO VEHICULO ===" << endl;
                cout << "Tipo de vehiculo:" << endl;
                cout << "1. Auto" << endl;
                cout << "2. Moto" << endl;
                cout << "Selecciona: ";

                int tipo;
                cin >> tipo;
                cin.ignore();

                string marca, patente;
                int anio;
                float precio;

                cout << "Marca: ";
                getline(cin, marca);
                cout << "Patente: ";
                getline(cin, patente);
                cout << "Anio: ";
                cin >> anio;
                cout << "Precio base por hora: ";
                cin >> precio;
                cin.ignore();

                Vehiculo* vehiculo = nullptr;

                if (tipo == 1) {
                    int puertas;
                    cout << "Numero de puertas: ";
                    cin >> puertas;
                    cin.ignore();
                    vehiculo = new Auto(marca, patente, anio, precio, puertas);
                } else if (tipo == 2) {
                    int cilindradas;
                    cout << "Cilindradas: ";
                    cin >> cilindradas;
                    cin.ignore();
                    vehiculo = new Moto(marca, patente, anio, precio, cilindradas);
                }

                if (vehiculo && sistema.registrarVehiculo(vehiculo)) {
                    cout << "\nVehiculo registrado exitosamente!" << endl;
                } else {
                    cout << "\nError: No se pudo registrar el vehiculo." << endl;
                }
                break;
            }
            case 3:
                cout << "\n";
                sistema.listarClientesRegistrados();
                break;
            case 4:
                cout << "\n";
                sistema.listarVehiculosDisponibles();
                break;
            case 5:
                cout << "\n";
                sistema.listarTodosVehiculos();
                break;
            case 6: {
                cout << "\n=== CREAR NUEVO CONTRATO ===" << endl;
                int dni;
                string patente;
                float horas;

                cout << "DNI del cliente: ";
                cin >> dni;
                cin.ignore();
                cout << "Patente del vehiculo: ";
                getline(cin, patente);
                cout << "Horas de alquiler: ";
                cin >> horas;
                cin.ignore();

                Contrato* contrato = sistema.crearNuevoContrato(dni, patente, horas);
                if (contrato != nullptr) {
                    cout << "\nContrato creado exitosamente!" << endl;
                    cout << "ID del contrato: " << contrato->getId() << endl;
                } else {
                    cout << "\nError: No se pudo crear el contrato." << endl;
                }
                break;
            }
            case 7: {
                cout << "\n=== CERRAR CONTRATO ===" << endl;
                int id;
                cout << "ID del contrato a cerrar: ";
                cin >> id;
                cin.ignore();

                if (sistema.cerrarContrato(id)) {
                    cout << "\nContrato cerrado exitosamente!" << endl;
                } else {
                    cout << "\nError: No se pudo cerrar el contrato." << endl;
                }
                break;
            }
            case 8:
                cout << "\n";
                sistema.listarContratos();
                break;
            case 9:
                cout << "\n";
                sistema.mostrarHistorialCompleto();
                break;
            case 0:
                cout << "\nSaliendo del sistema..." << endl;
                return;
            default:
                cout << "\nOpcion invalida. Intenta de nuevo." << endl;
        }

        cout << "\nPresiona Enter para continuar...";
        cin.get();
    }
}

// ============= MENÚS CURSES =============

void menuRegistrarCliente(SistemaAlquiler* sistema) {
    clear();
    mvprintw(0, 0, "=== REGISTRAR NUEVO CLIENTE ===");
    mvprintw(2, 0, "Nombre: ");
    refresh();

    echo();
    char nombre[100], apellido[100];
    int edad, dni;

    mvgetnstr(2, 8, nombre, 99);

    mvprintw(3, 0, "Apellido: ");
    mvgetnstr(3, 10, apellido, 99);

    mvprintw(4, 0, "Edad: ");
    mvscanw(4, 6, "%d", &edad);

    mvprintw(5, 0, "DNI: ");
    mvscanw(5, 5, "%d", &dni);
    noecho();

    clear();
    if (sistema->registrarCliente(string(nombre), string(apellido), edad, dni)) {
        mvprintw(0, 0, "Cliente registrado exitosamente!");
    } else {
        mvprintw(0, 0, "Error: No se pudo registrar el cliente (quizas ya existe).");
    }

    mvprintw(2, 0, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();
}

void menuRegistrarVehiculo(SistemaAlquiler* sistema) {
    clear();
    mvprintw(0, 0, "=== REGISTRAR NUEVO VEHICULO ===");
    mvprintw(1, 0, "Tipo de vehiculo:");
    mvprintw(2, 0, "1. Auto");
    mvprintw(3, 0, "2. Moto");
    mvprintw(4, 0, "Selecciona: ");
    refresh();

    int tipo = getch() - '0';

    if (tipo != 1 && tipo != 2) {
        clear();
        mvprintw(0, 0, "Opcion invalida.");
        mvprintw(2, 0, "Presiona cualquier tecla...");
        refresh();
        getch();
        return;
    }

    clear();
    echo();

    char marca[100], patente[100];
    int anio, extra;
    float precio;

    mvprintw(0, 0, "Marca: ");
    mvgetnstr(0, 7, marca, 99);

    mvprintw(1, 0, "Patente: ");
    mvgetnstr(1, 9, patente, 99);

    mvprintw(2, 0, "Anio: ");
    mvscanw(2, 6, "%d", &anio);

    mvprintw(3, 0, "Precio base por hora: ");
    mvscanw(3, 22, "%f", &precio);

    Vehiculo* vehiculo = nullptr;

    if (tipo == 1) {
        mvprintw(4, 0, "Numero de puertas: ");
        mvscanw(4, 19, "%d", &extra);
        vehiculo = new Auto(string(marca), string(patente), anio, precio, extra);
    } else {
        mvprintw(4, 0, "Cilindradas: ");
        mvscanw(4, 13, "%d", &extra);
        vehiculo = new Moto(string(marca), string(patente), anio, precio, extra);
    }

    noecho();

    clear();
    if (sistema->registrarVehiculo(vehiculo)) {
        mvprintw(0, 0, "Vehiculo registrado exitosamente!");
    } else {
        mvprintw(0, 0, "Error: No se pudo registrar el vehiculo.");
    }

    mvprintw(2, 0, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();
}

void menuCrearContrato(SistemaAlquiler* sistema) {
    clear();
    mvprintw(0, 0, "=== CREAR NUEVO CONTRATO ===");

    echo();
    char patente[100];
    int dni;
    float horas;

    mvprintw(2, 0, "DNI del cliente: ");
    mvscanw(2, 17, "%d", &dni);

    mvprintw(3, 0, "Patente del vehiculo: ");
    mvgetnstr(3, 22, patente, 99);

    mvprintw(4, 0, "Horas de alquiler: ");
    mvscanw(4, 19, "%f", &horas);
    noecho();

    clear();
    Contrato* contrato = sistema->crearNuevoContrato(dni, string(patente), horas);

    if (contrato != nullptr) {
        mvprintw(0, 0, "Contrato creado exitosamente!");
        mvprintw(1, 0, "ID del contrato: %d", contrato->getId());
    } else {
        mvprintw(0, 0, "Error: No se pudo crear el contrato.");
        mvprintw(1, 0, "Verifica que el cliente y vehiculo existan.");
    }

    mvprintw(3, 0, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();
}

void menuCerrarContrato(SistemaAlquiler* sistema) {
    clear();
    mvprintw(0, 0, "=== CERRAR CONTRATO ===");

    echo();
    int id;
    mvprintw(2, 0, "ID del contrato a cerrar: ");
    mvscanw(2, 26, "%d", &id);
    noecho();

    clear();
    if (sistema->cerrarContrato(id)) {
        mvprintw(0, 0, "Contrato cerrado exitosamente!");
    } else {
        mvprintw(0, 0, "Error: No se pudo cerrar el contrato.");
    }

    mvprintw(2, 0, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();
}

// ============= FUNCIÓN PRINCIPAL =============
void funcion_menu() {
    DataBase db("alquiler.db");
    db.crearTablas();
    SistemaAlquiler sistema(&db);

    cout << "\nIntentando inicializar interfaz grafica..." << endl;

#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    int altura = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    int ancho = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    cout << "Tamanio actual de ventana: " << ancho << "x" << altura << endl;

    if (altura < 25 || ancho < 80) {
        cout << "Ventana muy pequena. Ajustando..." << endl;

        // Primero ajustar el buffer
        COORD bufferSize = {80, 30};
        SetConsoleScreenBufferSize(hConsole, bufferSize);

        // Luego ajustar la ventana
        SMALL_RECT windowSize = {0, 0, 79, 29};
        SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

        Sleep(1000);  // Dar más tiempo para que se ajuste

        // Verificar si funcionó
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

            // Verificar de nuevo
            GetConsoleScreenBufferInfo(hConsole, &csbi);
            altura = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

            if (altura < 10) {
                cout << "La ventana sigue siendo muy pequena." << endl;
                cout << "Usando menu simple..." << endl;
                Sleep(2000);
                menu_simple();
                return;
            }
        }
    }
#endif

    WINDOW* mainwin = initscr();
    if (mainwin == NULL) {
        cout << "\n================================================" << endl;
        cout << "No se pudo inicializar la interfaz curses." << endl;
        cout << "Usando menu simple en su lugar..." << endl;
        cout << "================================================\n" << endl;
        Sleep(2000);
        menu_simple();
        return;
    }

    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    const char *opciones[] = {
        "1. Registrar Cliente",
        "2. Registrar Vehiculo",
        "3. Listar Clientes",
        "4. Listar Vehiculos Disponibles",
        "5. Listar Todos los Vehiculos",
        "6. Crear Contrato",
        "7. Cerrar Contrato",
        "8. Ver Contratos Activos",
        "9. Ver Historial Completo",
        "0. Salir"
    };

    int n_opciones = 10;
    int seleccion = 0;

    while (true) {
        clear();
        mvprintw(0, 0, "=== SISTEMA DE ALQUILER DE VEHICULOS ===");
        mvprintw(1, 0, "Usa las flechas para navegar y Enter para seleccionar:");
        mvprintw(2, 0, "");

        for (int i = 0; i < n_opciones; ++i) {
            if (i == seleccion) {
                attron(A_REVERSE);
            }
            mvprintw(i + 3, 2, "%s", opciones[i]);
            if (i == seleccion) {
                attroff(A_REVERSE);
            }
        }

        refresh();
        int ch = getch();

        switch (ch) {
            case KEY_UP:
                seleccion = (seleccion - 1 + n_opciones) % n_opciones;
                break;
            case KEY_DOWN:
                seleccion = (seleccion + 1) % n_opciones;
                break;
            case '\n':
                clear();
                refresh();

                // NO salir de curses para las opciones de menú
                switch(seleccion) {
                    case 0:
                        menuRegistrarCliente(&sistema);
                        break;
                    case 1:
                        menuRegistrarVehiculo(&sistema);
                        break;
                    case 2:
                        endwin();
                        cout << endl;
                        sistema.listarClientesRegistrados();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore();
                        cin.get();
                        initscr();
                        cbreak();
                        noecho();
                        keypad(stdscr, TRUE);
                        break;
                    case 3:
                        endwin();
                        cout << endl;
                        sistema.listarVehiculosDisponibles();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore();
                        cin.get();
                        initscr();
                        cbreak();
                        noecho();
                        keypad(stdscr, TRUE);
                        break;
                    case 4:
                        endwin();
                        cout << endl;
                        sistema.listarTodosVehiculos();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore();
                        cin.get();
                        initscr();
                        cbreak();
                        noecho();
                        keypad(stdscr, TRUE);
                        break;
                    case 5:
                        menuCrearContrato(&sistema);
                        break;
                    case 6:
                        menuCerrarContrato(&sistema);
                        break;
                    case 7:
                        endwin();
                        cout << endl;
                        sistema.listarContratos();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore();
                        cin.get();
                        initscr();
                        cbreak();
                        noecho();
                        keypad(stdscr, TRUE);
                        break;
                    case 8:
                        endwin();
                        cout << endl;
                        sistema.mostrarHistorialCompleto();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore();
                        cin.get();
                        initscr();
                        cbreak();
                        noecho();
                        keypad(stdscr, TRUE);
                        break;
                    case 9:
                        endwin();
                        return;
                }
                break;
        }
    }

    endwin();
}