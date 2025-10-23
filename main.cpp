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

    // FIX para caracteres especiales en Windows
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif

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
    Cliente cliente1("Juan", "Perez", 30, 12345678);
    Cliente cliente2("Maria", "Gonzalez", 25, 87654321);
    Cliente cliente3("Pedro", "Martinez", 40, 11223344);

    bool r1 = db.guardarCliente(cliente1);
    bool r2 = db.guardarCliente(cliente2);
    bool r3 = db.guardarCliente(cliente3);

    cout << "Resultado cliente1: " << (r1 ? "OK - Nuevo cliente insertado" : "INFO - Cliente ya existe") << endl;
    cout << "Resultado cliente2: " << (r2 ? "OK - Nuevo cliente insertado" : "INFO - Cliente ya existe") << endl;
    cout << "Resultado cliente3: " << (r3 ? "OK - Nuevo cliente insertado" : "INFO - Cliente ya existe") << endl;
    cout << endl;

    // 4. Probar búsqueda de cliente
    cout << "4. PROBANDO BUSQUEDA DE CLIENTE..." << endl;
    Cliente* encontrado = db.buscarClientePorDNI(12345678);
    if (encontrado != nullptr) {
        cout << "Cliente encontrado:" << endl;
        encontrado->mostrarInfo();
        delete encontrado;
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
        cout << "ERROR: Se encontro un cliente que no deberia existir" << endl;
        delete noExiste;
    }
    cout << endl;

    // 6. Probar inserción duplicada
    cout << "6. PROBANDO INSERCION DUPLICADA..." << endl;
    Cliente clienteDuplicado("Juan", "Perez", 30, 12345678);
    bool duplicado = db.guardarCliente(clienteDuplicado);
    cout << "Resultado (deberia ser false): " << (duplicado ? "ERROR - Se inserto duplicado" : "OK - Se rechazo duplicado") << endl;
    cout << endl;

    // 7. Cargar todos los clientes
    cout << "7. CARGANDO TODOS LOS CLIENTES..." << endl;
    vector<Cliente*> todos = db.cargarClientes();
    cout << "Clientes cargados: " << todos.size() << endl;
    for (Cliente* c : todos) {
        c->mostrarInfo();
        delete c;
    }
    cout << endl;

    // 8. Probar inserción de vehículos
    cout << "8. PROBANDO INSERCION DE VEHICULOS..." << endl;

    // Crear una moto
    Moto* moto1 = new Moto("Honda", "ABC123", 2020, 500.0, 150);
    bool rm = db.guardarVehiculo(moto1);
    cout << "Resultado moto: " << (rm ? "OK - Moto insertada" : "INFO - Moto ya existe") << endl;

    // Crear un auto
    Auto* auto1 = new Auto("Toyota", "XYZ789", 2022, 1000.0, 4);
    bool ra = db.guardarVehiculo(auto1);
    cout << "Resultado auto: " << (ra ? "OK - Auto insertado" : "INFO - Auto ya existe") << endl;

    // Crear más vehículos para pruebas
    Moto* moto2 = new Moto("Yamaha", "DEF456", 2021, 600.0, 200);
    db.guardarVehiculo(moto2);

    Auto* auto2 = new Auto("Ford", "GHI789", 2023, 1200.0, 5);
    db.guardarVehiculo(auto2);
    cout << endl;

    // 9. Cargar todos los vehículos
    cout << "9. CARGANDO TODOS LOS VEHICULOS..." << endl;
    vector<Vehiculo*> vehiculos = db.cargarVehiculos();
    cout << "Vehiculos cargados: " << vehiculos.size() << endl;
    for (Vehiculo* v : vehiculos) {
        v->mostrarInfo();
        cout << endl;
        delete v;
    }
    cout << endl;

    // 10. Probar actualización de disponibilidad
    cout << "10. PROBANDO ACTUALIZACION DE DISPONIBILIDAD..." << endl;
    bool act = db.actualizarDisponibilidadVehiculo("ABC123", false);
    cout << "Resultado actualizacion: " << (act ? "OK" : "ERROR") << endl;

    // Verificar el cambio
    cout << "Verificando cambio de disponibilidad..." << endl;
    vector<Vehiculo*> vehiculos_check = db.cargarVehiculos();
    for (Vehiculo* v : vehiculos_check) {
        if (v->getPatente() == "ABC123") {
            cout << "Patente ABC123 - Disponible: " << (v->getActivo() ? "SI" : "NO") << endl;
        }
        delete v;
    }
    cout << endl;

    // 11. Probar inserción duplicada de vehículo
    cout << "11. PROBANDO INSERCION DUPLICADA DE VEHICULO..." << endl;
    Moto* motoDuplicada = new Moto("Honda", "ABC123", 2020, 500.0, 150);
    bool dupVeh = db.guardarVehiculo(motoDuplicada);
    cout << "Resultado (deberia ser false): " << (dupVeh ? "ERROR - Se inserto duplicado" : "OK - Se rechazo duplicado") << endl;
    delete motoDuplicada;
    cout << endl;

    // 12. Estadísticas finales
    cout << "==================================================" << endl;
    cout << "ESTADISTICAS FINALES" << endl;
    cout << "==================================================" << endl;

    vector<Cliente*> clientesFinales = db.cargarClientes();
    cout << "Total de clientes en BD: " << clientesFinales.size() << endl;
    for (Cliente* c : clientesFinales) {
        delete c;
    }

    vector<Vehiculo*> vehiculosFinales = db.cargarVehiculos();
    cout << "Total de vehiculos en BD: " << vehiculosFinales.size() << endl;

    int disponibles = 0;
    int noDisponibles = 0;
    for (Vehiculo* v : vehiculosFinales) {
        if (v->getActivo()) {
            disponibles++;
        } else {
            noDisponibles++;
        }
        delete v;
    }
    cout << "  - Disponibles: " << disponibles << endl;
    cout << "  - No disponibles: " << noDisponibles << endl;
    cout << endl;

    // 13. Resumen final
    cout << "==================================================" << endl;
    cout << "PRUEBAS COMPLETADAS" << endl;
    cout << "==================================================" << endl;
    cout << "El archivo 'alquiler.db' se encuentra en:" << endl;
    cout << "  - Windows: Directorio del ejecutable (.exe)" << endl;
    cout << "  - Normalmente: cmake-build-debug/" << endl;
    cout << endl;
    cout << "Puedes abrirlo con:" << endl;
    cout << "  - DB Browser for SQLite (recomendado)" << endl;
    cout << "  - SQLite Studio" << endl;
    cout << "  - Comando sqlite3 en terminal" << endl;
    cout << endl;

    // Limpiar memoria de los vehículos creados
    delete moto1;
    delete auto1;
    delete moto2;
    delete auto2;

    return 0;
}