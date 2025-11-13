// IMPLEMENTACIONES

#include "Archivo.h"
#include <iostream>
#include <vector>
#include "database.h"
#include <ncurses.h>
#include <algorithm>
#include <cctype>

//CLIENTE
Cliente :: Cliente(string n, string a, int e, int d):nombre(n), apellido(a), edad(e), dni(d){}
string Cliente::getNombre() const {return nombre;}
string Cliente::getApellido() const {return apellido;}
int Cliente::getEdad() const{return edad;}
int Cliente::getDni() const {return dni;}
/*void Cliente::mostrarInfo() const {
        cout << "=====================================" << endl;
        cout<<"CLIENTE:"<<endl;
    cout<<"Nombre: "<<nombre<< " Apellido: "<< apellido <<" Edad: "<<edad<<" Dni: "<<dni<<endl;
    cout << "=====================================" << endl;}
*/
void Cliente::mostrarInfo(int y) const {
    mvprintw(y, 0,  "=====================================");
    //mvprintw(y + 1, 0, "CLIENTE:");
    mvprintw(y + 1, 0, "Nombre: %s  Apellido: %s  Edad: %d  DNI: %d",
             nombre.c_str(),
             apellido.c_str(),
             edad,
             dni);
    mvprintw(y + 2, 0, "=====================================");
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
/*void Vehiculo::mostrarInfo() {
    cout << "=====================================" << endl;
    cout<<"Marca: "<<marca<<" Patente: "<<patente<<" Año:"<< anio
    <<" Precio Base: "<< precioBase <<" Disponibilidad: "<<disponible<<endl;
    cout << "=====================================" << endl;
}*/
void Vehiculo::mostrarInfo(int y) {
    mvprintw(y, 0,  "=====================================");
    mvprintw(y + 1, 0, "Marca: %s  Patente: %s  Año: %d  Precio Base: %.2f  Disponibilidad: %s",
             marca.c_str(),
             patente.c_str(),
             anio,
             precioBase,
             disponible ? "Sí" : "No");
    mvprintw(y + 2, 0, "=====================================");
}

int Vehiculo::vivas=0;
//------------------------------------------------------------------------------------------------------------------------------------

//AUTO
Auto::Auto(string m, string pat, int a, float pB, int puer):Vehiculo(m,pat,a,pB),puertas(puer){}

/*void Auto::mostrarInfo(){
    cout << "=====================================" << endl;
    cout << "AUTO" << endl;
    cout<<"Marca: "<<marca<<" Patente: "<<patente<<" Año:"<< anio
        <<" Precio Base: "<< precioBase <<" Disponibilidad: "<<disponible<< " PUERTAS: "<< puertas <<endl;
    cout << "=====================================" << endl;
}*/
void Auto::mostrarInfo(int y) {
    mvprintw(y, 0,  "=====================================");
    mvprintw(y + 1, 0, "AUTO");
    mvprintw(y + 2, 0,
             "Marca: %s  Patente: %s  Año: %d  Precio Base: %.2f  Disponibilidad: %s  PUERTAS: %d",
             marca.c_str(),
             patente.c_str(),
             anio,
             precioBase,
             disponible ? "Sí" : "No",
             puertas);
    mvprintw(y + 3, 0, "=====================================");
}

int Auto::getPuertas() const{return puertas;}


//MOTO
Moto::Moto(string m, string pat, int a, float pB, int cil):Vehiculo(m,pat,a,pB),cilindradas(cil){}

/*void Moto::mostrarInfo(){
    cout << "=====================================" << endl;
    cout << "MOTO" << endl;
    cout<<"Marca: "<<marca<<" Patente: "<<patente<<" Año:"<< anio
        <<" Precio Base: "<< precioBase <<" Disponibilidad: "<<disponible<< " CILINDRADAS: "<< cilindradas <<endl;
    cout << "=====================================" << endl;
}*/
void Moto::mostrarInfo(int y) {
    mvprintw(y, 0,  "=====================================");
    mvprintw(y + 1, 0, "MOTO");
    mvprintw(y + 2, 0,
             "Marca: %s  Patente: %s  Año: %d  Precio Base: %.2f  Disponibilidad: %s  CILINDRADAS: %d",
             marca.c_str(),
             patente.c_str(),
             anio,
             precioBase,
             disponible ? "Sí" : "No",
             cilindradas);
    mvprintw(y + 3, 0, "=====================================");
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
    mvprintw(0, 0,  "=====================================");
    mvprintw(1, 0, "Contrato #: %d iniciado", id_contrato);
    mvprintw(2, 0, "=====================================");
}

void Contrato::cerrarContrato(){
    fin = system_clock::now();
    duration<float> tiempoReal=fin - inicio;
    duration<float> exceso=tiempoReal-tiempoEstablecido;
    int y=0;

    if(exceso.count() > 0){
        float HorasExtra=exceso.count() / 3600.f;
        costo+=HorasExtra*cargoExtraporHora;
        mvprintw(y++, 0, "=====================================");
        mvprintw(y++, 0, "Tiempo excedido: %.2f horas", HorasExtra);
        mvprintw(y++, 0, "Cargo adicional: $%.2f", costo);
        mvprintw(y++, 0, "=====================================");
    }
    float HorasEstablecidas=tiempoEstablecido.count() / 3600.f;
    costo+=vehiculo->getPrecioBase()*HorasEstablecidas;
    mvprintw(y++, 0, "Contrato cerrado. Costo total: $%.2f", costo);
    mvprintw(y++, 0, "=====================================");
}
void Contrato::mostrarInfo(int& y) const {

    mvprintw(y++, 0, "=====================================");
    mvprintw(y++, 0, "ID: %d", id_contrato);
    mvprintw(y++, 0, "Vehiculo alquilado:");
    vehiculo->mostrarInfo(y+1); // tu mostrarInfo de Vehiculo usa y
    y += 4;

    mvprintw(y++, 0, "CLIENTE:");
    cliente.mostrarInfo(y+1); // tu mostrarInfo de Cliente usa y
    y += 4;

    mvprintw(y++, 0, "DETALLES:");
    mvprintw(y++, 0, "Tiempo establecido: %.2f horas", tiempoEstablecido.count() / 3600.0f);

    duration<float> tiempoReal = fin - inicio;
    mvprintw(y++, 0, "Duración real: %.2f horas", tiempoReal.count() / 3600.0f);
    mvprintw(y++, 0, "Costo total: $%.2f", costo);
    mvprintw(y++, 0, "=====================================");
    /*mvprintw(y + 1, 0, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();*/

}

//METODOS ADICIONALES CONTRATO

int Contrato::getId() const { return id_contrato; }
Vehiculo* Contrato::getVehiculo() const { return vehiculo; }
float Contrato::getCosto() const { return costo; }
duration<float> Contrato::getTiempoEstablecido() const { return tiempoEstablecido; }


//HISTORIAL
Historial::~Historial(){
    for (Contrato* c : lista_contratos) {
        delete c;
    }
}

void Historial::agregarContrato(Contrato *contratoAgregar) {
    lista_contratos.push_back(contratoAgregar);
}

size_t Historial::getsize() {return lista_contratos.size();}

void Historial::mostrarContratoPorCliente(Cliente cliente) {

    clear();
    int y = 0;

    mvprintw(y++, 0, "=====================================");
    mvprintw(y++, 0, "CONTRATOS DEL CLIENTE: %s, %s, DNI: %d",
             cliente.getNombre().c_str(),
             cliente.getApellido().c_str(),
             cliente.getDni());

    bool encontrado = false;

    for (Contrato* c : lista_contratos) {
        if (c->getCliente().getDni() == cliente.getDni()) {
            c->mostrarInfo(y);   // mostrarInfo de Contrato usa y
            y += 10; // deja espacio entre contratos
            encontrado = true;
        }
    }

    if (!encontrado) {
        mvprintw(y++, 0, "No se encontro al cliente solicitado.");
    }
    /*
    mvprintw(y++, 0, "=====================================");
    mvprintw(y++, 0, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();*/

}

void Historial::mostrarHistorial() {
    clear();
    int y = 0;

    mvprintw(y++, 0, "=====================================");
    mvprintw(y++, 0, "HISTORIAL DE CONTRATOS");
    mvprintw(y++, 0, "Total de contratos: %zu", lista_contratos.size());
    mvprintw(y++, 0, "=====================================");

    if (lista_contratos.empty()) {
        mvprintw(y++, 0, "No hay contratos registrados en el historial.");
        /*mvprintw(y++, 0, "Presiona cualquier tecla para continuar...");
        refresh();
        getch();*/
        return;
    }

    int contador = 0;
    for (Contrato* c : lista_contratos) {
        mvprintw(y++, 0, "CONTRATO #%d ---", contador++);
        c->mostrarInfo(y);  // mostrarInfo de Contrato usa y
        y += 10;             // deja espacio entre contratos
    }

    mvprintw(y++, 0, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();

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
    clear();
    int y = 0;

    mvprintw(y++, 0, "Cargando datos desde la base de datos...");
    refresh();
    clientes = database->cargarClientes();+
    mvprintw(y++, 0, "Clientes cargados: %lu", clientes.size());
    refresh();
    vehiculos = database->cargarVehiculos();
    mvprintw(y++, 0, "Vehiculos cargados: %lu", vehiculos.size());
    refresh();
    contratos_activos = database->cargarContratosActivos();
    mvprintw(y++, 0, "Contratos cargados: %lu", contratos_activos.size());
    refresh();
    database->cargarHistorial(&historial);
    mvprintw(y++, 0, "Contratos cargados en el historial: %lu", contratos_activos.size());
    refresh();

    mvprintw(y+1, 0, "Presiona cualquier tecla para continuar...");
    getch();

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
    clear();
    int y = 0;

    if (v == nullptr) {
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
    clear();
    int y = 0;

    Cliente* cliente = buscarCliente(dni);
    if (cliente == nullptr) {
        mvprintw(y++, 0, "Error: Cliente con DNI %d no encontrado.", dni);
        mvprintw(y+1, 0, "Presiona cualquier tecla para continuar...");
        getch();
        return nullptr;
    }

    Vehiculo* vehiculo = buscarVehiculo(patente);
    if (vehiculo == nullptr) {
        mvprintw(y++, 0, "Error: Vehiculo con patente %s no encontrado.", patente.c_str());
        mvprintw(y+1, 0, "Presiona cualquier tecla para continuar...");
        getch();
        return nullptr;
    }

    if (!vehiculo->getActivo()) {
        mvprintw(y++, 0, "Error: Vehiculo no disponible.");
        mvprintw(y+1, 0, "Presiona cualquier tecla para continuar...");
        getch();
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
    clear();
    int y = 0;

    Contrato* contrato = buscarContratoActivo(id_contrato);

    if (contrato == nullptr) {
        mvprintw(y++, 0, "Error: Contrato #%d no encontrado.", id_contrato);
        mvprintw(y++, 0, "Presiona cualquier tecla para continuar...");
        getch();
        return false;
    }

    contrato->cerrarContrato(); // Si esta función también usa cout, deberías pasarlo a ncurses

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

    return true;

}

void SistemaAlquiler::listarVehiculosDisponibles() {
    clear();
    mvprintw(0, 0,  "=====================================");
    mvprintw(1, 0,  "VEHICULOS DISPONIBLES");
    mvprintw(2, 0,  "=====================================");

    int y = 4;  // posición vertical inicial
    int contador = 0;

    for (Vehiculo* v : vehiculos) {
        if (v->getActivo()) {
            v->mostrarInfo(y);
            y += 4; // deja espacio entre cada vehículo
            contador++;
        }
    }

    if (contador == 0) {
        mvprintw(y, 0, "No hay vehículos disponibles.");
        y++;
    } else {
        mvprintw(y, 0, "Total disponibles: %d", contador);
        y++;
    }


    mvprintw(y + 2, 0, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();

}

void SistemaAlquiler::listarTodosVehiculos() {
    clear();
    mvprintw(0, 0,  "=====================================");
    mvprintw(1, 0,  "TODOS LOS VEHICULOS");
    mvprintw(2, 0,  "=====================================");

    int y = 4;  // posición vertical inicial

    for (Vehiculo* v : vehiculos) {
        v->mostrarInfo(y);  // tu mostrarInfo ahora usa mvprintw
        y += 4; // deja espacio entre cada vehículo
    }

    mvprintw(y, 0, "Total vehiculos: %zu", vehiculos.size());
    mvprintw(y + 2, 0, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();

}

void SistemaAlquiler::listarContratos() {
    clear();
    mvprintw(0, 0,  "=====================================");
    mvprintw(1, 0,  "TODOS LOS VEHICULOS");
    mvprintw(2, 0,  "=====================================");

    int y = 4;  // posición vertical inicial

    for (Vehiculo* v : vehiculos) {
        v->mostrarInfo(y);  // tu mostrarInfo ahora usa mvprintw
        y += 4; // deja espacio entre cada vehículo
    }

    mvprintw(y, 0, "Total vehiculos: %zu", vehiculos.size());
    mvprintw(y + 2, 0, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();

}

void SistemaAlquiler::listarClientesRegistrados() {
    clear();
    mvprintw(0, 0,  "=====================================");
    mvprintw(1, 0,  "CLIENTES REGISTRADOS");
    mvprintw(2, 0,  "=====================================");

    int y = 4;
    for (Cliente* c : clientes) {
        c->mostrarInfo(y);
        y += 5; // deja un poco de espacio entre clientes
    }

    mvprintw(y, 0, "Total clientes: %d", (int)clientes.size());
    mvprintw(y + 2, 0, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();

}

void SistemaAlquiler::mostrarHistorialCompleto() {
    clear();
    historial.mostrarHistorial();
}

void SistemaAlquiler::mostrarHistorialCliente(int dni) {
    clear();
    Cliente* cliente = buscarCliente(dni);
    if (cliente != nullptr) {
        historial.mostrarContratoPorCliente(*cliente);
    } else {
        mvprintw(0, 0,  "Cliente no encontrado.");
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