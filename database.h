#ifndef PP3_PROYECTO_DATABASE_H
#define PP3_PROYECTO_DATABASE_H

#include "Archivo.h"
#include <sqlite3.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

class DataBase {
private:
    sqlite3* datab;      // Puntero a una base de datos de SQLite
    string nombreBD;     // Nombre del archivo de la base de datos

public:
    // Constructor: abre/crea la base de datos
    DataBase(string nBD);

    // Destructor: cierra la conexión con la base de datos
    ~DataBase();

    // Crea todas las tablas necesarias (Cliente, Vehiculo, Contrato)
    void crearTablas();

    //=============================================================================
    // MÉTODOS PARA CLIENTE
    //=============================================================================

    // Guarda un cliente en la BD (retorna false si ya existe)
    bool guardarCliente(Cliente cliente);

    // Busca un cliente por DNI (retorna nullptr si no existe)
    // IMPORTANTE: liberar memoria con delete
    Cliente* buscarClientePorDNI(int dni);

    // Carga todos los clientes de la BD en un vector
    vector<Cliente*> cargarClientes();

    //=============================================================================
    // MÉTODOS PARA VEHÍCULO
    //=============================================================================

    // Guarda cualquier tipo de vehículo (Auto o Moto)
    bool guardarVehiculo(Vehiculo* vehiculo);

    // Guarda específicamente una Moto
    bool guardarMoto(Moto* moto);

    // Guarda específicamente un Auto
    bool guardarAuto(Auto* automovil);

    // Actualiza el estado de disponibilidad de un vehículo
    bool actualizarDisponibilidadVehiculo(string patente, bool disponible);

    //=============================================================================
    // MÉTODOS PARA CONTRATOS
    //=============================================================================

    // Guarda un nuevo contrato en la BD
    bool guardarContrato(Contrato contrato);

    // Finaliza un contrato actualizando su costo y marcándolo como cerrado
    bool finalizarContrato(int id_contrato, float costo_final);

    // Carga todos los contratos (historial completo)
    vector<Contrato*> cargarHistorial();

    // Carga solo los contratos de un cliente específico
    vector<Contrato*> cargarHistorialPorCliente(int dni);

    // Carga solo los contratos activos (no finalizados)
    vector<Contrato*> cargarContratosActivos();
};

#endif //PP3_PROYECTO_DATABASE_H
