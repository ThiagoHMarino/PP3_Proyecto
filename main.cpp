#include <iostream>
#include <vector>
#include "Archivo.h"
#include "menu.h"
#include "database.h"
#include "sqlite3.h"

using namespace std;

int main() {
    //funcion_menu();

    cout << "==================================================" << endl;
    cout << "PRUEBA DE BASE DE DATOS - SISTEMA DE ALQUILER" << endl;
    cout << "==================================================" << endl;
    cout << endl;

    // 1. Crear/abrir la base de datos
    cout << "1. CREANDO BASE DE DATOS..." << endl;
    DataBase db("alquiler.db");
    cout << endl;

    // 2. Crear las tablas
    cout << "2. CREANDO TABLAS..." << endl;
    db.crearTablas();
    cout << endl;

    // 3. Probar inserción de clientes
    cout << "3. PROBANDO INSERCION DE CLIENTES..." << endl;
    Cliente cliente1("Juan", "Pérez", 30, 12345678);
    Cliente cliente2("María", "González", 25, 87654321);
    Cliente cliente3("Pedro", "Martínez", 40, 11223344);

    bool r1 = db.guardarCliente(cliente1);
    bool r2 = db.guardarCliente(cliente2);
    bool r3 = db.guardarCliente(cliente3);

    cout << "Resultado cliente1: " << (r1 ? "OK" : "ERROR") << endl;
    cout << "Resultado cliente2: " << (r2 ? "OK" : "ERROR") << endl;
    cout << "Resultado cliente3: " << (r3 ? "OK" : "ERROR") << endl;
    cout << endl;

    // 4. Probar búsqueda de cliente
    cout << "4. PROBANDO BUSQUEDA DE CLIENTE..." << endl;
    Cliente* encontrado = db.buscarClientePorDNI(12345678);
    if (encontrado != nullptr) {
        cout << "Cliente encontrado:" << endl;
        encontrado->mostrarInfo();
        delete encontrado; // ¡IMPORTANTE! Liberar memoria
    } else {
        cout << "Cliente no encontrado." << endl;
    }
    cout << endl;

    // 5. Probar búsqueda de cliente inexistente
    cout << "5. PROBANDO BUSQUEDA DE CLIENTE INEXISTENTE..." << endl;
    Cliente* noExiste = db.buscarClientePorDNI(99999999);
    if (noExiste == nullptr) {
        cout << "Correcto: Cliente no encontrado (como se esperaba)" << endl;
    } else {
        cout << "ERROR: Se encontró un cliente que no debería existir" << endl;
        delete noExiste;
    }
    cout << endl;

    // 6. Probar inserción duplicada
    cout << "6. PROBANDO INSERCION DUPLICADA..." << endl;
    Cliente clienteDuplicado("Juan", "Pérez", 30, 12345678);
    bool duplicado = db.guardarCliente(clienteDuplicado);
    cout << "Resultado (debería ser false): " << (duplicado ? "ERROR - Se insertó duplicado" : "OK - Se rechazó duplicado") << endl;
    cout << endl;

    // 7. Cargar todos los clientes
    cout << "7. CARGANDO TODOS LOS CLIENTES..." << endl;
    vector<Cliente*> todos = db.cargarClientes();
    cout << "Clientes cargados: " << todos.size() << endl;
    for (Cliente* c : todos) {
        c->mostrarInfo();
        delete c; // ¡IMPORTANTE! Liberar memoria
    }
    cout << endl;

    // 8. Probar inserción de vehículos
    cout << "8. PROBANDO INSERCION DE VEHICULOS..." << endl;

    // Crear una moto
    Moto* moto1 = new Moto("Honda", "ABC123", 2020, 500.0, 150);
    bool rm = db.guardarVehiculo(moto1);
    cout << "Resultado moto: " << (rm ? "OK" : "ERROR") << endl;

    // Crear un auto
    Auto* auto1 = new Auto("Toyota", "XYZ789", 2022, 1000.0, 4);
    bool ra = db.guardarVehiculo(auto1);
    cout << "Resultado auto: " << (ra ? "OK" : "ERROR") << endl;
    cout << endl;

    // 9. Probar actualización de disponibilidad
    cout << "9. PROBANDO ACTUALIZACION DE DISPONIBILIDAD..." << endl;
    bool act = db.actualizarDisponibilidadVehiculo("ABC123", false);
    cout << "Resultado actualización: " << (act ? "OK" : "ERROR") << endl;
    cout << endl;

    // 10. Resumen final
    cout << "==================================================" << endl;
    cout << "PRUEBAS COMPLETADAS" << endl;
    cout << "==================================================" << endl;
    cout << "Verifica el archivo 'alquiler.db' en tu directorio." << endl;
    cout << "Puedes abrirlo con SQLite Browser o similar para ver los datos." << endl;
    cout << endl;

    // Limpiar memoria
    delete moto1;
    delete auto1;

    return 0;
}
