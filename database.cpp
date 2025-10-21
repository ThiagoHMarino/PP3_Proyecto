#include "database.h"
#include <sqlite3.h>
#include <string>
#include <iostream>

//Constructor
DataBase :: DataBase(string nBD): datab(nullptr), nombreBD(nBD) {
    int retorno = sqlite3_open(nombreBD.c_str(), &datab);

    if(retorno!=SQLITE_OK){ //SQLITE_OK: operacion de exito.
        //hubo error:
        cout << "Error al abrir la BD: " << sqlite3_errmsg(datab) << endl; //sqlite3_errmsg(datab): mensaje de error.
        datab = nullptr;
    }else{
        cout << "BASE DE DATOS: " << nombreBD << " abierta con éxito!" << endl;
    }
}

//Crear tablas
void DataBase::crearTablas() {

    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return;
    }

// Tabla Cliente:
    string sql_cliente = "CREATE TABLE IF NOT EXISTS Cliente ("
        "dni INTEGER PRIMARY KEY, "
        "nombre TEXT NOT NULL, "
        "apellido TEXT NOT NULL, "
        "edad INTEGER NOT NULL"
        ");";

// Tabla Vehiculo:
    string sql_vehiculos = "CREATE TABLE IF NOT EXISTS Vehiculo ("
        "patente TEXT PRIMARY KEY, "
        "marca TEXT NOT NULL,"
        "anio INTEGER NOT NULL,"
        "precioBase DECIMAL NOT NULL,"
        "disponible INTEGER NOT NULL DEFAULT 1, " // 1 : disponible, 0 : no
        "cilindradas INTEGER DEFAULT 0, "      // solo para motos
        "puertas INTEGER DEFAULT 0"            // solo para autos
        ");";

//  Tabla contrato:
    string sql_contrato = "CREATE TABLE IF NOT EXISTS Contrato ("
        "id_contrato INTEGER PRIMARY KEY AUTOINCREMENT, "
        "dni_cliente INTEGER NOT NULL, "
        "patente_vehiculo TEXT NOT NULL, "
        "tiempo_establecido REAL NOT NULL, " //REAL: Variable . flotante de 4 bytes.
        "costo REAL DEFAULT 0, "
        "cargo_extra REAL DEFAULT 0, "
        "activo INTEGER DEFAULT 1, " // 1 = activo, 0 = cerrado
        "FOREIGN KEY(dni_cliente) REFERENCES Cliente(dni), "
         //Le dice a SQLitle, que la columna dni_cliente,
         //debe contener valores existentes de Cliente.
        "FOREIGN KEY(patente_vehiculo) REFERENCES Vehiculo(patente)"
    ");";

    char* errorMsg = nullptr; //puntero a cadena q devuelve SQLite para devolver msj de error.




    cout << "---------------------------------------------" << endl;
    cout << "Todas las tablas fueron verificadas o creadas." << endl;
    cout << "---------------------------------------------" << endl;

}

//Destructor
DataBase :: ~DataBase(){
    if (datab!=nullptr) {
        sqlite3_close(datab);
        cout << "BD cerrada." << endl;
    }
}
