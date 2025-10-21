// DECLARACIONES

#ifndef PP3_PROYECTO_ARCHIVO_H
#define PP3_PROYECTO_ARCHIVO_H

#include <vector>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono; //para no escribir el prefijo en chrono.

class Vehiculo{
protected:
    string marca;
    string patente;
    int anio;
    float precioBase;
    bool disponible;
    static int vivas;

public:
    Vehiculo(string m, string p, int a, float pB);

    virtual ~Vehiculo();

    string getMarca()const;
    string getPatente()const;
    int getAnio()const;
    float getPrecioBase()const;
    bool getActivo()const;

    void setPrecioBase(float p);
    void setDisponible(bool a);
    virtual void mostrarInfo();
};


class Moto: public Vehiculo{
private:
    int cilindradas;
public:
    Moto(string mar, string pat, int a, float pB, int cil);

    void mostrarInfo() override;
};

class Auto: public Vehiculo{
private:
    int puertas;
public:
    Auto(string mar, string pat, int a, float pB, int puer);

    void mostrarInfo() override;
};

class Cliente{
private:
    string nombre;
    string apellido;
    int edad;
    int dni;
public:
    Cliente(string n, string a, int e, int d);

    string getNombre() const;
    string getApellido() const;
    int getEdad() const;
    int getDni() const;

    void mostrarInfo() const;

    ~Cliente(){};
};

class Contrato{
private:
    int id_contrato;
    Cliente cliente;
    Vehiculo* vehiculo;
    duration<float> tiempoEstablecido;  // duración en segundos
    time_point<system_clock> inicio;
    time_point<system_clock> fin;
    float costo;
    float cargoExtraporHora;
public:
    Contrato(int id, Cliente c, Vehiculo* v, float tiempoHoras, float cargo);

    Cliente getCliente() const;
    void iniciarContrato();
    void cerrarContrato();
    void mostrarInfo() const;

    ~Contrato(){};
};


class Historial{
private:
    vector<Contrato*> lista_contratos;
public:
    void agregarContrato(Contrato * contratoAgregar);
    void mostrarContratoPorCliente(Cliente cliente);
    void mostrarHistorial();
};

class SistemaAlquiler{
private:
    vector<Cliente*> clientes;
    vector<Vehiculo*> vehiculos;
    Historial historial;
    vector<Contrato*> contratos_activos;
public:
    bool registrarCliente(string nombre, string apellido, int edad, int dni);
    bool registrarVehiculo(Vehiculo* v);
    Contrato* crearNuevoContrato(int dni, string patente, float horas);
    bool cerrarContrato(int id_contrato);
    void listarVehiculosDisponibles();
    void listarContratos();
    void listarClientesRegistrados();

};

#endif //PP3_PROYECTO_ARCHIVO_H
