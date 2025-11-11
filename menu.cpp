#include "menu.h"
#include "Archivo.h"
#include "database.h"
// ============================================
// INCLUIR LA LIBRERÍA CURSES CORRECTA
// ============================================
#ifdef _WIN32
#define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #ifndef NOGDI
        #define NOGDI
    #endif
    #include <windows.h>
    #include <conio.h>
    #include <curses.h>
#else
#include <ncurses.h>
#include <termios.h>
#include <unistd.h>
#endif

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cctype>

using namespace std;

// ============================================
// FUNCIÓN PARA LIMPIAR EL BUFFER DE ENTRADA
// ============================================
void flush_input() {
#ifdef _WIN32
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
#elif defined(__unix__) || defined(__APPLE__)
    tcflush(STDIN_FILENO, TCIFLUSH);
#endif
}

// ============================================
// DESACTIVAR MOUSE TEMPORALMENTE
// ============================================
void desactivarMouse() {
    printf("\033[?1003l");  // Desactivar mouse tracking
    fflush(stdout);
    mousemask(0, NULL);
    flush_input();
}

// ============================================
// REACTIVAR MOUSE
// ============================================
void reactivarMouse() {
    flush_input();
    mouseinterval(0);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    printf("\033[?1003h");  // Activar mouse tracking
    fflush(stdout);
}

// ============================================
// SALIR DE CURSES TEMPORALMENTE
// ============================================
void salirModoCurses() {
    desactivarMouse();

#ifndef _WIN32
    usleep(10000);
#else
    Sleep(10);
#endif

    flush_input();
    def_prog_mode();
    endwin();
}

// ============================================
// REENTRAR EN CURSES
// ============================================
void entrarModoCurses() {
    reset_prog_mode();
    refresh();
    flush_input();

#ifndef _WIN32
    usleep(10000);
#else
    Sleep(10);
#endif

    reactivarMouse();

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
}

// ============= FUNCIONES DE VALIDACIÓN =============
bool esStringVacio(const string& str) {
    if (str.empty()) return true;
    return all_of(str.begin(), str.end(), [](unsigned char c) { return isspace(c); });
}

bool validarEdad(int edad) {
    return edad > 0 && edad < 150;
}

bool validarDNI(int dni) {
    return dni > 0 && dni < 100000000;
}

bool validarAnio(int anio) {
    return anio >= 1900 && anio <= 2025;
}

bool validarPrecio(float precio) {
    return precio > 0;
}

bool validarHoras(float horas) {
    return horas > 0;
}

// ============= MENÚS CURSES =============

void menuRegistrarCliente(SistemaAlquiler* sistema) {
    // DESACTIVAR MOUSE AL ENTRAR
    desactivarMouse();

    char nombre[100], apellido[100];
    int edad, dni;

    // Obtener dimensiones de la pantalla
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Calcular dimensiones del menú
    int menu_ancho = 50;
    int menu_alto = 17;

    // Centrar el menú
    int y_inicio = (max_y - menu_alto) / 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    // Mostrar título centrado dentro del menú
    const char* titulo = "=== REGISTRAR NUEVO CLIENTE ===";
    int titulo_len = strlen(titulo);
    int titulo_x = x_inicio + (menu_ancho - titulo_len) / 2;

    // Pedir y validar NOMBRE
    bool nombreValido = false;
    while (!nombreValido) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Nombre: ");
        refresh();

        echo();
        mvgetnstr(y_inicio + 2, x_inicio + 8, nombre, 99);
        noecho();

        if (esStringVacio(string(nombre))) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 5, x_inicio, "Error: El nombre no puede estar vacio.");
            mvprintw(y_inicio + 7, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else {
            nombreValido = true;
        }
    }

    // Pedir y validar APELLIDO
    bool apellidoValido = false;
    while (!apellidoValido) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Nombre: %s", nombre);
        mvprintw(y_inicio + 3, x_inicio, "Apellido: ");
        refresh();

        echo();
        mvgetnstr(y_inicio + 3, x_inicio + 10, apellido, 99);
        noecho();

        if (esStringVacio(string(apellido))) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 5, x_inicio, "Error: El apellido no puede estar vacio.");
            mvprintw(y_inicio + 7, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else {
            apellidoValido = true;
        }
    }

    // Pedir y validar EDAD
    bool edadValida = false;
    while (!edadValida) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Nombre: %s", nombre);
        mvprintw(y_inicio + 3, x_inicio, "Apellido: %s", apellido);
        mvprintw(y_inicio + 4, x_inicio, "Edad: ");
        refresh();

        echo();
        int resultado = mvscanw(y_inicio + 4, x_inicio + 6, const_cast<char*>("%d"), &edad);
        noecho();

        if (resultado != 1 || !validarEdad(edad)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 6, x_inicio, "Error: La edad debe ser un numero entre 1 y 149.");
            mvprintw(y_inicio + 8, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
        } else {
            edadValida = true;
        }
    }

    // Pedir y validar DNI
    bool dniValido = false;
    while (!dniValido) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Nombre: %s", nombre);
        mvprintw(y_inicio + 3, x_inicio, "Apellido: %s", apellido);
        mvprintw(y_inicio + 4, x_inicio, "Edad: %d", edad);
        mvprintw(y_inicio + 5, x_inicio, "DNI: ");
        refresh();

        echo();
        int resultado = mvscanw(y_inicio + 5, x_inicio + 4, const_cast<char*>("%d"), &dni);
        noecho();

        if (resultado != 1 || !validarDNI(dni)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 7, x_inicio, "Error: El DNI debe ser un numero valido.");
            mvprintw(y_inicio + 9, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
        } else {
            dniValido = true;
        }
    }

    // Registrar cliente
    clear();
    const char* msg_exito = "Cliente registrado exitosamente!";
    const char* msg_error = "Error: No se pudo registrar el cliente (posiblemente ya existe).";
    const char* msg_continuar = "Presiona cualquier tecla para continuar...";

    int msg_x;
    if (sistema->registrarCliente(string(nombre), string(apellido), edad, dni)) {
        msg_x = x_inicio + (menu_ancho - strlen(msg_exito)) / 2;
        mvprintw(y_inicio + 5, msg_x, "%s", msg_exito);
    } else {
        msg_x = x_inicio + (menu_ancho - strlen(msg_error)) / 2;
        mvprintw(y_inicio + 5, msg_x, "%s", msg_error);
    }

    int continuar_x = x_inicio + (menu_ancho - strlen(msg_continuar)) / 2;
    mvprintw(y_inicio + 7, continuar_x, "%s", msg_continuar);
    refresh();
    getch();

    // REACTIVAR MOUSE AL SALIR
    reactivarMouse();
}

void menuRegistrarVehiculo(SistemaAlquiler* sistema) {
    // DESACTIVAR MOUSE AL ENTRAR
    desactivarMouse();

    // Obtener dimensiones de la pantalla
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Calcular dimensiones del menú
    int menu_ancho = 60;
    int menu_alto = 20;

    // Centrar el menú
    int y_inicio = (max_y - menu_alto) / 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    const char* titulo = "=== REGISTRAR NUEVO VEHICULO ===";
    int titulo_len = strlen(titulo);
    int titulo_x = x_inicio + (menu_ancho - titulo_len) / 2;

    // Selección de tipo
    clear();
    mvprintw(y_inicio, titulo_x, "%s", titulo);
    mvprintw(y_inicio + 2, x_inicio, "Tipo de vehiculo:");
    mvprintw(y_inicio + 3, x_inicio, "1. Auto");
    mvprintw(y_inicio + 4, x_inicio, "2. Moto");
    mvprintw(y_inicio + 5, x_inicio, "Selecciona: ");
    refresh();

    int tipo = getch() - '0';

    if (tipo != 1 && tipo != 2) {
        clear();
        const char* msg_error = "Opcion invalida.";
        const char* msg_continuar = "Presiona cualquier tecla...";
        int error_x = x_inicio + (menu_ancho - strlen(msg_error)) / 2;
        int cont_x = x_inicio + (menu_ancho - strlen(msg_continuar)) / 2;

        mvprintw(y_inicio + 5, error_x, "%s", msg_error);
        mvprintw(y_inicio + 7, cont_x, "%s", msg_continuar);
        refresh();
        getch();
        reactivarMouse();
        return;
    }

    char marca[100], patente[100];
    int anio, extra;
    float precio;

    // Pedir y validar MARCA
    bool marcaValida = false;
    while (!marcaValida) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Tipo: %s", tipo == 1 ? "Auto" : "Moto");
        mvprintw(y_inicio + 4, x_inicio, "Marca: ");
        refresh();

        echo();
        mvgetnstr(y_inicio + 4, x_inicio + 7, marca, 99);
        noecho();

        if (esStringVacio(string(marca))) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 6, x_inicio, "Error: La marca no puede estar vacia.");
            mvprintw(y_inicio + 8, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else {
            marcaValida = true;
        }
    }

    // Pedir y validar PATENTE
    bool patenteValida = false;
    while (!patenteValida) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Tipo: %s", tipo == 1 ? "Auto" : "Moto");
        mvprintw(y_inicio + 4, x_inicio, "Marca: %s", marca);
        mvprintw(y_inicio + 5, x_inicio, "Patente: ");
        refresh();

        echo();
        mvgetnstr(y_inicio + 5, x_inicio + 9, patente, 99);
        noecho();

        if (esStringVacio(string(patente))) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 7, x_inicio, "Error: La patente no puede estar vacia.");
            mvprintw(y_inicio + 9, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else {
            patenteValida = true;
        }
    }

    // Pedir y validar AÑO
    bool anioValido = false;
    while (!anioValido) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Tipo: %s", tipo == 1 ? "Auto" : "Moto");
        mvprintw(y_inicio + 4, x_inicio, "Marca: %s", marca);
        mvprintw(y_inicio + 5, x_inicio, "Patente: %s", patente);
        mvprintw(y_inicio + 6, x_inicio, "Anio: ");
        refresh();

        echo();
        int resultado = mvscanw(y_inicio + 6, x_inicio + 6, const_cast<char*>("%d"), &anio);
        noecho();

        if (resultado != 1 || !validarAnio(anio)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 8, x_inicio, "Error: El anio debe estar entre 1900 y 2025.");
            mvprintw(y_inicio + 10, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
        } else {
            anioValido = true;
        }
    }

    // Pedir y validar PRECIO
    bool precioValido = false;
    while (!precioValido) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Tipo: %s", tipo == 1 ? "Auto" : "Moto");
        mvprintw(y_inicio + 4, x_inicio, "Marca: %s", marca);
        mvprintw(y_inicio + 5, x_inicio, "Patente: %s", patente);
        mvprintw(y_inicio + 6, x_inicio, "Anio: %d", anio);
        mvprintw(y_inicio + 7, x_inicio, "Precio base por hora: ");
        refresh();

        echo();
        int resultado = mvscanw(y_inicio + 7, x_inicio + 22, const_cast<char*>("%f"), &precio);
        noecho();

        if (resultado != 1 || !validarPrecio(precio)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 9, x_inicio, "Error: El precio debe ser mayor a 0.");
            mvprintw(y_inicio + 11, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
        } else {
            precioValido = true;
        }
    }

    Vehiculo* vehiculo = nullptr;

    // Pedir y validar dato específico según tipo
    bool extraValido = false;
    while (!extraValido) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Tipo: %s", tipo == 1 ? "Auto" : "Moto");
        mvprintw(y_inicio + 4, x_inicio, "Marca: %s", marca);
        mvprintw(y_inicio + 5, x_inicio, "Patente: %s", patente);
        mvprintw(y_inicio + 6, x_inicio, "Anio: %d", anio);
        mvprintw(y_inicio + 7, x_inicio, "Precio base por hora: %.2f", precio);
        refresh();

        echo();
        int resultado;

        if (tipo == 1) {
            mvprintw(y_inicio + 8, x_inicio, "Numero de puertas: ");
            resultado = mvscanw(y_inicio + 8, x_inicio + 19, const_cast<char*>("%d"), &extra);
            noecho();

            if (resultado != 1 || extra <= 0 || extra > 10) {
                clear();
                mvprintw(y_inicio, titulo_x, "%s", titulo);
                mvprintw(y_inicio + 10, x_inicio, "Error: El numero de puertas debe ser entre 1 y 10.");
                mvprintw(y_inicio + 12, x_inicio, "Presiona cualquier tecla para reintentar...");
                refresh();
                flushinp();
                getch();
            } else {
                extraValido = true;
                vehiculo = new Auto(string(marca), string(patente), anio, precio, extra);
            }
        } else {
            mvprintw(y_inicio + 8, x_inicio, "Cilindradas: ");
            resultado = mvscanw(y_inicio + 8, x_inicio + 13, const_cast<char*>("%d"), &extra);
            noecho();

            if (resultado != 1 || extra <= 0) {
                clear();
                mvprintw(y_inicio, titulo_x, "%s", titulo);
                mvprintw(y_inicio + 10, x_inicio, "Error: Las cilindradas deben ser mayores a 0.");
                mvprintw(y_inicio + 12, x_inicio, "Presiona cualquier tecla para reintentar...");
                refresh();
                flushinp();
                getch();
            } else {
                extraValido = true;
                vehiculo = new Moto(string(marca), string(patente), anio, precio, extra);
            }
        }
    }

    clear();
    const char* msg_exito = "Vehiculo registrado exitosamente!";
    const char* msg_error = "Error: No se pudo registrar el vehiculo.";
    const char* msg_continuar = "Presiona cualquier tecla para continuar...";

    int msg_x;
    if (sistema->registrarVehiculo(vehiculo)) {
        msg_x = x_inicio + (menu_ancho - strlen(msg_exito)) / 2;
        mvprintw(y_inicio + 5, msg_x, "%s", msg_exito);
    } else {
        msg_x = x_inicio + (menu_ancho - strlen(msg_error)) / 2;
        mvprintw(y_inicio + 5, msg_x, "%s", msg_error);
    }

    int continuar_x = x_inicio + (menu_ancho - strlen(msg_continuar)) / 2;
    mvprintw(y_inicio + 7, continuar_x, "%s", msg_continuar);
    refresh();
    getch();

    // REACTIVAR MOUSE AL SALIR
    reactivarMouse();
}

void menuCrearContrato(SistemaAlquiler* sistema) {
    // DESACTIVAR MOUSE AL ENTRAR
    desactivarMouse();

    char patente[100];
    int dni;
    float horas;

    // Obtener dimensiones de la pantalla
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Calcular dimensiones del menú
    int menu_ancho = 60;
    int menu_alto = 15;

    // Centrar el menú
    int y_inicio = (max_y - menu_alto) / 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    const char* titulo = "=== CREAR NUEVO CONTRATO ===";
    int titulo_len = strlen(titulo);
    int titulo_x = x_inicio + (menu_ancho - titulo_len) / 2;

    // Pedir y validar DNI
    bool dniValido = false;
    while (!dniValido) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 3, x_inicio, "DNI del cliente: ");
        refresh();

        echo();
        int resultado = mvscanw(y_inicio + 3, x_inicio + 17, const_cast<char*>("%d"), &dni);
        noecho();

        if (resultado != 1 || !validarDNI(dni)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 5, x_inicio, "Error: DNI invalido.");
            mvprintw(y_inicio + 7, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
        } else {
            dniValido = true;
        }
    }

    // Pedir y validar PATENTE
    bool patenteValida = false;
    while (!patenteValida) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 3, x_inicio, "DNI del cliente: %d", dni);
        mvprintw(y_inicio + 4, x_inicio, "Patente del vehiculo: ");
        refresh();

        echo();
        mvgetnstr(y_inicio + 4, x_inicio + 22, patente, 99);
        noecho();

        if (esStringVacio(string(patente))) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 6, x_inicio, "Error: La patente no puede estar vacia.");
            mvprintw(y_inicio + 8, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else {
            patenteValida = true;
        }
    }

    // Pedir y validar HORAS
    bool horasValidas = false;
    while (!horasValidas) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 3, x_inicio, "DNI del cliente: %d", dni);
        mvprintw(y_inicio + 4, x_inicio, "Patente del vehiculo: %s", patente);
        mvprintw(y_inicio + 5, x_inicio, "Horas de alquiler: ");
        refresh();

        echo();
        int resultado = mvscanw(y_inicio + 5, x_inicio + 19, const_cast<char*>("%f"), &horas);
        noecho();

        if (resultado != 1 || !validarHoras(horas)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 7, x_inicio, "Error: Las horas deben ser mayores a 0.");
            mvprintw(y_inicio + 9, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
        } else {
            horasValidas = true;
        }
    }

    clear();
    Contrato* contrato = sistema->crearNuevoContrato(dni, string(patente), horas);

    mvprintw(y_inicio, titulo_x, "%s", titulo);
    if (contrato != nullptr) {
        const char* msg_exito = "Contrato creado exitosamente!";
        int msg_x = x_inicio + (menu_ancho - strlen(msg_exito)) / 2;
        mvprintw(y_inicio + 4, msg_x, "%s", msg_exito);

        char id_msg[50];
        snprintf(id_msg, sizeof(id_msg), "ID del contrato: %d", contrato->getId());
        int id_x = x_inicio + (menu_ancho - strlen(id_msg)) / 2;
        mvprintw(y_inicio + 5, id_x, "%s", id_msg);
    } else {
        const char* msg_error = "Error: No se pudo crear el contrato.";
        const char* msg_verif = "Verifica que el cliente y vehiculo existan.";
        int error_x = x_inicio + (menu_ancho - strlen(msg_error)) / 2;
        int verif_x = x_inicio + (menu_ancho - strlen(msg_verif)) / 2;
        mvprintw(y_inicio + 4, error_x, "%s", msg_error);
        mvprintw(y_inicio + 5, verif_x, "%s", msg_verif);
    }

    const char* msg_continuar = "Presiona cualquier tecla para continuar...";
    int continuar_x = x_inicio + (menu_ancho - strlen(msg_continuar)) / 2;
    mvprintw(y_inicio + 8, continuar_x, "%s", msg_continuar);
    refresh();
    getch();

    // REACTIVAR MOUSE AL SALIR
    reactivarMouse();
}

void menuCerrarContrato(SistemaAlquiler* sistema) {
    // DESACTIVAR MOUSE AL ENTRAR
    desactivarMouse();

    // Obtener dimensiones de la pantalla
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Calcular dimensiones del menú
    int menu_ancho = 60;
    int menu_alto = 12;

    // Centrar el menú
    int y_inicio = (max_y - menu_alto) / 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    const char* titulo = "=== CERRAR CONTRATO ===";
    int titulo_len = strlen(titulo);
    int titulo_x = x_inicio + (menu_ancho - titulo_len) / 2;

    bool datosValidos = false;

    while (!datosValidos) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);

        echo();
        int id;
        mvprintw(y_inicio + 3, x_inicio, "ID del contrato a cerrar: ");
        int resultado = mvscanw(y_inicio + 3, x_inicio + 26, const_cast<char*>("%d"), &id);
        noecho();

        if (resultado != 1 || id <= 0) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 5, x_inicio, "Error: ID invalido. Debe ser un numero mayor a 0.");
            mvprintw(y_inicio + 7, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
            continue;
        }

        datosValidos = true;

        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);

        const char* msg_continuar = "Presiona cualquier tecla para continuar...";
        int continuar_x = x_inicio + (menu_ancho - strlen(msg_continuar)) / 2;

        if (sistema->cerrarContrato(id)) {
            const char* msg_exito = "Contrato cerrado exitosamente!";
            int msg_x = x_inicio + (menu_ancho - strlen(msg_exito)) / 2;
            mvprintw(y_inicio + 4, msg_x, "%s", msg_exito);
        } else {
            const char* msg_error = "Error: No se pudo cerrar el contrato.";
            int msg_x = x_inicio + (menu_ancho - strlen(msg_error)) / 2;
            mvprintw(y_inicio + 4, msg_x, "%s", msg_error);
        }

        mvprintw(y_inicio + 7, continuar_x, "%s", msg_continuar);
        refresh();
        getch();
    }
    // REACTIVAR MOUSE AL SALIR
    reactivarMouse();
}

void menuLimpiarBaseDatos() {
    // DESACTIVAR MOUSE AL ENTRAR
    desactivarMouse();
    // Obtener dimensiones de la pantalla
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Calcular dimensiones del menú
    int menu_ancho = 60;
    int menu_alto = 14;

    // Centrar el menú
    int y_inicio = (max_y - menu_alto) / 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    const char* titulo = "=== LIMPIAR BASE DE DATOS ===";
    int titulo_len = strlen(titulo);
    int titulo_x = x_inicio + (menu_ancho - titulo_len) / 2;

    clear();
    mvprintw(y_inicio, titulo_x, "%s", titulo);

    const char* advertencia = "ADVERTENCIA: Esta operacion eliminara TODOS los datos!";
    int adv_x = x_inicio + (menu_ancho - strlen(advertencia)) / 2;
    mvprintw(y_inicio + 3, adv_x, "%s", advertencia);

    mvprintw(y_inicio + 5, x_inicio + 5, "- Todos los clientes");
    mvprintw(y_inicio + 6, x_inicio + 5, "- Todos los vehiculos");
    mvprintw(y_inicio + 7, x_inicio + 5, "- Todos los contratos");

    const char* aviso = "Esta accion NO se puede deshacer.";
    int aviso_x = x_inicio + (menu_ancho - strlen(aviso)) / 2;
    mvprintw(y_inicio + 9, aviso_x, "%s", aviso);

    mvprintw(y_inicio + 11, x_inicio + 5, "Estas seguro? (S/N): ");
    refresh();

    echo();
    char confirmacion = getch();
    noecho();

    if (confirmacion == 'S' || confirmacion == 's') {
        clear();
        const char* msg_limpiando = "Limpiando base de datos...";
        int limp_x = x_inicio + (menu_ancho - strlen(msg_limpiando)) / 2;
        mvprintw(y_inicio + 5, limp_x, "%s", msg_limpiando);
        refresh();

        endwin();

        {
            DataBase db_temp("alquiler.db");
            db_temp.limpiarDatos();
        }

        cout << "\nBase de datos limpiada exitosamente!" << endl;
        cout << "El programa se reiniciara..." << endl;
        cout << "\nPresiona Enter para continuar...";
        cin.ignore();
        cin.get();

        exit(0);
    } else {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);

        const char* msg_cancelado = "Operacion cancelada.";
        int canc_x = x_inicio + (menu_ancho - strlen(msg_cancelado)) / 2;
        mvprintw(y_inicio + 5, canc_x, "%s", msg_cancelado);

        const char* msg_continuar = "Presiona cualquier tecla para continuar...";
        int cont_x = x_inicio + (menu_ancho - strlen(msg_continuar)) / 2;
        mvprintw(y_inicio + 7, cont_x, "%s", msg_continuar);
        refresh();
        getch();
    }
    // REACTIVAR MOUSE AL SALIR
    reactivarMouse();
}

// ============= RECUADRO Y CENTRADO =============
// Dibuja un recuadro con título centrado
void dibujarRecuadro(int y, int x, int alto, int ancho, const char* titulo = nullptr) {
    // Esquinas y bordes usando caracteres ASCII extendidos
    // Si no se ven bien, usa '+', '-', '|'

    // Esquina superior izquierda
    mvaddch(y, x, ACS_ULCORNER);
    // Línea superior
    for (int i = 1; i < ancho - 1; i++) {
        mvaddch(y, x + i, ACS_HLINE);
    }
    // Esquina superior derecha
    mvaddch(y, x + ancho - 1, ACS_URCORNER);

    // Lados verticales
    for (int i = 1; i < alto - 1; i++) {
        mvaddch(y + i, x, ACS_VLINE);
        mvaddch(y + i, x + ancho - 1, ACS_VLINE);
    }

    // Esquina inferior izquierda
    mvaddch(y + alto - 1, x, ACS_LLCORNER);
    // Línea inferior
    for (int i = 1; i < ancho - 1; i++) {
        mvaddch(y + alto - 1, x + i, ACS_HLINE);
    }
    // Esquina inferior derecha
    mvaddch(y + alto - 1, x + ancho - 1, ACS_LRCORNER);

    // Si hay título, dibujarlo centrado en el borde superior
    if (titulo != nullptr) {
        int titulo_len = strlen(titulo);
        int titulo_x = x + (ancho - titulo_len - 4) / 2;
        mvprintw(y, titulo_x, "[ %s ]", titulo);
    }
}

// Muestra un mensaje centrado en un recuadro
void mostrarMensaje(const char* titulo, const char* mensaje, const char* instruccion = "Presiona cualquier tecla para continuar...") {
    // DESACTIVAR MOUSE AL ENTRAR
    desactivarMouse();
    clear();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Calcular dimensiones del recuadro
    int mensaje_len = strlen(mensaje);
    int instruccion_len = strlen(instruccion);
    int ancho_contenido = (mensaje_len > instruccion_len ? mensaje_len : instruccion_len) + 4;

    if (ancho_contenido > max_x - 4) {
        ancho_contenido = max_x - 4;
    }

    int alto_recuadro = 6;
    int ancho_recuadro = ancho_contenido + 4;

    // Centrar el recuadro
    int y_inicio = (max_y - alto_recuadro) / 2;
    int x_inicio = (max_x - ancho_recuadro) / 2;

    // Dibujar recuadro
    dibujarRecuadro(y_inicio, x_inicio, alto_recuadro, ancho_recuadro, titulo);

    // Mostrar mensaje centrado
    int mensaje_x = x_inicio + (ancho_recuadro - mensaje_len) / 2;
    mvprintw(y_inicio + 2, mensaje_x, "%s", mensaje);

    // Mostrar instrucción centrada
    int instr_x = x_inicio + (ancho_recuadro - instruccion_len) / 2;
    mvprintw(y_inicio + 4, instr_x, "%s", instruccion);

    refresh();
    getch();

    // REACTIVAR MOUSE AL SALIR
    reactivarMouse();
}

// ============= Menú principal =============

// MENÚ PRINCIPAL (CON MOUSE Y TECLADO)
// ============================================
void funcion_menu() {
    DataBase db("alquiler.db");
    db.crearTablas();
    SistemaAlquiler sistema(&db);

    // === Inicialización segura del entorno curses ===
    initscr();             // <-- ESTA ES LA CLAVE (antes de usar cbreak/noecho)
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Activar soporte de mouse (click + hover)
    mouseinterval(0);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    printf("\033[?1003h"); // Activar tracking extendido
    fflush(stdout);

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
            "L. Limpiar Base de Datos",
            "0. Salir"
    };

    int n_opciones = 11;
    int seleccion = 0;

    while (true) {
        clear();

        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);

        int menu_ancho = 50;
        int menu_alto = n_opciones + 6;

        int y_inicio = (max_y - menu_alto) / 2;
        int x_inicio = (max_x - menu_ancho) / 2;

        dibujarRecuadro(y_inicio, x_inicio, menu_alto, menu_ancho, "SISTEMA DE ALQUILER");

        const char *instrucciones = "Usa flechas, Enter o Mouse";
        int instr_x = x_inicio + (menu_ancho - strlen(instrucciones)) / 2;
        mvprintw(y_inicio + 2, instr_x, "%s", instrucciones);

        mvaddch(y_inicio + 3, x_inicio, ACS_LTEE);
        for (int i = 1; i < menu_ancho - 1; i++) {
            mvaddch(y_inicio + 3, x_inicio + i, ACS_HLINE);
        }
        mvaddch(y_inicio + 3, x_inicio + menu_ancho - 1, ACS_RTEE);

        for (int i = 0; i < n_opciones; ++i) {
            int y_opcion = y_inicio + 4 + i;
            int opcion_x = (i == seleccion) ? x_inicio + 2 : x_inicio + 4;

            if (i == seleccion)
                attron(A_REVERSE);

            mvprintw(y_opcion, opcion_x, "%-*s", menu_ancho - 8, opciones[i]);

            if (i == seleccion)
                attroff(A_REVERSE);
        }

        refresh();
        int ch = getch();

        if (ch == KEY_MOUSE) {
            MEVENT me;
            if (getmouse(&me) == OK) {
                // Hover o clic
                if (me.x >= x_inicio + 2 && me.x < x_inicio + menu_ancho - 2) {
                    int hovered = me.y - (y_inicio + 4);
                    if (hovered >= 0 && hovered < n_opciones)
                        seleccion = hovered;
                }

                if (me.bstate & (BUTTON1_CLICKED | BUTTON1_RELEASED)) {
                    int idx = me.y - (y_inicio + 4);
                    if (idx >= 0 && idx < n_opciones) {
                        seleccion = idx;
                        ch = '\n';
                    }
                }
            }
        }

        switch (ch) {
            case KEY_UP:
                seleccion = (seleccion - 1 + n_opciones) % n_opciones;
                break;

            case KEY_DOWN:
                seleccion = (seleccion + 1) % n_opciones;
                break;

            case '\n':
            case '\r':
                clear();
                refresh();

                switch (seleccion) {
                    case 0: menuRegistrarCliente(&sistema); break;
                    case 1: menuRegistrarVehiculo(&sistema); break;

                    case 2:
                        salirModoCurses();
                        sistema.listarClientesRegistrados();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        entrarModoCurses();
                        break;

                    case 3:
                        salirModoCurses();
                        sistema.listarVehiculosDisponibles();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        entrarModoCurses();
                        break;

                    case 4:
                        salirModoCurses();
                        sistema.listarTodosVehiculos();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        entrarModoCurses();
                        break;

                    case 5: menuCrearContrato(&sistema); break;
                    case 6: menuCerrarContrato(&sistema); break;

                    case 7:
                        salirModoCurses();
                        sistema.listarContratos();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        entrarModoCurses();
                        break;

                    case 8:
                        salirModoCurses();
                        sistema.mostrarHistorialCompleto();
                        cout << "\nPresiona Enter para continuar...";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        entrarModoCurses();
                        break;

                    case 9: menuLimpiarBaseDatos(); break;

                    case 10:
                        mostrarMensaje("SALIR", "Gracias por usar el sistema!");
                        printf("\033[?1003l\n");
                        fflush(stdout);
                        endwin();
                        return;
                }
                break;
        }
    }

    // Limpieza final defensiva
    printf("\033[?1003l\n");
    fflush(stdout);
    endwin();
}
