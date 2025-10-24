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
    sqlite3* datab;
    string nombreBD;

    // Función auxiliar privada para verificar existencia de vehículo
    bool existeVehiculo(string patente);

public:
    // Constructor y destructor
    DataBase(string nBD);
    ~DataBase();

    // Crear tablas
    void crearTablas();

    //=============================================================================
    // MÉTODOS PARA CLIENTE
    //=============================================================================
    bool guardarCliente(Cliente cliente);
    Cliente* buscarClientePorDNI(int dni);
    vector<Cliente*> cargarClientes();

    //=============================================================================
    // MÉTODOS PARA VEHÍCULO
    //=============================================================================
    bool guardarVehiculo(Vehiculo* vehiculo);
    bool guardarMoto(Moto* moto);
    bool guardarAuto(Auto* automovil);
    bool actualizarDisponibilidadVehiculo(string patente, bool disponible);
    vector<Vehiculo*> cargarVehiculos();  // NUEVA FUNCIÓN
    Vehiculo* Buscarvehiculoporpatente(string patente);

    //=============================================================================
    // MÉTODOS PARA CONTRATOS
    //=============================================================================
    bool guardarContrato(Contrato contrato);
    bool finalizarContrato(int id_contrato, float costo_final);
    vector<Contrato*> cargarHistorial();
    vector<Contrato*> cargarHistorialPorCliente(int dni);
    vector<Contrato*> cargarContratosActivos();
};

#endif //PP3_PROYECTO_DATABASE_H