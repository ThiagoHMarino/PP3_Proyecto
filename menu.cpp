#include "menu.h"
#include "Archivo.h"
#include "database.h"
#include <curses.h>
#include <string>
#include <iostream>

using namespace std;

// Función auxiliar para leer string en ncurses
string leerString(const char* prompt) {
    echo();
    char buffer[100];
    mvprintw(getcury(stdscr) + 1, 0, "%s", prompt);
    refresh();
    getnstr(buffer, 99);
    noecho();
    return string(buffer);
}

// Función auxiliar para leer entero
int leerEntero(const char* prompt) {
    echo();
    mvprintw(getcury(stdscr) + 1, 0, "%s", prompt);
    refresh();
    int valor;
    scanw("%d", &valor);
    noecho();
    return valor;
}

// Función auxiliar para leer float
float leerFloat(const char* prompt) {
    echo();
    mvprintw(getcury(stdscr) + 1, 0, "%s", prompt);
    refresh();
    float valor;
    scanw("%f", &valor);
    noecho();
    return valor;
}

// Menú para registrar cliente
void menuRegistrarCliente(SistemaAlquiler* sistema) {
    clear();
    mvprintw(0, 0, "=== REGISTRAR NUEVO CLIENTE ===");
    mvprintw(1, 0, "");

    string nombre = leerString("Nombre: ");
    string apellido = leerString("Apellido: ");
    int edad = leerEntero("Edad: ");
    int dni = leerEntero("DNI: ");

    clear();
    if (sistema->registrarCliente(nombre, apellido, edad, dni)) {
        mvprintw(0, 0, "Cliente registrado exitosamente!");
    } else {
        mvprintw(0, 0, "Error: No se pudo registrar el cliente (quizas ya existe).");
    }

    mvprintw(2, 0, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();
}

// Menú para registrar vehículo
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
    string marca = leerString("Marca: ");
    string patente = leerString("Patente: ");
    int anio = leerEntero("Anio: ");
    float precio = leerFloat("Precio base por hora: ");

    Vehiculo* vehiculo = nullptr;

    if (tipo == 1) {
        int puertas = leerEntero("Numero de puertas: ");
        vehiculo = new Auto(marca, patente, anio, precio, puertas);
    } else {
        int cilindradas = leerEntero("Cilindradas: ");
        vehiculo = new Moto(marca, patente, anio, precio, cilindradas);
    }

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

// Menú para crear contrato
void menuCrearContrato(SistemaAlquiler* sistema) {
    clear();
    mvprintw(0, 0, "=== CREAR NUEVO CONTRATO ===");
    mvprintw(1, 0, "");

    int dni = leerEntero("DNI del cliente: ");
    string patente = leerString("Patente del vehiculo: ");
    float horas = leerFloat("Horas de alquiler: ");

    clear();
    Contrato* contrato = sistema->crearNuevoContrato(dni, patente, horas);

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

// Menú para cerrar contrato
void menuCerrarContrato(SistemaAlquiler* sistema) {
    clear();
    mvprintw(0, 0, "=== CERRAR CONTRATO ===");
    mvprintw(1, 0, "");

    int id = leerEntero("ID del contrato a cerrar: ");

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

// Función principal del menú
void funcion_menu() {
    // Inicializar base de datos
    DataBase db("alquiler.db");
    db.crearTablas();

    // Inicializar sistema
    SistemaAlquiler sistema(&db);

    initscr();
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
            case '\n': // Enter
                clear();
                refresh();

                // Temporalmente salir de curses para operaciones que usan cout
                endwin();

                switch(seleccion) {
                    case 0:
                        menuRegistrarCliente(&sistema);
                        break;
                    case 1:
                        menuRegistrarVehiculo(&sistema);
                        break;
                    case 2:
                        cout << endl;
                        sistema.listarClientesRegistrados();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore();
                        cin.get();
                        break;
                    case 3:
                        cout << endl;
                        sistema.listarVehiculosDisponibles();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore();
                        cin.get();
                        break;
                    case 4:
                        cout << endl;
                        sistema.listarTodosVehiculos();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore();
                        cin.get();
                        break;
                    case 5:
                        menuCrearContrato(&sistema);
                        break;
                    case 6:
                        menuCerrarContrato(&sistema);
                        break;
                    case 7:
                        cout << endl;
                        sistema.listarContratos();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore();
                        cin.get();
                        break;
                    case 8:
                        cout << endl;
                        sistema.mostrarHistorialCompleto();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore();
                        cin.get();
                        break;
                    case 9:
                        endwin();
                        return;
                }

                // Reiniciar curses
                initscr();
                cbreak();
                noecho();
                keypad(stdscr, TRUE);
                break;
        }
    }

    endwin();
}
