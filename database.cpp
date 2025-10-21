#include "database.h"
#include <sqlite3.h>
#include <string>
#include <iostream>

//Constructor
DataBase :: DataBase(string nBD): datab(nullptr), nombreBD(nBD) {
    int retorno = sqlite3_open(nombreBD.c_str(), &datab);
    /*
    .c_str() se utiliza para convertir el string en un
    array de caracteres, ya que sqlite3_open() espera un char* y no un string.
    */

    if(retorno!=SQLITE_OK){ //SQLITE_OK: operacion de exito.
        //hubo error:
        cout << "Error al abrir la BD: " << sqlite3_errmsg(datab) << endl; //sqlite3_errmsg(datab): mensaje de error.
        datab = nullptr;
    }else{
        cout << "BASE DE DATOS: " << nombreBD << " abierta con éxito!" << endl;
    }
}

// Crear tablas
// Primera ejecucion se crean las tablas.
// Ejecuciones posteriores se verifican que existen gracias al 'IF NOT EXISTS'
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
    // Si la consulta sale mal -> apunta a una cadena.
    // Si la consulta sale bien -> sigue apuntando a nullptr.

    /*
    sqlite3_exec() recibe 5 parametros:
    - Puntero a la base de datos.
    - Sentencia SQL a ejecutar.
    - Funcion para procesar resultados. callback (se usa en consultas con SELECT.)
    - Datos que pasas al callback.
    - Mensaje de error (si ocurre).

    Devuelve un entero del 0 al 8. 0->BIEN , el resto son distintos inconvenientes.
    */

    if(sqlite3_exec(datab, sql_cliente.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK){
        cout << "Error creando tabla Cliente: " << errorMsg << endl;
        sqlite3_free(errorMsg); //libera la memoria que SQLite uso para resolver el mensaje.
    }else{
        cout << "Tabla Cliente verificada y creada correctamente." << endl;
    }

    if(sqlite3_exec(datab, sql_vehiculos.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK){
        cout << "Error creando tabla Vehiculos: " << errorMsg << endl;
        sqlite3_free(errorMsg);
    }else{
        cout << "Tabla Vehiculos verificada y creada correctamente." << endl;
    }

    if(sqlite3_exec(datab, sql_contrato.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK){
        cout << "Error creando tabla Contrato: " << errorMsg << endl;
        sqlite3_free(errorMsg);
    }else{
        cout << "Tabla Contrato verificada y creada correctamente." << endl;
    }

    cout << "---------------------------------------------" << endl;
    cout << "Todas las tablas fueron verificadas o creadas." << endl;
    cout << "---------------------------------------------" << endl;

}

bool DataBase :: tablasExisten(){}

//Metodos para Cliente
bool DataBase :: guardarCliente(Cliente cliente){

    if (datab == nullptr) { // Chequeo si existe la BD
        cout << "Error: Base de datos no inicializada." << endl;
        return;
    }

    //Verifica si el cliente existe en la tabla.
    Cliente *existe = buscarClientePorDNI(cliente.getDni());
    if(existe!=nullptr){
    delete existe;
    cout<<"El cliente de DNI: "<<cliente.getDni<<" Ya se encuentra en la BD"<<endl;
    return false;
    }
    //Si no existe:
    string sql = "INSERT INTO Cliente (dni, nombre, apellido, edad) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt; //Sentencia preparada. 'Compilar' tu SQL antes de ejecutarlo.

    /*
    sqlite3_prepare_v2() recibe 5 parametros:
    - Puntero a la bd.
    - Tu consulta SQL. -> el INSERT
    - Tamaño del SQL, detectar hasta el /0
    - Sentencia preparada
    - Puntero al SQL no usado (rara vez se usa)
    */

//Verifico que se pueda agregar a la tabla:
    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando SQL: " << sqlite3_errmsg(datab) << endl;
        return false;
    }

// Agrego a la tabla los datos reemplazando los marcadores ? por los datos
    sqlite3_bind_int(stmt, 1, cliente.getDni());//posicion 1
    sqlite3_bind_text(stmt, 2, cliente.getNombre().c_str(), -1, SQLITE_TRANSIENT);//posicion 2
    sqlite3_bind_text(stmt, 3, cliente.getApellido().c_str(), -1, SQLITE_TRANSIENT);//posicion 3
    sqlite3_bind_int(stmt, 4, cliente.getEdad());//posicion 4

//Chequeo que el cliente se haya guardado de manera santisfactoria
    bool resultado = (sqlite3_step(stmt) == SQLITE_DONE); //retorna SQLITE_DONE si se inserto correctamente.

    //libera los recursos usados por la estructura
    sqlite3_finalize(stmt);

    if (resultado) { //si es true
        cout << "Cliente guardado: " << cliente.getNombre() << " " << cliente.getApellido() << endl;
    }

    return resultado;
}

bool DataBase :: buscarClientePorDNI(int dni){
    if(datab == nullptr) return nullptr

//Destructor
DataBase :: ~DataBase(){
    if (datab!=nullptr) {
        sqlite3_close(datab);
        cout << "BD cerrada." << endl;
    }
}
