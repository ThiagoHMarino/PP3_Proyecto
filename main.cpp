#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <vector>
#include "Archivo.h"
#include "menu.h"
#include "database.h"
#include "sqlite3.h"

using namespace std;

int main() {
    // Fix para caracteres especiales en Windows
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    cout << "==================================================" << endl;
    cout << "   SISTEMA DE ALQUILER DE VEHICULOS" << endl;
    cout << "==================================================" << endl;
    cout << endl;

    cout << "Iniciando sistema..." << endl;
    cout << "Cargando base de datos..." << endl;
    cout << endl;

    // Inicializar el menú interactivo
    funcion_menu();

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