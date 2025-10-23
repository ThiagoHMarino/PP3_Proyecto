#include "database.h"
#include <sqlite3.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

//=============================================================================
// CONSTRUCTOR
//=============================================================================
// Abre la conexión con la base de datos SQLite
// Si el archivo no existe, lo crea automáticamente
DataBase::DataBase(string nBD): datab(nullptr), nombreBD(nBD) {
    int retorno = sqlite3_open(nombreBD.c_str(), &datab);
    /*
    .c_str() se utiliza para convertir el string en un
    array de caracteres, ya que sqlite3_open() espera un char* y no un string.
    */

    if(retorno != SQLITE_OK) { //SQLITE_OK: operacion de exito.
        //hubo error:
        cout << "Error al abrir la BD: " << sqlite3_errmsg(datab) << endl; //sqlite3_errmsg(datab): mensaje de error.
        datab = nullptr;
    } else {
        cout << "BASE DE DATOS: " << nombreBD << " abierta con éxito!" << endl;
    }
}

//=============================================================================
// CREAR TABLAS
//=============================================================================
// Primera ejecucion se crean las tablas.
// Ejecuciones posteriores se verifican que existen gracias al 'IF NOT EXISTS'
void DataBase::crearTablas() {

    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return;
    }

    // Tabla Cliente: almacena los datos de los clientes registrados
    string sql_cliente = "CREATE TABLE IF NOT EXISTS Cliente ("
        "dni INTEGER PRIMARY KEY, "
        "nombre TEXT NOT NULL, "
        "apellido TEXT NOT NULL, "
        "edad INTEGER NOT NULL"
        ");";

    // Tabla Vehiculo: almacena todos los vehículos (autos y motos)
    string sql_vehiculos = "CREATE TABLE IF NOT EXISTS Vehiculo ("
        "patente TEXT PRIMARY KEY, "
        "marca TEXT NOT NULL,"
        "anio INTEGER NOT NULL,"
        "precioBase DECIMAL NOT NULL,"
        "disponible INTEGER NOT NULL DEFAULT 1, " // 1 : disponible, 0 : no
        "cilindradas INTEGER DEFAULT 0, "      // solo para motos
        "puertas INTEGER DEFAULT 0"            // solo para autos
        ");";

    // Tabla contrato: registra todos los contratos realizados
    string sql_contrato = "CREATE TABLE IF NOT EXISTS Contrato ("
        "id_contrato INTEGER PRIMARY KEY AUTOINCREMENT, " //IDs consecutivos.
        "dni_cliente INTEGER NOT NULL, "
        "patente_vehiculo TEXT NOT NULL, "
        "tiempo_establecido REAL NOT NULL, " //REAL: Variable flotante de 4 bytes.
        "costo REAL DEFAULT 0, "
        "cargo_extra REAL DEFAULT 0, "
        "activo INTEGER DEFAULT 1, " // 1 = activo, 0 = cerrado
        "FOREIGN KEY(dni_cliente) REFERENCES Cliente(dni), "
         //Le dice a SQLite, que la columna dni_cliente,
         //debe contener valores existentes de Cliente.
        "FOREIGN KEY(patente_vehiculo) REFERENCES Vehiculo(patente)"
    ");";

    char* errorMsg = nullptr; //puntero a cadena que devuelve SQLite para devolver msj de error.
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

    if(sqlite3_exec(datab, sql_cliente.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        cout << "Error creando tabla Cliente: " << errorMsg << endl;
        sqlite3_free(errorMsg); //libera la memoria que SQLite usó para resolver el mensaje.
    } else {
        cout << "Tabla Cliente verificada y creada correctamente." << endl;
    }

    if(sqlite3_exec(datab, sql_vehiculos.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        cout << "Error creando tabla Vehiculos: " << errorMsg << endl;
        sqlite3_free(errorMsg);
    } else {
        cout << "Tabla Vehiculos verificada y creada correctamente." << endl;
    }

    if(sqlite3_exec(datab, sql_contrato.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        cout << "Error creando tabla Contrato: " << errorMsg << endl;
        sqlite3_free(errorMsg);
    } else {
        cout << "Tabla Contrato verificada y creada correctamente." << endl;
    }

    cout << "---------------------------------------------" << endl;
    cout << "Todas las tablas fueron verificadas o creadas." << endl;
    cout << "---------------------------------------------" << endl;
}

//=============================================================================
// MÉTODOS PARA CLIENTE
//=============================================================================

// Guarda un cliente en la base de datos
// Retorna true si se guardó exitosamente, false si ya existía o hubo error
bool DataBase::guardarCliente(Cliente cliente) {

    if (datab == nullptr) { // Chequeo si existe la BD
        cout << "Error: Base de datos no inicializada." << endl;
        return false;
    }

    // Verifica si el cliente existe en la tabla.
    Cliente *existe = buscarClientePorDNI(cliente.getDni());
    if(existe != nullptr) {
        delete existe;
        cout << "El cliente de DNI: " << cliente.getDni() << " ya se encuentra en la BD" << endl;
        return false;
    }

    // Si no existe, preparo la consulta SQL para insertar
    string sql = "INSERT INTO Cliente (dni, nombre, apellido, edad) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt; //Sentencia preparada. 'Compilar' tu SQL antes de ejecutarlo.

    /*
    sqlite3_prepare_v2() recibe 5 parametros:
    - Puntero a la bd.
    - Tu consulta SQL. -> el INSERT
    - Tamaño del SQL, -1 detecta automáticamente hasta el \0
    - Sentencia preparada
    - Puntero al SQL no usado (rara vez se usa)
    */

    // Verifico que se pueda agregar a la tabla:
    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando SQL: " << sqlite3_errmsg(datab) << endl;
        return false;
    }

    // Agrego a la tabla los datos reemplazando los marcadores ? por los datos
    // SQLITE_TRANSIENT indica que SQLite debe hacer una copia del string
    sqlite3_bind_int(stmt, 1, cliente.getDni());//posicion 1
    sqlite3_bind_text(stmt, 2, cliente.getNombre().c_str(), -1, SQLITE_TRANSIENT);//posicion 2
    sqlite3_bind_text(stmt, 3, cliente.getApellido().c_str(), -1, SQLITE_TRANSIENT);//posicion 3
    sqlite3_bind_int(stmt, 4, cliente.getEdad());//posicion 4

    // Ejecuto la sentencia preparada
    // Chequeo que el cliente se haya guardado de manera satisfactoria
    bool resultado = (sqlite3_step(stmt) == SQLITE_DONE); //retorna SQLITE_DONE si se insertó correctamente.

    // Libera los recursos usados por la estructura
    sqlite3_finalize(stmt);

    if (resultado) { //si es true
        cout << "Cliente guardado: " << cliente.getNombre() << " " << cliente.getApellido() << endl;
    }

    return resultado;
}

// Busca un cliente por DNI en la base de datos
// Retorna un puntero al Cliente si lo encuentra, nullptr si no existe
// IMPORTANTE: El que llama a esta función es responsable de liberar la memoria con delete
Cliente* DataBase::buscarClientePorDNI(int dni) {
    if(datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return nullptr;
    }

    // Consulta SQL para buscar cliente por DNI
    string sql = "SELECT dni, nombre, apellido, edad FROM Cliente WHERE dni = ?;";
    sqlite3_stmt* stmt;

    // Preparo la consulta
    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando búsqueda: " << sqlite3_errmsg(datab) << endl;
        return nullptr;
    }

    // Asigno el DNI a buscar en el marcador ?
    sqlite3_bind_int(stmt, 1, dni);

    // Ejecuto la consulta
    // sqlite3_step() retorna SQLITE_ROW si encontró una fila
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Extraigo los datos de la fila encontrada
        // sqlite3_column_int() obtiene un entero de la columna indicada (0, 1, 2...)
        // sqlite3_column_text() obtiene texto, lo convierto a string con reinterpret_cast
        int dni_encontrado = sqlite3_column_int(stmt, 0);
        string nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string apellido = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int edad = sqlite3_column_int(stmt, 3);

        // Creo un nuevo objeto Cliente con los datos encontrados
        Cliente* cliente = new Cliente(nombre, apellido, edad, dni_encontrado);

        sqlite3_finalize(stmt);
        return cliente;
    }

    // No se encontró el cliente
    sqlite3_finalize(stmt);
    return nullptr;
}

// Carga todos los clientes de la base de datos
// Retorna un vector con punteros a todos los clientes
vector<Cliente*> DataBase::cargarClientes() {
    vector<Cliente*> clientes;

    if(datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return clientes;
    }

    // Consulta SQL para obtener todos los clientes
    string sql = "SELECT dni, nombre, apellido, edad FROM Cliente;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando carga de clientes: " << sqlite3_errmsg(datab) << endl;
        return clientes;
    }

    // Itero sobre todas las filas encontradas
    // Mientras sqlite3_step() retorne SQLITE_ROW, hay más filas
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int dni = sqlite3_column_int(stmt, 0);
        string nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string apellido = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int edad = sqlite3_column_int(stmt, 3);

        // Agrego el nuevo cliente al vector
        clientes.push_back(new Cliente(nombre, apellido, edad, dni));
    }

    sqlite3_finalize(stmt);
    cout << "Se cargaron " << clientes.size() << " clientes." << endl;
    return clientes;
}

//=============================================================================
// MÉTODOS PARA VEHÍCULOS
//=============================================================================

// Guarda un vehículo (Auto o Moto) en la base de datos
// Determina automáticamente el tipo según los atributos
bool DataBase::guardarVehiculo(Vehiculo* vehiculo) {
    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return false;
    }

    // Intento convertir a Moto primero
    Moto* moto = dynamic_cast<Moto*>(vehiculo);
    if (moto != nullptr) {
        return guardarMoto(moto);
    }

    // Si no es Moto, intento convertir a Auto
    Auto* automovil = dynamic_cast<Auto*>(vehiculo);
    if (automovil != nullptr) {
        return guardarAuto(automovil);
    }

    cout << "Error: Tipo de vehículo no reconocido." << endl;
    return false;
}

// Guarda una Moto específicamente
bool DataBase::guardarMoto(Moto* moto) {
    string sql = "INSERT INTO Vehiculo (patente, marca, anio, precioBase, disponible, cilindradas, puertas) "
                 "VALUES (?, ?, ?, ?, ?, ?, 0);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando inserción de moto: " << sqlite3_errmsg(datab) << endl;
        return false;
    }

    // Asigno los valores a los marcadores ?
    sqlite3_bind_text(stmt, 1, moto->getPatente().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, moto->getMarca().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, moto->getAnio());
    sqlite3_bind_double(stmt, 4, moto->getPrecioBase());
    sqlite3_bind_int(stmt, 5, moto->getActivo() ? 1 : 0);
    sqlite3_bind_int(stmt, 6, moto->getCilindradas());


    bool resultado = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (resultado) {
        cout << "Moto guardada: " << moto->getPatente() << endl;
    }

    return resultado;
}

// Guarda un Auto específicamente
bool DataBase::guardarAuto(Auto* automovil) {
    string sql = "INSERT INTO Vehiculo (patente, marca, anio, precioBase, disponible, cilindradas, puertas) "
                 "VALUES (?, ?, ?, ?, ?, 0, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando inserción de auto: " << sqlite3_errmsg(datab) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, automovil->getPatente().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, automovil->getMarca().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, automovil->getAnio());
    sqlite3_bind_double(stmt, 4, automovil->getPrecioBase());
    sqlite3_bind_int(stmt, 5, automovil->getActivo() ? 1 : 0);
    sqlite3_bind_int(stmt, 6, automovil->getPuertas());

    bool resultado = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (resultado) {
        cout << "Auto guardado: " << automovil->getPatente() << endl;
    }

    return resultado;
}

// Actualiza la disponibilidad de un vehículo en la BD
// Útil cuando se inicia o finaliza un contrato
bool DataBase::actualizarDisponibilidadVehiculo(string patente, bool disponible) {
    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return false;
    }

    string sql = "UPDATE Vehiculo SET disponible = ? WHERE patente = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando actualización: " << sqlite3_errmsg(datab) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, disponible ? 1 : 0);
    sqlite3_bind_text(stmt, 2, patente.c_str(), -1, SQLITE_TRANSIENT);

    bool resultado = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (resultado) {
        cout << "Disponibilidad actualizada para: " << patente << endl;
    }

    return resultado;
}

//=============================================================================
// MÉTODOS PARA CONTRATOS
//=============================================================================

// Guarda un contrato en la base de datos
// Registra el alquiler de un vehículo por un cliente
bool DataBase::guardarContrato(Contrato contrato) {
    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return false;
    }

    // Nota: Este método guarda el contrato inicial
    // Los campos costo y cargo_extra se actualizarán cuando se cierre el contrato
    string sql = "INSERT INTO Contrato (dni_cliente, patente_vehiculo, tiempo_establecido, activo) "
                 "VALUES (?, ?, ?, 1);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando inserción de contrato: " << sqlite3_errmsg(datab) << endl;
        return false;
    }

    // Obtengo el DNI del cliente y la patente del vehículo del contrato
    Cliente cliente = contrato.getCliente();
    // Necesitarías un método getVehiculo() en Contrato para obtener la patente
    // Por ahora asumo que puedes acceder a estos datos

    sqlite3_bind_int(stmt, 1, cliente.getDni());
    // sqlite3_bind_text(stmt, 2, contrato.getVehiculo()->getPatente().c_str(), -1, SQLITE_TRANSIENT);
    // sqlite3_bind_double(stmt, 3, contrato.getTiempoEstablecido());

    bool resultado = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (resultado) {
        cout << "Contrato guardado exitosamente." << endl;
    }

    return resultado;
}

// Finaliza un contrato actualizando su costo final y marcándolo como inactivo
bool DataBase::finalizarContrato(int id_contrato, float costo_final) {
    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return false;
    }

    string sql = "UPDATE Contrato SET costo = ?, activo = 0 WHERE id_contrato = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando finalización: " << sqlite3_errmsg(datab) << endl;
        return false;
    }

    sqlite3_bind_double(stmt, 1, costo_final);
    sqlite3_bind_int(stmt, 2, id_contrato);

    bool resultado = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (resultado) {
        cout << "Contrato #" << id_contrato << " finalizado." << endl;
    }

    return resultado;
}

// Carga todos los contratos (historial completo) de la base de datos
// Retorna un vector con punteros a todos los contratos
// NOTA: Esta función es simplificada porque necesitaría reconstruir objetos complejos
// Se recomienda usarla junto con buscarClientePorDNI para obtener el cliente completo
vector<Contrato*> DataBase::cargarHistorial() {
    vector<Contrato*> historial;

    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return historial;
    }

    // Consulta que une las tablas para obtener toda la información necesaria
    string sql = "SELECT c.id_contrato, c.dni_cliente, c.patente_vehiculo, "
                 "c.tiempo_establecido, c.costo, c.cargo_extra, c.activo, "
                 "cl.nombre, cl.apellido, cl.edad "
                 "FROM Contrato c "
                 "INNER JOIN Cliente cl ON c.dni_cliente = cl.dni "
                 "ORDER BY c.id_contrato;";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando carga de historial: " << sqlite3_errmsg(datab) << endl;
        return historial;
    }

    // Itero sobre todos los contratos encontrados
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Extraigo los datos del contrato
        int id_contrato = sqlite3_column_int(stmt, 0);
        int dni = sqlite3_column_int(stmt, 1);
        string patente = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        float tiempo = sqlite3_column_double(stmt, 3);
        float costo = sqlite3_column_double(stmt, 4);
        float cargo_extra = sqlite3_column_double(stmt, 5);
        int activo = sqlite3_column_int(stmt, 6);

        // Datos del cliente
        string nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        string apellido = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        int edad = sqlite3_column_int(stmt, 9);

        // Creo el objeto Cliente
        Cliente cliente(nombre, apellido, edad, dni);

        // NOTA: Aquí necesitarías buscar el vehículo por patente
        // Por ahora lo dejo como nullptr, deberías implementar buscarVehiculoPorPatente()
        // Vehiculo* vehiculo = buscarVehiculoPorPatente(patente);

        // Como no podemos crear el contrato completo sin el vehículo,
        // esta función necesita ser completada cuando implementes buscarVehiculoPorPatente()

        cout << "Contrato #" << id_contrato << " - Cliente: " << nombre << " " << apellido
             << " - Vehículo: " << patente << " - Estado: " << (activo ? "Activo" : "Cerrado") << endl;
    }

    sqlite3_finalize(stmt);
    cout << "Historial cargado: " << historial.size() << " contratos." << endl;
    return historial;
}

// Carga todos los contratos de un cliente específico
// Útil para ver el historial de alquileres de un cliente
vector<Contrato*> DataBase::cargarHistorialPorCliente(int dni) {
    vector<Contrato*> historial;

    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return historial;
    }

    string sql = "SELECT c.id_contrato, c.dni_cliente, c.patente_vehiculo, "
                 "c.tiempo_establecido, c.costo, c.cargo_extra, c.activo, "
                 "cl.nombre, cl.apellido, cl.edad "
                 "FROM Contrato c "
                 "INNER JOIN Cliente cl ON c.dni_cliente = cl.dni "
                 "WHERE c.dni_cliente = ? "
                 "ORDER BY c.id_contrato;";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando carga de historial por cliente: " << sqlite3_errmsg(datab) << endl;
        return historial;
    }

    // Asigno el DNI del cliente a buscar
    sqlite3_bind_int(stmt, 1, dni);

    // Itero sobre todos los contratos del cliente
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id_contrato = sqlite3_column_int(stmt, 0);
        string patente = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        float tiempo = sqlite3_column_double(stmt, 3);
        float costo = sqlite3_column_double(stmt, 4);
        int activo = sqlite3_column_int(stmt, 6);

        string nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        string apellido = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));

        cout << "Contrato #" << id_contrato << " - Cliente: " << nombre << " " << apellido
             << " - Vehículo: " << patente << " - Costo: $" << costo
             << " - Estado: " << (activo ? "Activo" : "Cerrado") << endl;
    }

    sqlite3_finalize(stmt);
    cout << "Contratos encontrados para DNI " << dni << ": " << historial.size() << endl;
    return historial;
}

// Carga solo los contratos que están actualmente activos (no finalizados)
// Útil para saber qué vehículos están siendo alquilados en este momento
vector<Contrato*> DataBase::cargarContratosActivos() {
    vector<Contrato*> activos;

    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return activos;
    }

    string sql = "SELECT c.id_contrato, c.dni_cliente, c.patente_vehiculo, "
                 "c.tiempo_establecido, cl.nombre, cl.apellido "
                 "FROM Contrato c "
                 "INNER JOIN Cliente cl ON c.dni_cliente = cl.dni "
                 "WHERE c.activo = 1 "
                 "ORDER BY c.id_contrato;";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando carga de contratos activos: " << sqlite3_errmsg(datab) << endl;
        return activos;
    }

    cout << "CONTRATOS ACTIVOS:" << endl;
    cout << "==================" << endl;

    // Itero sobre todos los contratos activos
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id_contrato = sqlite3_column_int(stmt, 0);
        int dni = sqlite3_column_int(stmt, 1);
        string patente = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        float tiempo = sqlite3_column_double(stmt, 3);
        string nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        string apellido = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        cout << "Contrato #" << id_contrato << " - " << nombre << " " << apellido
             << " (DNI: " << dni << ") - Vehículo: " << patente
             << " - Tiempo: " << (tiempo / 3600.0) << " horas" << endl;
    }

    sqlite3_finalize(stmt);
    cout << "Total de contratos activos: " << activos.size() << endl;
    return activos;
}

//=============================================================================
// DESTRUCTOR
//=============================================================================
// Cierra la conexión con la base de datos
DataBase::~DataBase() {
    if (datab != nullptr) {
        sqlite3_close(datab);
        cout << "BD cerrada." << endl;
    }
}