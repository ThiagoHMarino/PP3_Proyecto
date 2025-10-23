// IMPLEMENTACIONES

#include "Archivo.h"
#include <iostream>
#include <vector>
#include "database.h"

//CLIENTE
Cliente :: Cliente(string n, string a, int e, int d):nombre(n), apellido(a), edad(e), dni(d){}
string Cliente::getNombre() const {return nombre;}
string Cliente::getApellido() const {return apellido;}
int Cliente::getEdad() const{return edad;}
int Cliente::getDni() const {return dni;}
void Cliente::mostrarInfo() const {
        cout << "=====================================" << endl;
        cout<<"CLIENTE:"<<endl;
    cout<<"Nombre: "<<nombre<< " Apellido: "<< apellido <<" Edad: "<<edad<<" Dni: "<<dni<<endl;
    cout << "=====================================" << endl;

}

//VEHÍCULO------------------------------------------------------------------------------------------------------------------------------------
Vehiculo::Vehiculo(string m, string p, int a, float pB):marca(m), patente(p), anio(a), precioBase(pB), disponible{true}{vivas++;}

Vehiculo::~Vehiculo() {
    vivas--;
}

string Vehiculo::getMarca() const {return marca;}
string Vehiculo::getPatente() const {return patente;}
int Vehiculo::getAnio() const {return anio;}
bool Vehiculo::getActivo() const {return disponible;}
float Vehiculo::getPrecioBase() const {return precioBase;}
void Vehiculo::setDisponible(bool a) {disponible=a;}
void Vehiculo::setPrecioBase(float p) {precioBase=p;}
void Vehiculo::mostrarInfo() {
    cout << "=====================================" << endl;
    cout<<"Marca: "<<marca<<" Patente: "<<patente<<" Año:"<< anio
    <<" Precio Base: "<< precioBase <<" Disponibilidad: "<<disponible<<endl;
    cout << "=====================================" << endl;
}
int Vehiculo::vivas=0;
//------------------------------------------------------------------------------------------------------------------------------------

//AUTO
Auto::Auto(string m, string pat, int a, float pB, int puer):Vehiculo(m,pat,a,pB),puertas(puer){}

void Auto::mostrarInfo(){
    cout << "=====================================" << endl;
    cout << "AUTO" << endl;
    cout<<"Marca: "<<marca<<" Patente: "<<patente<<" Año:"<< anio
        <<" Precio Base: "<< precioBase <<" Disponibilidad: "<<disponible<< " PUERTAS: "<< puertas <<endl;
    cout << "=====================================" << endl;
}

int Auto::getPuertas() const{return puertas;}

//MOTO
Moto::Moto(string m, string pat, int a, float pB, int cil):Vehiculo(m,pat,a,pB),cilindradas(cil){}

void Moto::mostrarInfo(){
    cout << "=====================================" << endl;
    cout << "MOTO" << endl;
    cout<<"Marca: "<<marca<<" Patente: "<<patente<<" Año:"<< anio
        <<" Precio Base: "<< precioBase <<" Disponibilidad: "<<disponible<< " CILINDRADAS: "<< cilindradas <<endl;
    cout << "=====================================" << endl;
}

int Moto::getCilindradas() const{return cilindradas;}
//CONTRATO
Contrato::Contrato(int id, Cliente c, Vehiculo* v, float tiempoHoras, float cargo):
id_contrato(id),cliente(c),vehiculo(v),tiempoEstablecido(tiempoHoras), costo(0), cargoExtraporHora(cargo){}

Cliente Contrato::getCliente() const{return cliente;}

//AGREGUE ESTO
void Contrato::iniciarContrato() {
    inicio = system_clock::now();
    vehiculo->setDisponible(false);
    cout << "=====================================" << endl;
    cout << "Contrato #" << id_contrato << " iniciado." << endl;
    cout << "=====================================" << endl;
}

void Contrato::cerrarContrato(){
    fin = system_clock::now();
    duration<float> tiempoReal=fin - inicio;
    duration<float> exceso=tiempoReal-tiempoEstablecido;

    if(exceso.count() > 0){
        float HorasExtra=exceso.count() / 3600.f;
        costo+=HorasExtra*cargoExtraporHora;
        cout << "=====================================" << endl;
        cout << "Tiempo excedido: " << HorasExtra << " horas" << endl;
        cout << "Cargo adicional: $" << costo << endl;
    }
    float HorasEstablecidas=tiempoEstablecido.count() / 3600.f;
    costo+=vehiculo->getPrecioBase()*HorasEstablecidas;
    cout << "Contrato cerrado. Costo total: $" << costo << endl;
    cout << "=====================================" << endl;
}
void Contrato::mostrarInfo() const {

    cout << "=====================================" << endl;
    cout << "CONTRATO: " <<id_contrato<< endl;
    cout<< "Vehiculo alquilado"<<endl;
    vehiculo->mostrarInfo();
    cout<<endl;

    cout<<"CLIENTE: "<<endl;
    cliente.mostrarInfo();
    cout<<endl;

    cout<<"DETALLES:"<<endl;

    cout<<"Tiempo establecido: "<< (tiempoEstablecido.count() / 3600.0f) << " horas" << endl;
    cout<<endl;

    duration<float> tiempoReal = fin - inicio;
    cout << "Duración real: " << (tiempoReal.count() / 3600.0f) << " horas" << endl;

    cout << "Costo total: $" << costo << endl;
    cout << "=====================================" << endl;
}
//METODOS ADICIONALES CONTRATO

int Contrato::getId() const { return id_contrato; }
Vehiculo* Contrato::getVehiculo() const { return vehiculo; }
float Contrato::getCosto() const { return costo; }
duration<float> Contrato::getTiempoEstablecido() const { return tiempoEstablecido; }


//HISTORIAL

void Historial::agregarContrato(Contrato *contratoAgregar) {
    lista_contratos.push_back(contratoAgregar);
}

void Historial::mostrarContratoPorCliente(Cliente cliente) {
    cout << "=====================================" << endl;
    cout << "CONTRATOS DEL CLIENTE: " << cliente.getNombre() << ", " << cliente.getApellido() <<
    ", DNI: " << cliente.getDni() << endl;

    bool encontrado=false;

    vector<Contrato*>::iterator it;
    for(it=lista_contratos.begin();it!=lista_contratos.end();it++){  //Mepa que en el for va un (auto it = lista_contratos;...) 
        if((*it)->getCliente().getDni()==cliente.getDni()){
            (*it)->mostrarInfo();
            encontrado=true;
        }
    }

    if(!encontrado){
        cout << "No se encontro al cliente solicitado." << endl;
    }

    cout << "=====================================" << endl;
}

void Historial::mostrarHistorial() {
    cout << "=====================================" << endl;
    cout << "HISTORIAL DE CONTRATOS" << endl;
    cout << "Total de contratos: " << lista_contratos.size() << endl;
    cout << "=====================================" << endl;

    if (lista_contratos.empty()) {
        cout << "No hay contratos registrados en el historial." << endl;
        return;
    }

    vector<Contrato*>::iterator it;
    int contador=0;
    for (it = lista_contratos.begin(); it != lista_contratos.end(); it++) {
        cout << "CONTRATO #" << contador << " ---" << endl;
        (*it)->mostrarInfo();
        contador++;
    }

    cout << "=====================================" << endl;
}

//=============================================================================
// SISTEMA ALQUILER - IMPLEMENTACIÓN COMPLETA
//=============================================================================

SistemaAlquiler::SistemaAlquiler(DataBase* db) : database(db), proximo_id_contrato(1) {
    cargarDatos();
}

SistemaAlquiler::~SistemaAlquiler() {
    for (Cliente* c : clientes) {
        delete c;
    }
    for (Vehiculo* v : vehiculos) {
        delete v;
    }
    for (Contrato* c : contratos_activos) {
        delete c;
    }
}

void SistemaAlquiler::cargarDatos() {
    cout << "Cargando datos desde la base de datos..." << endl;
    clientes = database->cargarClientes();
    cout << "Clientes cargados: " << clientes.size() << endl;
    vehiculos = database->cargarVehiculos();
    cout << "Vehiculos cargados: " << vehiculos.size() << endl;
}

bool SistemaAlquiler::registrarCliente(string nombre, string apellido, int edad, int dni) {
    Cliente nuevoCliente(nombre, apellido, edad, dni);

    if (database->guardarCliente(nuevoCliente)) {
        clientes.push_back(new Cliente(nombre, apellido, edad, dni));
        return true;
    }
    return false;
}

bool SistemaAlquiler::registrarVehiculo(Vehiculo* v) {
    if (v == nullptr) {
        cout << "Error: Vehiculo nulo." << endl;
        return false;
    }

    if (database->guardarVehiculo(v)) {
        vehiculos.push_back(v);
        return true;
    }

    delete v;
    return false;
}

Contrato* SistemaAlquiler::crearNuevoContrato(int dni, string patente, float horas) {
    Cliente* cliente = buscarCliente(dni);
    if (cliente == nullptr) {
        cout << "Error: Cliente con DNI " << dni << " no encontrado." << endl;
        return nullptr;
    }

    Vehiculo* vehiculo = buscarVehiculo(patente);
    if (vehiculo == nullptr) {
        cout << "Error: Vehiculo con patente " << patente << " no encontrado." << endl;
        return nullptr;
    }

    if (!vehiculo->getActivo()) {
        cout << "Error: Vehiculo no disponible." << endl;
        return nullptr;
    }

    float horasEnSegundos = horas * 3600.0f;
    Contrato* nuevoContrato = new Contrato(
        proximo_id_contrato++,
        *cliente,
        vehiculo,
        horasEnSegundos,
        100.0f
    );

    nuevoContrato->iniciarContrato();
    database->actualizarDisponibilidadVehiculo(patente, false);
    database->guardarContrato(*nuevoContrato);

    contratos_activos.push_back(nuevoContrato);
    vehiculo->setDisponible(false);

    return nuevoContrato;
}

bool SistemaAlquiler::cerrarContrato(int id_contrato) {
    Contrato* contrato = buscarContratoActivo(id_contrato);

    if (contrato == nullptr) {
        cout << "Error: Contrato #" << id_contrato << " no encontrado." << endl;
        return false;
    }

    contrato->cerrarContrato();

    string patente = contrato->getVehiculo()->getPatente();
    float costo = contrato->getCosto();

    database->finalizarContrato(id_contrato, costo);
    database->actualizarDisponibilidadVehiculo(patente, true);

    Vehiculo* vehiculo = buscarVehiculo(patente);
    if (vehiculo != nullptr) {
        vehiculo->setDisponible(true);
    }

    historial.agregarContrato(contrato);

    for (size_t i = 0; i < contratos_activos.size(); i++) {
        if (contratos_activos[i]->getId() == id_contrato) {
            contratos_activos.erase(contratos_activos.begin() + i);
            break;
        }
    }

    cout << "Contrato #" << id_contrato << " cerrado exitosamente." << endl;
    return true;
}

void SistemaAlquiler::listarVehiculosDisponibles() {
    cout << "=====================================" << endl;
    cout << "VEHICULOS DISPONIBLES" << endl;
    cout << "=====================================" << endl;

    int contador = 0;
    for (Vehiculo* v : vehiculos) {
        if (v->getActivo()) {
            v->mostrarInfo();
            contador++;
        }
    }

    if (contador == 0) {
        cout << "No hay vehiculos disponibles." << endl;
    } else {
        cout << "Total disponibles: " << contador << endl;
    }
    cout << "=====================================" << endl;
}

void SistemaAlquiler::listarTodosVehiculos() {
    cout << "=====================================" << endl;
    cout << "TODOS LOS VEHICULOS" << endl;
    cout << "=====================================" << endl;

    for (Vehiculo* v : vehiculos) {
        v->mostrarInfo();
    }

    cout << "Total vehiculos: " << vehiculos.size() << endl;
    cout << "=====================================" << endl;
}

void SistemaAlquiler::listarContratos() {
    cout << "=====================================" << endl;
    cout << "CONTRATOS ACTIVOS" << endl;
    cout << "=====================================" << endl;

    if (contratos_activos.empty()) {
        cout << "No hay contratos activos." << endl;
    } else {
        for (Contrato* c : contratos_activos) {
            c->mostrarInfo();
        }
        cout << "Total contratos activos: " << contratos_activos.size() << endl;
    }
    cout << "=====================================" << endl;
}

void SistemaAlquiler::listarClientesRegistrados() {
    cout << "=====================================" << endl;
    cout << "CLIENTES REGISTRADOS" << endl;
    cout << "=====================================" << endl;

    for (Cliente* c : clientes) {
        c->mostrarInfo();
    }

    cout << "Total clientes: " << clientes.size() << endl;
    cout << "=====================================" << endl;
}

void SistemaAlquiler::mostrarHistorialCompleto() {
    historial.mostrarHistorial();
}

void SistemaAlquiler::mostrarHistorialCliente(int dni) {
    Cliente* cliente = buscarCliente(dni);
    if (cliente != nullptr) {
        historial.mostrarContratoPorCliente(*cliente);
    } else {
        cout << "Cliente no encontrado." << endl;
    }
}

Cliente* SistemaAlquiler::buscarCliente(int dni) {
    for (Cliente* c : clientes) {
        if (c->getDni() == dni) {
            return c;
        }
    }
    return nullptr;
}

Vehiculo* SistemaAlquiler::buscarVehiculo(string patente) {
    for (Vehiculo* v : vehiculos) {
        if (v->getPatente() == patente) {
            return v;
        }
    }
    return nullptr;
}

Contrato* SistemaAlquiler::buscarContratoActivo(int id) {
    for (Contrato* c : contratos_activos) {
        if (c->getId() == id) {
            return c;
        }
    }
    return nullptr;
}