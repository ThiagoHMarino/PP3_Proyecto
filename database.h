#ifndef PP3_PROYECTO_DATABASE_H
#define PP3_PROYECTO_DATABASE_H

#include "Archivo.h"
#include <sqlite3.h>
#include <string>
#include <iostream>

using namespace std;

class DataBase{
private:
    sqlite3 * datab; //puntero a una bd de sqlite
    string nombreBD;
public:
    DataBase(string nBD);
    ~DataBase();

    void crearTablas(); // Metodo que crea todas las tablas

    // Métodos para Vehiculo
    bool guardarVehiculo(Vehiculo* vehiculo);
    bool guardarMoto(Moto* moto);
    bool guardarAuto(Auto* automovil);
    vector<Vehiculo*> cargarVehiculos();
    vector<Vehiculo*> cargarVehiculosDisponibles();
    bool actualizarDisponibilidadVehiculo(string patente, bool disponible);

    // Métodos para Clientes
    bool guardarCliente(Cliente cliente);
    vector<Cliente> cargarClientes();
    Cliente* buscarClientePorDNI(int dni);

    // Métodos para Contratos
    bool guardarContrato(Contrato contrato);
    vector<Contrato> cargarContratos();
    vector<Contrato> cargarContratosActivos();
    bool finalizarContrato(int id_contrato, float costo_final);

    // Métodos auxiliares
    Vehiculo* crearVehiculoDesdeBD(string tipo, string marca, string patente,
    int anio, float precio_base, bool disponible,int cilindradas, int puertas);

};

#endif //PP3_PROYECTO_DATABASE_H
