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
#include <limits>
#include <cstring>
#include <regex>
#include <string.h>


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

// ============= FUNCIONES DE VALIDACIÓN MEJORADAS =============

// Validar que el string no esté vacío ni contenga solo espacios
bool esStringVacio(const string& str) {
    if (str.empty()) return true;
    return all_of(str.begin(), str.end(), [](unsigned char c) { return isspace(c); });
}

// Validar que el string solo contenga letras y espacios (para nombres)
bool validarSoloLetras(const string& str) {
    if (esStringVacio(str)) return false;
    for (char c : str) {
        if (!isalpha(c) && !isspace(c)) {
            return false;
        }
    }
    return true;
}

// Validar longitud mínima y máxima de strings
bool validarLongitud(const string& str, int min, int max) {
    int len = str.length();
    return len >= min && len <= max;
}

// Validar edad
bool validarEdad(int edad) {
    return edad >= 18 && edad <= 120;  // Edad mínima 18 para alquilar
}

// Validar DNI (Argentina: 7-8 dígitos)
bool validarDNI(int dni) {
    return dni >= 1000000 && dni <= 99999999;
}

// Validar año
bool validarAnio(int anio) {
    auto now = chrono::system_clock::now();
    time_t tt = chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&tt);
    int anioActual = local_tm.tm_year + 1900;

    return anio >= 1900 && anio <= anioActual + 1;
}

// Validar precio
bool validarPrecio(float precio) {
    return precio > 0 && precio <= 1000000;  // Precio máximo razonable
}

// Validar horas
bool validarHoras(float horas) {
    return horas > 0 && horas <= 720;  // Máximo 30 días (720 horas)
}

// Validar patente (formatos argentinosS)
bool validarPatente(const string& patente) {
    if (esStringVacio(patente)) return false;

    string pat = patente;
    // Eliminar espacios
    pat.erase(remove_if(pat.begin(), pat.end(), ::isspace), pat.end());

    // Convertir a mayúsculas
    transform(pat.begin(), pat.end(), pat.begin(), ::toupper);

    // Formato viejo: ABC123 o AB123CD
    // Formato nuevo: AB123CD o AA123AA
    int len = pat.length();

    if (len < 6 || len > 7) return false;

    // Verificar caracteres alfanuméricos
    for (char c : pat) {
        if (!isalnum(c)) return false;
    }

    return true;
}

// Validar número de puertas
bool validarPuertas(int puertas) {
    return puertas >= 2 && puertas <= 5;  // Autos típicos tienen 2-5 puertas
}

// Validar cilindradas
bool validarCilindradas(int cilindradas) {
    return cilindradas >= 50 && cilindradas <= 2000;  // Rango típico de motos
}

// Validar ID de contrato
bool validarIdContrato(int id) {
    return id > 0 && id < 1000000;
}

// Limpiar entrada numérica (eliminar caracteres no numéricos)
string limpiarEntradaNumerica(const string& input) {
    string resultado;
    for (char c : input) {
        if (isdigit(c) || c == '.' || c == '-') {
            resultado += c;
        }
    }
    return resultado;
}

// Convertir string a mayúsculas
string toUpper(const string& str) {
    string resultado = str;
    transform(resultado.begin(), resultado.end(), resultado.begin(), ::toupper);
    return resultado;
}

// Trim espacios al inicio y final
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool esBisiesto(int anio) {
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
}

// Validar si una fecha es válida
bool validarFecha(int dia, int mes, int anio) {
    // Validar mes
    if (mes < 1 || mes > 12) return false;

    // Obtener año actual
    auto now = chrono::system_clock::now();
    time_t tt = chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&tt);
    int anioActual = local_tm.tm_year + 1900;

    // Validar año (persona debe haber nacido entre 1900 y año actual)
    if (anio < 1900 || anio > anioActual) return false;

    // Días por mes
    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Ajustar febrero si es bisiesto
    if (esBisiesto(anio)) {
        diasPorMes[1] = 29;
    }

    // Validar día
    if (dia < 1 || dia > diasPorMes[mes - 1]) return false;

    return true;
}

// Calcular edad a partir de fecha de nacimiento
int calcularEdad(int dia, int mes, int anio) {
    // Obtener fecha actual
    auto now = chrono::system_clock::now();
    time_t tt = chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&tt);

    int diaActual = local_tm.tm_mday;
    int mesActual = local_tm.tm_mon + 1; // tm_mon es 0-11
    int anioActual = local_tm.tm_year + 1900;

    // Calcular edad
    int edad = anioActual - anio;

    // Ajustar si aún no cumplió años este año
    if (mesActual < mes || (mesActual == mes && diaActual < dia)) {
        edad--;
    }

    return edad;
}
// ============= MENÚS CURSES MEJORADOS =============

void menuRegistrarCliente(SistemaAlquiler* sistema) {
    desactivarMouse();

    char nombre[100], apellido[100];
    int dia, mes, anio, edad;

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int menu_ancho = 60;
    int menu_alto = 20;

    int y_inicio = (max_y - menu_alto) / 2;
    int x_inicio = (max_x - menu_ancho) / 2;

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
        curs_set(1);
        mvgetnstr(y_inicio + 2, x_inicio + 8, nombre, 99);
        curs_set(0);
        noecho();

        string nombreStr = trim(string(nombre));

        if (esStringVacio(nombreStr)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 5, x_inicio, "Error: El nombre no puede estar vacio.");
            mvprintw(y_inicio + 7, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else if (!validarSoloLetras(nombreStr)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 5, x_inicio, "Error: El nombre solo puede contener letras.");
            mvprintw(y_inicio + 7, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else if (!validarLongitud(nombreStr, 2, 50)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 5, x_inicio, "Error: El nombre debe tener entre 2 y 50 caracteres.");
            mvprintw(y_inicio + 7, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else {
            strcpy(nombre, nombreStr.c_str());
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
        curs_set(1);
        mvgetnstr(y_inicio + 3, x_inicio + 10, apellido, 99);
        curs_set(0);
        noecho();

        string apellidoStr = trim(string(apellido));

        if (esStringVacio(apellidoStr)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 5, x_inicio, "Error: El apellido no puede estar vacio.");
            mvprintw(y_inicio + 7, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else if (!validarSoloLetras(apellidoStr)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 5, x_inicio, "Error: El apellido solo puede contener letras.");
            mvprintw(y_inicio + 7, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else if (!validarLongitud(apellidoStr, 2, 50)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 5, x_inicio, "Error: El apellido debe tener entre 2 y 50 caracteres.");
            mvprintw(y_inicio + 7, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else {
            strcpy(apellido, apellidoStr.c_str());
            apellidoValido = true;
        }
    }

    // Pedir y validar FECHA DE NACIMIENTO
    bool fechaValida = false;
    while (!fechaValida) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Nombre: %s", nombre);
        mvprintw(y_inicio + 3, x_inicio, "Apellido: %s", apellido);
        mvprintw(y_inicio + 5, x_inicio, "=== FECHA DE NACIMIENTO ===");
        mvprintw(y_inicio + 7, x_inicio, "Dia (1-31): ");
        refresh();

        echo();
        curs_set(1);
        int resultado = mvscanw(y_inicio + 7, x_inicio + 12, const_cast<char*>("%d"), &dia);
        curs_set(0);
        noecho();

        if (resultado != 1) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 9, x_inicio, "Error: Debe ingresar un numero valido.");
            mvprintw(y_inicio + 11, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
            continue;
        }

        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Nombre: %s", nombre);
        mvprintw(y_inicio + 3, x_inicio, "Apellido: %s", apellido);
        mvprintw(y_inicio + 5, x_inicio, "=== FECHA DE NACIMIENTO ===");
        mvprintw(y_inicio + 7, x_inicio, "Dia: %d", dia);
        mvprintw(y_inicio + 8, x_inicio, "Mes (1-12): ");
        refresh();

        echo();
        curs_set(1);
        resultado = mvscanw(y_inicio + 8, x_inicio + 12, const_cast<char*>("%d"), &mes);
        curs_set(0);
        noecho();

        if (resultado != 1) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 10, x_inicio, "Error: Debe ingresar un numero valido.");
            mvprintw(y_inicio + 12, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
            continue;
        }

        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Nombre: %s", nombre);
        mvprintw(y_inicio + 3, x_inicio, "Apellido: %s", apellido);
        mvprintw(y_inicio + 5, x_inicio, "=== FECHA DE NACIMIENTO ===");
        mvprintw(y_inicio + 7, x_inicio, "Dia: %d", dia);
        mvprintw(y_inicio + 8, x_inicio, "Mes: %d", mes);
        mvprintw(y_inicio + 9, x_inicio, "Anio (mayor a 1990): ");
        refresh();

        echo();
        curs_set(1);
        resultado = mvscanw(y_inicio + 9, x_inicio + 21, const_cast<char*>("%d"), &anio);
        curs_set(0);
        noecho();

        if (resultado != 1) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 11, x_inicio, "Error: Debe ingresar un numero valido.");
            mvprintw(y_inicio + 13, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
            continue;
        }

        // Validar fecha
        if (!validarFecha(dia, mes, anio)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 11, x_inicio, "Error: Fecha invalida.");
            mvprintw(y_inicio + 12, x_inicio, "Verifica que el dia, mes y anio sean correctos.");
            mvprintw(y_inicio + 14, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
            continue;
        }

        // Calcular edad
        edad = calcularEdad(dia, mes, anio);

        // Validar edad mínima
        if (!validarEdad(edad)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 11, x_inicio, "Error: Debe ser mayor de 18 anos para alquilar.");
            mvprintw(y_inicio + 12, x_inicio, "Edad calculada: %d anos", edad);
            mvprintw(y_inicio + 14, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
            continue;
        }

        fechaValida = true;
    }

    // Pedir y validar DNI
    int dni;
    bool dniValido = false;
    while (!dniValido) {
        clear();
        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 2, x_inicio, "Nombre: %s", nombre);
        mvprintw(y_inicio + 3, x_inicio, "Apellido: %s", apellido);
        mvprintw(y_inicio + 4, x_inicio, "Fecha Nac: %02d/%02d/%d (Edad: %d anos)", dia, mes, anio, edad);
        mvprintw(y_inicio + 6, x_inicio, "DNI: ");
        refresh();

        echo();
        curs_set(1);
        int resultado = mvscanw(y_inicio + 6, x_inicio + 5, const_cast<char*>("%d"), &dni);
        curs_set(0);
        noecho();

        if (resultado != 1) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 8, x_inicio, "Error: Debe ingresar un numero valido.");
            mvprintw(y_inicio + 10, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
        } else if (!validarDNI(dni)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 8, x_inicio, "Error: El DNI debe tener entre 7 y 8 digitos.");
            mvprintw(y_inicio + 10, x_inicio, "Presiona cualquier tecla para reintentar...");
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
    const char* msg_error = "Error: El DNI ya existe en el sistema.";
    const char* msg_continuar = "Presiona cualquier tecla para continuar...";

    int msg_x;
    if (sistema->registrarCliente(string(nombre), string(apellido), edad, dni)) {
        msg_x = x_inicio + (menu_ancho - strlen(msg_exito)) / 2;
        mvprintw(y_inicio + 5, msg_x, "%s", msg_exito);
        mvprintw(y_inicio + 7, x_inicio, "Nombre: %s %s", nombre, apellido);
        mvprintw(y_inicio + 8, x_inicio, "Fecha Nac: %02d/%02d/%d", dia, mes, anio);
        mvprintw(y_inicio + 9, x_inicio, "Edad: %d anos", edad);
        mvprintw(y_inicio + 10, x_inicio, "DNI: %d", dni);
    } else {
        msg_x = x_inicio + (menu_ancho - strlen(msg_error)) / 2;
        mvprintw(y_inicio + 5, msg_x, "%s", msg_error);
    }

    int continuar_x = x_inicio + (menu_ancho - strlen(msg_continuar)) / 2;
    mvprintw(y_inicio + 12, continuar_x, "%s", msg_continuar);
    refresh();
    getch();

    reactivarMouse();
}

void menuRegistrarVehiculo(SistemaAlquiler* sistema) {
    desactivarMouse();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int menu_ancho = 60;
    int menu_alto = 20;

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
    mvprintw(y_inicio + 5, x_inicio, "Selecciona (1 o 2): ");
    refresh();

    int tipo = getch() - '0';

    if (tipo != 1 && tipo != 2) {
        clear();
        const char* msg_error = "Opcion invalida. Debe ser 1 o 2.";
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
        curs_set(1);
        mvgetnstr(y_inicio + 4, x_inicio + 7, marca, 99);
        curs_set(0);
        noecho();

        string marcaStr = trim(string(marca));

        if (esStringVacio(marcaStr)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 6, x_inicio, "Error: La marca no puede estar vacia.");
            mvprintw(y_inicio + 8, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else if (!validarLongitud(marcaStr, 2, 50)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 6, x_inicio, "Error: La marca debe tener entre 2 y 50 caracteres.");
            mvprintw(y_inicio + 8, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else {
            strcpy(marca, marcaStr.c_str());
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
        mvprintw(y_inicio + 5, x_inicio, "Patente (ej: ABC123): ");
        refresh();

        echo();
        curs_set(1);
        mvgetnstr(y_inicio + 5, x_inicio + 22, patente, 99);
        curs_set(0);
        noecho();

        string patenteStr = trim(string(patente));

        if (esStringVacio(patenteStr)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 7, x_inicio, "Error: La patente no puede estar vacia.");
            mvprintw(y_inicio + 9, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else if (!validarPatente(patenteStr)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 7, x_inicio, "Error: Formato de patente invalido (6-7 caracteres).");
            mvprintw(y_inicio + 9, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            getch();
        } else {
            strcpy(patente, toUpper(patenteStr).c_str());
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
        mvprintw(y_inicio + 6, x_inicio, "Anio (1900-2026): ");
        refresh();

        echo();
        curs_set(1);
        int resultado = mvscanw(y_inicio + 6, x_inicio + 18, const_cast<char*>("%d"), &anio);
        curs_set(0);
        noecho();

        if (resultado != 1) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 8, x_inicio, "Error: Debe ingresar un numero valido.");
            mvprintw(y_inicio + 10, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
        } else if (!validarAnio(anio)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 8, x_inicio, "Error: El anio debe estar entre 1900 y el anio actual.");
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
        mvprintw(y_inicio + 7, x_inicio, "Precio por hora ($): ");
        refresh();

        echo();
        curs_set(1);
        int resultado = mvscanw(y_inicio + 7, x_inicio + 21, const_cast<char*>("%f"), &precio);
        curs_set(0);
        noecho();

        if (resultado != 1) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 9, x_inicio, "Error: Debe ingresar un numero valido.");
            mvprintw(y_inicio + 11, x_inicio, "Presiona cualquier tecla para reintentar...");
            refresh();
            flushinp();
            getch();
        } else if (!validarPrecio(precio)) {
            clear();
            mvprintw(y_inicio, titulo_x, "%s", titulo);
            mvprintw(y_inicio + 9, x_inicio, "Error: El precio debe estar entre $0.01 y $1,000,000.");
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
        mvprintw(y_inicio + 7, x_inicio, "Precio por hora: $%.2f", precio);
        refresh();

        echo();
        curs_set(1);
        int resultado;

        if (tipo == 1) {
            mvprintw(y_inicio + 8, x_inicio, "Numero de puertas (2-5): ");
            resultado = mvscanw(y_inicio + 8, x_inicio + 25, const_cast<char*>("%d"), &extra);
            curs_set(0);
            noecho();

            if (resultado != 1) {
                clear();
                mvprintw(y_inicio, titulo_x, "%s", titulo);
                mvprintw(y_inicio + 10, x_inicio, "Error: Debe ingresar un numero valido.");
                mvprintw(y_inicio + 12, x_inicio, "Presiona cualquier tecla para reintentar...");
                refresh();
                flushinp();
                getch();
            } else if (!validarPuertas(extra)) {
                clear();
                mvprintw(y_inicio, titulo_x, "%s", titulo);
                mvprintw(y_inicio + 10, x_inicio, "Error: El numero de puertas debe estar entre 2 y 5.");
                mvprintw(y_inicio + 12, x_inicio, "Presiona cualquier tecla para reintentar...");
                refresh();
                flushinp();
                getch();
            } else {
                extraValido = true;
                vehiculo = new Auto(string(marca), string(patente), anio, precio, extra);
            }
        } else {
            mvprintw(y_inicio + 8, x_inicio, "Cilindradas (50-2000cc): ");
            resultado = mvscanw(y_inicio + 8, x_inicio + 25, const_cast<char*>("%d"), &extra);
            curs_set(0);
            noecho();

            if (resultado != 1) {
                clear();
                mvprintw(y_inicio, titulo_x, "%s", titulo);
                mvprintw(y_inicio + 10, x_inicio, "Error: Debe ingresar un numero valido.");
                mvprintw(y_inicio + 12, x_inicio, "Presiona cualquier tecla para reintentar...");
                refresh();
                flushinp();
                getch();
            } else if (!validarCilindradas(extra)) {
                clear();
                mvprintw(y_inicio, titulo_x, "%s", titulo);
                mvprintw(y_inicio + 10, x_inicio, "Error: Las cilindradas deben estar entre 50 y 2000cc.");
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
    const char* msg_error = "Error: La patente ya existe en el sistema.";
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
            const char* msg_error = "Error: No se pudo cerrar el contrato. ID no encontrado";
            int msg_x = x_inicio + (menu_ancho - strlen(msg_error)) / 2;
            mvprintw(y_inicio + 4, msg_x, "%s", msg_error);
        }

        mvprintw(y_inicio + 11, continuar_x+2, "%s", msg_continuar);
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
    initscr();
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
                        entrarModoCurses();
                        break;

                    case 3:
                        salirModoCurses();
                        sistema.listarVehiculosDisponibles();
                        entrarModoCurses();
                        break;

                    case 4:
                        salirModoCurses();
                        sistema.listarTodosVehiculos();
                        entrarModoCurses();
                        break;

                    case 5: menuCrearContrato(&sistema); break;
                    case 6: menuCerrarContrato(&sistema); break;

                    case 7:
                        salirModoCurses();
                        sistema.listarContratos();
                        entrarModoCurses();
                        break;

                    case 8:
                        salirModoCurses();
                        sistema.mostrarHistorialCompleto();
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