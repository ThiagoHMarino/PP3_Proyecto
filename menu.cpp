#include "menu.h"
#include "Archivo.h"
#include "database.h"
#include <curses.h>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cctype>

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
    char nombre[100], apellido[100];
    int edad, dni;

    // Pedir y validar NOMBRE
    bool nombreValido = false;
    while (!nombreValido) {
        clear();
        mvprintw(0, 0, "=== REGISTRAR NUEVO CLIENTE ===");
        mvprintw(2, 0, "Nombre: ");
        refresh();

        echo();
        mvgetnstr(2, 8, nombre, 99);
        noecho();

        if (esStringVacio(string(nombre))) {
            clear();
            mvprintw(0, 0, "Error: El nombre no puede estar vacio.");
            mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
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
        mvprintw(0, 0, "=== REGISTRAR NUEVO CLIENTE ===");
        mvprintw(2, 0, "Nombre: %s", nombre);
        mvprintw(3, 0, "Apellido: ");
        refresh();

        echo();
        mvgetnstr(3, 10, apellido, 99);
        noecho();

        if (esStringVacio(string(apellido))) {
            clear();
            mvprintw(0, 0, "Error: El apellido no puede estar vacio.");
            mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
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
        mvprintw(0, 0, "=== REGISTRAR NUEVO CLIENTE ===");
        mvprintw(2, 0, "Nombre: %s", nombre);
        mvprintw(3, 0, "Apellido: %s", apellido);
        mvprintw(4, 0, "Edad: ");
        refresh();

        echo();
        int resultado = mvscanw(4, 6, "%d", &edad);
        noecho();

        if (resultado != 1 || !validarEdad(edad)) {
            clear();
            mvprintw(0, 0, "Error: La edad debe ser un numero entre 1 y 149.");
            mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
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
        mvprintw(0, 0, "=== REGISTRAR NUEVO CLIENTE ===");
        mvprintw(2, 0, "Nombre: %s", nombre);
        mvprintw(3, 0, "Apellido: %s", apellido);
        mvprintw(4, 0, "Edad: %d", edad);
        mvprintw(5, 0, "DNI: ");
        refresh();

        echo();
        int resultado = mvscanw(5, 5, "%d", &dni);
        noecho();

        if (resultado != 1 || !validarDNI(dni)) {
            clear();
            mvprintw(0, 0, "Error: El DNI debe ser un numero valido.");
            mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
        } else {
            dniValido = true;
        }
    }

    // Registrar cliente
    clear();
    if (sistema->registrarCliente(string(nombre), string(apellido), edad, dni)) {
        mvprintw(0, 0, "Cliente registrado exitosamente!");
    } else {
        mvprintw(0, 0, "Error: No se pudo registrar el cliente (posiblemente ya existe).");
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

    char marca[100], patente[100];
    int anio, extra;
    float precio;

    // Pedir y validar MARCA
    bool marcaValida = false;
    while (!marcaValida) {
        clear();
        mvprintw(0, 0, "=== REGISTRAR NUEVO VEHICULO ===");
        mvprintw(1, 0, "Tipo: %s", tipo == 1 ? "Auto" : "Moto");
        mvprintw(3, 0, "Marca: ");
        refresh();

        echo();
        mvgetnstr(3, 7, marca, 99);
        noecho();

        if (esStringVacio(string(marca))) {
            clear();
            mvprintw(0, 0, "Error: La marca no puede estar vacia.");
            mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
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
        mvprintw(0, 0, "=== REGISTRAR NUEVO VEHICULO ===");
        mvprintw(1, 0, "Tipo: %s", tipo == 1 ? "Auto" : "Moto");
        mvprintw(3, 0, "Marca: %s", marca);
        mvprintw(4, 0, "Patente: ");
        refresh();

        echo();
        mvgetnstr(4, 9, patente, 99);
        noecho();

        if (esStringVacio(string(patente))) {
            clear();
            mvprintw(0, 0, "Error: La patente no puede estar vacia.");
            mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
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
        mvprintw(0, 0, "=== REGISTRAR NUEVO VEHICULO ===");
        mvprintw(1, 0, "Tipo: %s", tipo == 1 ? "Auto" : "Moto");
        mvprintw(3, 0, "Marca: %s", marca);
        mvprintw(4, 0, "Patente: %s", patente);
        mvprintw(5, 0, "Anio: ");
        refresh();

        echo();
        int resultado = mvscanw(5, 6, "%d", &anio);
        noecho();

        if (resultado != 1 || !validarAnio(anio)) {
            clear();
            mvprintw(0, 0, "Error: El anio debe estar entre 1900 y 2025.");
            mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
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
        mvprintw(0, 0, "=== REGISTRAR NUEVO VEHICULO ===");
        mvprintw(1, 0, "Tipo: %s", tipo == 1 ? "Auto" : "Moto");
        mvprintw(3, 0, "Marca: %s", marca);
        mvprintw(4, 0, "Patente: %s", patente);
        mvprintw(5, 0, "Anio: %d", anio);
        mvprintw(6, 0, "Precio base por hora: ");
        refresh();

        echo();
        int resultado = mvscanw(6, 22, "%f", &precio);
        noecho();

        if (resultado != 1 || !validarPrecio(precio)) {
            clear();
            mvprintw(0, 0, "Error: El precio debe ser mayor a 0.");
            mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
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
        mvprintw(0, 0, "=== REGISTRAR NUEVO VEHICULO ===");
        mvprintw(1, 0, "Tipo: %s", tipo == 1 ? "Auto" : "Moto");
        mvprintw(3, 0, "Marca: %s", marca);
        mvprintw(4, 0, "Patente: %s", patente);
        mvprintw(5, 0, "Anio: %d", anio);
        mvprintw(6, 0, "Precio base por hora: %.2f", precio);
        refresh();

        echo();
        int resultado;

        if (tipo == 1) {
            mvprintw(7, 0, "Numero de puertas: ");
            resultado = mvscanw(7, 19, "%d", &extra);
            noecho();

            if (resultado != 1 || extra <= 0 || extra > 10) {
                clear();
                mvprintw(0, 0, "Error: El numero de puertas debe ser entre 1 y 10.");
                mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
                refresh();
                flushinp();
                getch();
            } else {
                extraValido = true;
                vehiculo = new Auto(string(marca), string(patente), anio, precio, extra);
            }
        } else {
            mvprintw(7, 0, "Cilindradas: ");
            resultado = mvscanw(7, 13, "%d", &extra);
            noecho();

            if (resultado != 1 || extra <= 0) {
                clear();
                mvprintw(0, 0, "Error: Las cilindradas deben ser mayores a 0.");
                mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
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
    char patente[100];
    int dni;
    float horas;

    // Pedir y validar DNI
    bool dniValido = false;
    while (!dniValido) {
        clear();
        mvprintw(0, 0, "=== CREAR NUEVO CONTRATO ===");
        mvprintw(2, 0, "DNI del cliente: ");
        refresh();

        echo();
        int resultado = mvscanw(2, 17, "%d", &dni);
        noecho();

        if (resultado != 1 || !validarDNI(dni)) {
            clear();
            mvprintw(0, 0, "Error: DNI invalido.");
            mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
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
        mvprintw(0, 0, "=== CREAR NUEVO CONTRATO ===");
        mvprintw(2, 0, "DNI del cliente: %d", dni);
        mvprintw(3, 0, "Patente del vehiculo: ");
        refresh();

        echo();
        mvgetnstr(3, 22, patente, 99);
        noecho();

        if (esStringVacio(string(patente))) {
            clear();
            mvprintw(0, 0, "Error: La patente no puede estar vacia.");
            mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
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
        mvprintw(0, 0, "=== CREAR NUEVO CONTRATO ===");
        mvprintw(2, 0, "DNI del cliente: %d", dni);
        mvprintw(3, 0, "Patente del vehiculo: %s", patente);
        mvprintw(4, 0, "Horas de alquiler: ");
        refresh();

        echo();
        int resultado = mvscanw(4, 19, "%f", &horas);
        noecho();

        if (resultado != 1 || !validarHoras(horas)) {
            clear();
            mvprintw(0, 0, "Error: Las horas deben ser mayores a 0.");
            mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
        } else {
            horasValidas = true;
        }
    }

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
    bool datosValidos = false;

    while (!datosValidos) {
        clear();
        mvprintw(0, 0, "=== CERRAR CONTRATO ===");

        echo();
        int id;
        mvprintw(2, 0, "ID del contrato a cerrar: ");
        int resultado = mvscanw(2, 26, "%d", &id);
        noecho();

        if (resultado != 1 || id <= 0) {
            clear();
            mvprintw(0, 0, "Error: ID invalido. Debe ser un numero mayor a 0.");
            mvprintw(2, 0, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
            continue;
        }

        datosValidos = true;

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
}

void menuLimpiarBaseDatos() {
    clear();
    mvprintw(0, 0, "=== LIMPIAR BASE DE DATOS ===");
    mvprintw(2, 0, "ADVERTENCIA: Esta operacion eliminara TODOS los datos!");
    mvprintw(3, 0, "- Todos los clientes");
    mvprintw(4, 0, "- Todos los vehiculos");
    mvprintw(5, 0, "- Todos los contratos");
    mvprintw(7, 0, "Esta accion NO se puede deshacer.");
    mvprintw(9, 0, "Estas seguro? (S/N): ");
    refresh();

    echo();
    char confirmacion = getch();
    noecho();

    if (confirmacion == 'S' || confirmacion == 's') {
        clear();
        mvprintw(0, 0, "Limpiando base de datos...");
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
        mvprintw(0, 0, "Operacion cancelada.");
        mvprintw(2, 0, "Presiona cualquier tecla para continuar...");
        refresh();
        getch();
    }
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
}

// ============= Menú principal =============

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
        "L. Limpiar Base de Datos",
        "0. Salir"
    };

    int n_opciones = 11;
    int seleccion = 0;

    while (true) {
        clear();

        // Obtener dimensiones de la pantalla
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);

        // Calcular dimensiones del menú
        int menu_ancho = 50;  // Ancho fijo del menú
        int menu_alto = n_opciones + 6;  // Altura: opciones + título + bordes + instrucciones

        // Centrar el menú
        int y_inicio = (max_y - menu_alto) / 2;
        int x_inicio = (max_x - menu_ancho) / 2;

        // Dibujar recuadro principal
        dibujarRecuadro(y_inicio, x_inicio, menu_alto, menu_ancho, "SISTEMA DE ALQUILER");

        // Mostrar instrucciones
        const char* instrucciones = "Usa flechas y Enter";
        int instr_x = x_inicio + (menu_ancho - strlen(instrucciones)) / 2;
        mvprintw(y_inicio + 2, instr_x, "%s", instrucciones);

        // Dibujar separador
        mvaddch(y_inicio + 3, x_inicio, ACS_LTEE);
        for (int i = 1; i < menu_ancho - 1; i++) {
            mvaddch(y_inicio + 3, x_inicio + i, ACS_HLINE);
        }
        mvaddch(y_inicio + 3, x_inicio + menu_ancho - 1, ACS_RTEE);

        // Dibujar opciones centradas
        for (int i = 0; i < n_opciones; ++i) {
            int y_opcion = y_inicio + 4 + i;

            if (i == seleccion) {
                attron(A_REVERSE);
                // Dibujar opción resaltada con padding
                int opcion_x = x_inicio + 2;
                mvprintw(y_opcion, opcion_x, "  %-*s  ", menu_ancho - 6, opciones[i]);
                attroff(A_REVERSE);
            } else {
                // Opción normal
                int opcion_x = x_inicio + 4;
                mvprintw(y_opcion, opcion_x, "%-*s", menu_ancho - 8, opciones[i]);
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
            case '\r':
                clear();
                refresh();

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
                        menuLimpiarBaseDatos();
                        break;
                    case 10:
                        mostrarMensaje("SALIR", "Gracias por usar el sistema!");
                        endwin();
                        return;
                }
                break;
        }
    }

    endwin();
}
