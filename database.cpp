#include "database.h"
#include <sqlite3.h>
#include <string>
#include <iostream>
#include <vector>
#include <ncurses.h>
#include <algorithm>
#include <cctype>
#include <string.h>

using namespace std;

//=============================================================================
// CONSTRUCTOR
//=============================================================================
DataBase::DataBase(string nBD): datab(nullptr), nombreBD(nBD) {
    int retorno = sqlite3_open(nombreBD.c_str(), &datab);

    if(retorno != SQLITE_OK) {
        cout << "Error al abrir la BD: " << sqlite3_errmsg(datab) << endl;
        datab = nullptr;
    } else {
        // Configurar timeout para esperar si la BD está bloqueada (10 segundos)
        sqlite3_busy_timeout(datab, 10000);

        // Habilitar WAL mode para mejor concurrencia
        char* errorMsg = nullptr;
        sqlite3_exec(datab, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &errorMsg);
        if (errorMsg) {
            sqlite3_free(errorMsg);
        }

        cout << "BASE DE DATOS: " << nombreBD << " abierta con éxito!" << endl;
    }
}

//=============================================================================
// CREAR TABLAS
//=============================================================================
void DataBase::crearTablas() {
    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return;
    }

    string sql_cliente = "CREATE TABLE IF NOT EXISTS Cliente ("
        "dni INTEGER PRIMARY KEY, "
        "nombre TEXT NOT NULL, "
        "apellido TEXT NOT NULL, "
        "edad INTEGER NOT NULL"
        ");";

    string sql_vehiculos = "CREATE TABLE IF NOT EXISTS Vehiculo ("
        "patente TEXT PRIMARY KEY, "
        "marca TEXT NOT NULL,"
        "anio INTEGER NOT NULL,"
        "precioBase REAL NOT NULL,"
        "disponible INTEGER NOT NULL DEFAULT 1, "
        "cilindradas INTEGER DEFAULT 0, "
        "puertas INTEGER DEFAULT 0"
        ");";

    string sql_contrato = "CREATE TABLE IF NOT EXISTS Contrato ("
        "id_contrato INTEGER PRIMARY KEY AUTOINCREMENT, "
        "dni_cliente INTEGER NOT NULL, "
        "patente_vehiculo TEXT NOT NULL, "
        "tiempo_establecido REAL NOT NULL, "
        "tiempo_inicio INTEGER, "
        "costo REAL DEFAULT 0, "
        "cargo_extra REAL DEFAULT 0, "
        "activo INTEGER DEFAULT 1, "
        "FOREIGN KEY(dni_cliente) REFERENCES Cliente(dni), "
        "FOREIGN KEY(patente_vehiculo) REFERENCES Vehiculo(patente)"
    ");";

    char* errorMsg = nullptr;

    if(sqlite3_exec(datab, sql_cliente.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        cout << "Error creando tabla Cliente: " << errorMsg << endl;
        sqlite3_free(errorMsg);
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

    string sql_alter = "ALTER TABLE Contrato ADD COLUMN tiempo_inicio INTEGER;";
    if(sqlite3_exec(datab, sql_alter.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        // Si da error, probablemente la columna ya existe - no es problema
        sqlite3_free(errorMsg);
    }

    cout << "---------------------------------------------" << endl;
    cout << "Todas las tablas fueron verificadas o creadas." << endl;
    cout << "---------------------------------------------" << endl;
}

//=============================================================================
// LIMPIAR DATOS DE LA BASE DE DATOS
//=============================================================================
bool DataBase::limpiarDatos() {
    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return false;
    }

    cout << "=====================================================" << endl;
    cout << "LIMPIANDO DATOS DE LA BASE DE DATOS" << endl;
    cout << "=====================================================" << endl;

    char* errorMsg = nullptr;

    // Forzar checkpoint del WAL antes de comenzar
    sqlite3_wal_checkpoint_v2(datab, nullptr, SQLITE_CHECKPOINT_TRUNCATE, nullptr, nullptr);

    // Desactivar foreign keys primero (FUERA de la transacción)
    if (sqlite3_exec(datab, "PRAGMA foreign_keys = OFF;", nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        cout << "Advertencia desactivando foreign keys: " << errorMsg << endl;
        sqlite3_free(errorMsg);
    }

    // Iniciar transacción INMEDIATA (lock exclusivo)
    if (sqlite3_exec(datab, "BEGIN IMMEDIATE TRANSACTION;", nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        cout << "Error iniciando transacción: " << errorMsg << endl;
        sqlite3_free(errorMsg);
        sqlite3_exec(datab, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
        return false;
    }

    // Limpiar Contrato
    if (sqlite3_exec(datab, "DELETE FROM Contrato;", nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        cout << "Error limpiando Contrato: " << errorMsg << endl;
        sqlite3_free(errorMsg);
        sqlite3_exec(datab, "ROLLBACK;", nullptr, nullptr, nullptr);
        sqlite3_exec(datab, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
        return false;
    }
    cout << "Tabla Contrato limpiada." << endl;

    // Limpiar Vehiculo
    if (sqlite3_exec(datab, "DELETE FROM Vehiculo;", nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        cout << "Error limpiando Vehiculo: " << errorMsg << endl;
        sqlite3_free(errorMsg);
        sqlite3_exec(datab, "ROLLBACK;", nullptr, nullptr, nullptr);
        sqlite3_exec(datab, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
        return false;
    }
    cout << "Tabla Vehiculo limpiada." << endl;

    // Limpiar Cliente
    if (sqlite3_exec(datab, "DELETE FROM Cliente;", nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        cout << "Error limpiando Cliente: " << errorMsg << endl;
        sqlite3_free(errorMsg);
        sqlite3_exec(datab, "ROLLBACK;", nullptr, nullptr, nullptr);
        sqlite3_exec(datab, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
        return false;
    }
    cout << "Tabla Cliente limpiada." << endl;

    // Resetear autoincrement
    if (sqlite3_exec(datab, "DELETE FROM sqlite_sequence;", nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        // No es crítico, continuamos
        sqlite3_free(errorMsg);
    }

    // COMMIT
    if (sqlite3_exec(datab, "COMMIT;", nullptr, nullptr, &errorMsg) != SQLITE_OK) {
        cout << "Error haciendo commit: " << errorMsg << endl;
        sqlite3_free(errorMsg);
        sqlite3_exec(datab, "ROLLBACK;", nullptr, nullptr, nullptr);
        sqlite3_exec(datab, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
        return false;
    }

    // Reactivar foreign keys
    sqlite3_exec(datab, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    // Hacer vacuum para limpiar espacio
    sqlite3_exec(datab, "VACUUM;", nullptr, nullptr, nullptr);

    cout << "=====================================================" << endl;
    cout << "Todos los datos han sido eliminados exitosamente." << endl;
    cout << "Las tablas siguen existiendo y listas para usar." << endl;
    cout << "=====================================================" << endl;

    return true;
}

//=============================================================================
// MÉTODOS PARA CLIENTE
//=============================================================================

bool DataBase::guardarCliente(Cliente cliente) {
    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return false;
    }

    Cliente *existe = buscarClientePorDNI(cliente.getDni());
    if(existe != nullptr) {
        delete existe;
        cout << "El cliente de DNI: " << cliente.getDni() << " ya se encuentra en la BD" << endl;
        return false;
    }

    string sql = "INSERT INTO Cliente (dni, nombre, apellido, edad) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando SQL: " << sqlite3_errmsg(datab) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, cliente.getDni());
    sqlite3_bind_text(stmt, 2, cliente.getNombre().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, cliente.getApellido().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, cliente.getEdad());

    bool resultado = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (resultado) {
        cout << "Cliente guardado: " << cliente.getNombre() << " " << cliente.getApellido() << endl;
    }

    return resultado;
}

Cliente* DataBase::buscarClientePorDNI(int dni) {
    if(datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return nullptr;
    }

    string sql = "SELECT dni, nombre, apellido, edad FROM Cliente WHERE dni = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando búsqueda: " << sqlite3_errmsg(datab) << endl;
        return nullptr;
    }

    sqlite3_bind_int(stmt, 1, dni);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int dni_encontrado = sqlite3_column_int(stmt, 0);
        string nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string apellido = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int edad = sqlite3_column_int(stmt, 3);

        Cliente* cliente = new Cliente(nombre, apellido, edad, dni_encontrado);
        sqlite3_finalize(stmt);
        return cliente;
    }

    sqlite3_finalize(stmt);
    return nullptr;
}

vector<Cliente*> DataBase::cargarClientes() {
    vector<Cliente*> clientes;

    if(datab == nullptr) {
        mvprintw(0, 0, "Error: Base de datos no inicializada.");
        return clientes;
    }

    string sql = "SELECT dni, nombre, apellido, edad FROM Cliente;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        mvprintw(0, 0,"Error preparando carga de clientes: ");
        return clientes;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int dni = sqlite3_column_int(stmt, 0);
        string nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string apellido = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int edad = sqlite3_column_int(stmt, 3);

        clientes.push_back(new Cliente(nombre, apellido, edad, dni));
    }

    sqlite3_finalize(stmt);
    return clientes;
}

//=============================================================================
// MÉTODOS PARA VEHÍCULOS
//=============================================================================

bool DataBase::existeVehiculo(string patente) {
    if (datab == nullptr) {
        return false;
    }

    string sql = "SELECT patente FROM Vehiculo WHERE patente = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, patente.c_str(), -1, SQLITE_TRANSIENT);
    bool existe = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);

    return existe;
}

bool DataBase::guardarVehiculo(Vehiculo* vehiculo) {
    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return false;
    }

    if (existeVehiculo(vehiculo->getPatente())) {
        cout << "El vehículo con patente " << vehiculo->getPatente() << " ya existe en la BD" << endl;
        return false;
    }

    Moto* moto = dynamic_cast<Moto*>(vehiculo);
    if (moto != nullptr) {
        return guardarMoto(moto);
    }

    Auto* automovil = dynamic_cast<Auto*>(vehiculo);
    if (automovil != nullptr) {
        return guardarAuto(automovil);
    }

    cout << "Error: Tipo de vehículo no reconocido." << endl;
    return false;
}

bool DataBase::guardarMoto(Moto* moto) {
    string sql = "INSERT INTO Vehiculo (patente, marca, anio, precioBase, disponible, cilindradas, puertas) "
                 "VALUES (?, ?, ?, ?, ?, ?, 0);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando inserción de moto: " << sqlite3_errmsg(datab) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, moto->getPatente().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, moto->getMarca().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, moto->getAnio());
    sqlite3_bind_double(stmt, 4, moto->getPrecioBase());
    sqlite3_bind_int(stmt, 5, moto->getActivo() ? 1 : 0);
    sqlite3_bind_int(stmt, 6, moto->getCilindradas());

    bool resultado = (sqlite3_step(stmt) == SQLITE_DONE);

    if (!resultado) {
        cout << "Error insertando moto: " << sqlite3_errmsg(datab) << endl;
    }

    sqlite3_finalize(stmt);

    if (resultado) {
        cout << "Moto guardada: " << moto->getPatente() << endl;
    }

    return resultado;
}

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

    if (!resultado) {
        cout << "Error insertando auto: " << sqlite3_errmsg(datab) << endl;
    }

    sqlite3_finalize(stmt);

    if (resultado) {
        cout << "Auto guardado: " << automovil->getPatente() << endl;
    }

    return resultado;
}

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

Vehiculo* DataBase::Buscarvehiculoporpatente(string pat) {
    Vehiculo* v=nullptr;
    if(datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return v;
    }

    string sql = "SELECT patente, marca, anio, precioBase, disponible, cilindradas, puertas FROM Vehiculo WHERE patente = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error Buscando vehiculo: " << sqlite3_errmsg(datab) << endl;
        return v;
    }

    sqlite3_bind_text(stmt, 1, pat.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        string patente = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string marca = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int anio = sqlite3_column_int(stmt, 2);
        float precioBase = sqlite3_column_double(stmt, 3);
        bool disponible = sqlite3_column_int(stmt, 4) == 1;
        int cilindradas = sqlite3_column_int(stmt, 5);
        int puertas = sqlite3_column_int(stmt, 6);

        if (cilindradas > 0) {
            v = new Moto(marca, patente, anio, precioBase, cilindradas);
        } else if (puertas > 0) {
            v = new Auto(marca, patente, anio, precioBase, puertas);
        }

        if (v != nullptr) {
            v->setDisponible(disponible);
        }
    }

    sqlite3_finalize(stmt);
    return v;
}

vector<Vehiculo*> DataBase::cargarVehiculos() {
    vector<Vehiculo*> vehiculos;

    if(datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return vehiculos;
    }

    string sql = "SELECT patente, marca, anio, precioBase, disponible, cilindradas, puertas FROM Vehiculo;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando carga de vehículos: " << sqlite3_errmsg(datab) << endl;
        return vehiculos;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string patente = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string marca = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int anio = sqlite3_column_int(stmt, 2);
        float precioBase = sqlite3_column_double(stmt, 3);
        bool disponible = sqlite3_column_int(stmt, 4) == 1;
        int cilindradas = sqlite3_column_int(stmt, 5);
        int puertas = sqlite3_column_int(stmt, 6);

        Vehiculo* v = nullptr;

        if (cilindradas > 0) {
            v = new Moto(marca, patente, anio, precioBase, cilindradas);
        } else if (puertas > 0) {
            v = new Auto(marca, patente, anio, precioBase, puertas);
        }

        if (v != nullptr) {
            v->setDisponible(disponible);
            vehiculos.push_back(v);
        }
    }

    sqlite3_finalize(stmt);
    return vehiculos;
}

//=============================================================================
// MÉTODOS PARA CONTRATOS
//=============================================================================

bool DataBase::guardarContrato(Contrato contrato) {
    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return false;
    }

    string sql = "INSERT INTO Contrato (dni_cliente, patente_vehiculo, tiempo_establecido, tiempo_inicio, activo) "
                 "VALUES (?, ?, ?, ?, 1);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando inserción de contrato: " << sqlite3_errmsg(datab) << endl;
        return false;
    }

    Cliente cliente = contrato.getCliente();
    Vehiculo* vehiculo = contrato.getVehiculo();
    float tiempoSegundos = contrato.getTiempoEstablecido().count();

    auto ahora = system_clock::now();
    auto timestamp = system_clock::to_time_t(ahora);

    sqlite3_bind_int(stmt, 1, cliente.getDni());
    sqlite3_bind_text(stmt, 2, vehiculo->getPatente().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, tiempoSegundos);
    sqlite3_bind_int64(stmt, 4, static_cast<sqlite3_int64>(timestamp));

    bool resultado = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (resultado) {
        cout << "Contrato guardado exitosamente en BD con tiempo de inicio." << endl;
    }

    return resultado;
}

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

void DataBase::cargarHistorial(Historial *historial) {

    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return;
    }

    string sql = "SELECT c.id_contrato, c.dni_cliente, c.patente_vehiculo, "
                 "c.tiempo_establecido, c.tiempo_inicio, c.costo, c.cargo_extra, "
                 "cl.nombre, cl.apellido, cl.edad "
                 "FROM Contrato c "
                 "JOIN Cliente cl ON c.dni_cliente = cl.dni "
                 "WHERE c.activo = 0 "
                 "ORDER BY c.id_contrato;";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando carga de historial: " << sqlite3_errmsg(datab) << endl;
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id_contrato = sqlite3_column_int(stmt, 0);
        int dni = sqlite3_column_int(stmt, 1);
        string patente = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        float tiempo = sqlite3_column_double(stmt, 3);
        sqlite3_int64 tiempo_inicio_timestamp = sqlite3_column_int64(stmt, 4);
        float costo = sqlite3_column_double(stmt, 5);
        float cargo_extra = sqlite3_column_double(stmt, 6);
        string nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        string apellido = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        int edad = sqlite3_column_int(stmt, 9);

        // Buscar el vehículo
        Vehiculo* vehiculo = Buscarvehiculoporpatente(patente);

        if (vehiculo == nullptr) {
            cout << "Advertencia: No se encontró vehículo con patente " << patente << endl;
            return;
        }

        // Crear cliente
        Cliente cliente(nombre, apellido, edad, dni);

        // Crear contrato
        Contrato* contrato = new Contrato(id_contrato, cliente, vehiculo, tiempo, cargo_extra);
        contrato->setcosto(costo);
        // IMPORTANTE: Restaurar el tiempo de inicio desde la BD
        if (tiempo_inicio_timestamp > 0) {
            time_t inicio_time = static_cast<time_t>(tiempo_inicio_timestamp);
            auto inicio_timepoint = system_clock::from_time_t(inicio_time);

            // Restaurar el inicio del contrato
            contrato->setInicio(inicio_timepoint);
        }

        if (contrato != nullptr) {
            historial->agregarContrato(contrato);
        }
    }

    sqlite3_finalize(stmt);
}

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

    sqlite3_bind_int(stmt, 1, dni);

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

vector<Contrato*> DataBase::cargarContratosActivos() {
    vector<Contrato*> activos;

    if (datab == nullptr) {
        cout << "Error: Base de datos no inicializada." << endl;
        return activos;
    }

    string sql = "SELECT c.id_contrato, c.dni_cliente, c.patente_vehiculo, "
                 "c.tiempo_establecido, c.tiempo_inicio, c.costo, c.cargo_extra, "
                 "cl.nombre, cl.apellido, cl.edad "
                 "FROM Contrato c "
                 "JOIN Cliente cl ON c.dni_cliente = cl.dni "
                 "WHERE c.activo = 1 "
                 "ORDER BY c.id_contrato;";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(datab, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparando carga de contratos activos: " << sqlite3_errmsg(datab) << endl;
        return activos;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id_contrato = sqlite3_column_int(stmt, 0);
        int dni = sqlite3_column_int(stmt, 1);
        string patente = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        float tiempo = sqlite3_column_double(stmt, 3);
        sqlite3_int64 tiempo_inicio_timestamp = sqlite3_column_int64(stmt, 4);
        float costo = sqlite3_column_double(stmt, 5);
        float cargo_extra = sqlite3_column_double(stmt, 6);
        string nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        string apellido = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        int edad = sqlite3_column_int(stmt, 9);

        // Buscar el vehículo
        Vehiculo* vehiculo = Buscarvehiculoporpatente(patente);

        if (vehiculo == nullptr) {
            cout << "Advertencia: No se encontró vehículo con patente " << patente << endl;
            continue;
        }

        // Crear cliente
        Cliente cliente(nombre, apellido, edad, dni);

        // Crear contrato
        Contrato* contrato = new Contrato(id_contrato, cliente, vehiculo, tiempo, cargo_extra);

        // IMPORTANTE: Restaurar el tiempo de inicio desde la BD
        if (tiempo_inicio_timestamp > 0) {
            time_t inicio_time = static_cast<time_t>(tiempo_inicio_timestamp);
            auto inicio_timepoint = system_clock::from_time_t(inicio_time);

            // Restaurar el inicio del contrato
            contrato->setInicio(inicio_timepoint);
        }

        if (contrato != nullptr) {
            activos.push_back(contrato);
        }
    }

    sqlite3_finalize(stmt);
    return activos;
}

//=============================================================================
// DESTRUCTOR
//=============================================================================
DataBase::~DataBase() {
    if (datab != nullptr) {
        sqlite3_close(datab);
        cout << "BD cerrada." << endl;
    }
}