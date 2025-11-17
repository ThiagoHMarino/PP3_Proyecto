// IMPLEMENTACIONES

#include "Archivo.h"
#include <iostream>
#include <vector>
#include "database.h"
#include <curses.h>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string.h>

void Historial::mostrarHistorial() {
    clear();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int menu_ancho = 90;
    int y_inicio = 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    // Título centrado
    const char* titulo = "HISTORIAL DE CONTRATOS";
    int titulo_x = x_inicio + (menu_ancho - strlen(titulo)) / 2;

    mvprintw(y_inicio, titulo_x, "%s", titulo);
    mvprintw(y_inicio + 1, x_inicio, "%s", string(menu_ancho, '=').c_str());

    int y = y_inicio + 3;

    if (lista_contratos.empty()) {
        mvprintw(y++, x_inicio, "No hay contratos en el historial.");
        y++;
    } else {
        for (size_t i = 0; i < lista_contratos.size(); i++) {
            Contrato* c = lista_contratos[i];

            mvprintw(y++, x_inicio, "CONTRATO #%d (Finalizado)", c->getId());
            mvprintw(y++, x_inicio, "%s", string(menu_ancho, '-').c_str());

            // Cliente
            Cliente cliente = c->getCliente();
            mvprintw(y++, x_inicio + 2, "CLIENTE:");
            mvprintw(y++, x_inicio + 4, "Nombre: %s %s",
                     cliente.getNombre().c_str(),
                     cliente.getApellido().c_str());
            mvprintw(y++, x_inicio + 4, "DNI:    %d", cliente.getDni());
            mvprintw(y++, x_inicio + 4, "Edad:   %d años", cliente.getEdad());

            y++;

            // Vehículo
            Vehiculo* v = c->getVehiculo();
            mvprintw(y++, x_inicio + 2, "VEHICULO:");
            mvprintw(y++, x_inicio + 4, "Marca:   %s", v->getMarca().c_str());
            mvprintw(y++, x_inicio + 4, "Patente: %s", v->getPatente().c_str());
            mvprintw(y++, x_inicio + 4, "Año:     %d", v->getAnio());

            y++;

            // Detalles del contrato
            mvprintw(y++, x_inicio + 2, "DETALLES:");
            float horasEstablecidas = c->getTiempoEstablecido().count() / 3600.0f;
            mvprintw(y++, x_inicio + 4, "Tiempo contratado: %.2f horas", horasEstablecidas);
            mvprintw(y++, x_inicio + 4, "Costo total:       $%.2f", c->getCosto());

            y += 2; // Espacio entre contratos
        }
    }

    // Resumen
    mvprintw(y++, x_inicio, "%s", string(menu_ancho, '=').c_str());
    mvprintw(y++, x_inicio, "Total de contratos finalizados: %zu", lista_contratos.size());

    mvprintw(y + 2, x_inicio, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();
}

void Historial::mostrarContratoPorCliente(Cliente cliente) {
    clear();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int menu_ancho = 90;
    int y_inicio = 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    // Título centrado
    char titulo[100];
    snprintf(titulo, sizeof(titulo), "HISTORIAL DE %s %s",
             cliente.getNombre().c_str(),
             cliente.getApellido().c_str());
    int titulo_x = x_inicio + (menu_ancho - strlen(titulo)) / 2;

    mvprintw(y_inicio, titulo_x, "%s", titulo);
    mvprintw(y_inicio + 1, x_inicio, "%s", string(menu_ancho, '=').c_str());
    mvprintw(y_inicio + 2, x_inicio, "DNI: %d", cliente.getDni());

    int y = y_inicio + 4;

    bool encontrado = false;
    int contador = 0;

    for (Contrato* c : lista_contratos) {
        if (c->getCliente().getDni() == cliente.getDni()) {
            encontrado = true;
            contador++;

            mvprintw(y++, x_inicio, "CONTRATO #%d", c->getId());
            mvprintw(y++, x_inicio, "%s", string(menu_ancho, '-').c_str());

            // Vehículo
            Vehiculo* v = c->getVehiculo();
            mvprintw(y++, x_inicio + 2, "Vehiculo:          %s - %s",
                     v->getMarca().c_str(),
                     v->getPatente().c_str());

            // Detalles
            float horasEstablecidas = c->getTiempoEstablecido().count() / 3600.0f;
            mvprintw(y++, x_inicio + 2, "Tiempo contratado: %.2f horas", horasEstablecidas);
            mvprintw(y++, x_inicio + 2, "Costo total:       $%.2f", c->getCosto());

            y += 2; // Espacio entre contratos
        }
    }

    if (!encontrado) {
        mvprintw(y++, x_inicio, "No se encontraron contratos para este cliente.");
        y++;
    } else {
        mvprintw(y++, x_inicio, "%s", string(menu_ancho, '=').c_str());
        mvprintw(y++, x_inicio, "Total de contratos: %d", contador);
    }

    mvprintw(y + 2, x_inicio, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();
}


void SistemaAlquiler::listarClientesRegistrados() {
    clear();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int menu_ancho = 70;
    int y_inicio = 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    // Título centrado
    const char* titulo = "CLIENTES REGISTRADOS";
    int titulo_x = x_inicio + (menu_ancho - strlen(titulo)) / 2;

    mvprintw(y_inicio, titulo_x, "%s", titulo);
    mvprintw(y_inicio + 1, x_inicio, "%s", string(menu_ancho, '=').c_str());

    int y = y_inicio + 3;

    if (clientes.empty()) {
        mvprintw(y, x_inicio, "No hay clientes registrados.");
        y += 2;
    } else {
        for (size_t i = 0; i < clientes.size(); i++) {
            Cliente* c = clientes[i];

            // Encabezado del cliente
            mvprintw(y++, x_inicio, "Cliente #%zu", i + 1);
            mvprintw(y++, x_inicio, "%s", string(menu_ancho, '-').c_str());

            // Información del cliente
            mvprintw(y++, x_inicio + 2, "Nombre:   %s %s",
                     c->getNombre().c_str(),
                     c->getApellido().c_str());
            mvprintw(y++, x_inicio + 2, "Edad:     %d años", c->getEdad());
            mvprintw(y++, x_inicio + 2, "DNI:      %d", c->getDni());

            y++; // Espacio entre clientes
        }
    }

    // Resumen
    mvprintw(y++, x_inicio, "%s", string(menu_ancho, '=').c_str());
    mvprintw(y++, x_inicio, "Total de clientes: %zu", clientes.size());

    mvprintw(y + 2, x_inicio, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();
}

void SistemaAlquiler::listarVehiculosDisponibles() {
    clear();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int menu_ancho = 80;
    int y_inicio = 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    // Título centrado
    const char* titulo = "VEHICULOS DISPONIBLES";
    int titulo_x = x_inicio + (menu_ancho - strlen(titulo)) / 2;

    mvprintw(y_inicio, titulo_x, "%s", titulo);
    mvprintw(y_inicio + 1, x_inicio, "%s", string(menu_ancho, '=').c_str());

    int y = y_inicio + 3;
    int contador = 0;

    for (Vehiculo* v : vehiculos) {
        if (v->getActivo()) {
            contador++;

            // Determinar tipo
            Auto* auto_ptr = dynamic_cast<Auto*>(v);
            Moto* moto_ptr = dynamic_cast<Moto*>(v);

            // Encabezado
            if (auto_ptr) {
                mvprintw(y++, x_inicio, "AUTO #%d", contador);
            } else if (moto_ptr) {
                mvprintw(y++, x_inicio, "MOTO #%d", contador);
            }
            mvprintw(y++, x_inicio, "%s", string(menu_ancho, '-').c_str());

            // Información básica
            mvprintw(y++, x_inicio + 2, "Marca:        %s", v->getMarca().c_str());
            mvprintw(y++, x_inicio + 2, "Patente:      %s", v->getPatente().c_str());
            mvprintw(y++, x_inicio + 2, "Año:          %d", v->getAnio());
            mvprintw(y++, x_inicio + 2, "Precio/hora:  $%.2f", v->getPrecioBase());

            // Información específica
            if (auto_ptr) {
                mvprintw(y++, x_inicio + 2, "Puertas:      %d", auto_ptr->getPuertas());
            } else if (moto_ptr) {
                mvprintw(y++, x_inicio + 2, "Cilindradas:  %dcc", moto_ptr->getCilindradas());
            }

            y++; // Espacio entre vehículos
        }
    }

    if (contador == 0) {
        mvprintw(y++, x_inicio, "No hay vehiculos disponibles en este momento.");
        y++;
    }

    // Resumen
    mvprintw(y++, x_inicio, "%s", string(menu_ancho, '=').c_str());
    mvprintw(y++, x_inicio, "Total disponibles: %d", contador);

    mvprintw(y + 2, x_inicio, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();
}

void SistemaAlquiler::listarTodosVehiculos() {
    clear();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int menu_ancho = 80;
    int y_inicio = 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    // Título centrado
    const char* titulo = "TODOS LOS VEHICULOS";
    int titulo_x = x_inicio + (menu_ancho - strlen(titulo)) / 2;

    mvprintw(y_inicio, titulo_x, "%s", titulo);
    mvprintw(y_inicio + 1, x_inicio, "%s", string(menu_ancho, '=').c_str());

    int y = y_inicio + 3;

    for (size_t i = 0; i < vehiculos.size(); i++) {
        Vehiculo* v = vehiculos[i];

        // Determinar tipo
        Auto* auto_ptr = dynamic_cast<Auto*>(v);
        Moto* moto_ptr = dynamic_cast<Moto*>(v);

        // Encabezado
        if (auto_ptr) {
            mvprintw(y++, x_inicio, "AUTO #%zu", i + 1);
        } else if (moto_ptr) {
            mvprintw(y++, x_inicio, "MOTO #%zu", i + 1);
        }
        mvprintw(y++, x_inicio, "%s", string(menu_ancho, '-').c_str());

        // Información básica
        mvprintw(y++, x_inicio + 2, "Marca:         %s", v->getMarca().c_str());
        mvprintw(y++, x_inicio + 2, "Patente:       %s", v->getPatente().c_str());
        mvprintw(y++, x_inicio + 2, "Año:           %d", v->getAnio());
        mvprintw(y++, x_inicio + 2, "Precio/hora:   $%.2f", v->getPrecioBase());
        mvprintw(y++, x_inicio + 2, "Disponible:    %s", v->getActivo() ? "Si" : "No");

        // Información específica
        if (auto_ptr) {
            mvprintw(y++, x_inicio + 2, "Puertas:       %d", auto_ptr->getPuertas());
        } else if (moto_ptr) {
            mvprintw(y++, x_inicio + 2, "Cilindradas:   %dcc", moto_ptr->getCilindradas());
        }

        y++; // Espacio entre vehículos
    }

    // Resumen
    mvprintw(y++, x_inicio, "%s", string(menu_ancho, '=').c_str());
    mvprintw(y++, x_inicio, "Total vehiculos: %zu", vehiculos.size());

    mvprintw(y + 2, x_inicio, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();
}

void SistemaAlquiler::listarContratos() {
    clear();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int menu_ancho = 80;
    int y_inicio = 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    // Título centrado
    const char* titulo = "CONTRATOS ACTIVOS";
    int titulo_x = x_inicio + (menu_ancho - strlen(titulo)) / 2;

    mvprintw(y_inicio, titulo_x, "%s", titulo);
    mvprintw(y_inicio + 1, x_inicio, "%s", string(menu_ancho, '=').c_str());

    int y = y_inicio + 3;

    if (contratos_activos.empty()) {
        mvprintw(y++, x_inicio, "No hay contratos activos en este momento.");
        y++;
    } else {
        for (size_t i = 0; i < contratos_activos.size(); i++) {
            Contrato* c = contratos_activos[i];

            mvprintw(y++, x_inicio, "CONTRATO #%d", c->getId());
            mvprintw(y++, x_inicio, "%s", string(menu_ancho, '-').c_str());

            // Cliente
            mvprintw(y++, x_inicio + 2, "Cliente:       %s %s (DNI: %d)",
                     c->getCliente().getNombre().c_str(),
                     c->getCliente().getApellido().c_str(),
                     c->getCliente().getDni());

            // Vehículo
            Vehiculo* v = c->getVehiculo();
            mvprintw(y++, x_inicio + 2, "Vehiculo:      %s - %s",
                     v->getMarca().c_str(),
                     v->getPatente().c_str());

            // Tiempo
            float horas = c->getTiempoEstablecido().count() / 3600.0f;
            mvprintw(y++, x_inicio + 2, "Duracion:      %.2f horas", horas);

            y++; // Espacio entre contratos
        }
    }

    // Resumen
    mvprintw(y++, x_inicio, "%s", string(menu_ancho, '=').c_str());
    mvprintw(y++, x_inicio, "Total contratos activos: %zu", contratos_activos.size());

    mvprintw(y + 2, x_inicio, "Presiona cualquier tecla para continuar...");
    refresh();
    getch();
}

void SistemaAlquiler::mostrarHistorialCliente(int dni) {
    clear();
    Cliente* cliente = buscarCliente(dni);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int menu_ancho = 90;
    int y_inicio = 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    if (cliente != nullptr) {
        historial.mostrarContratoPorCliente(*cliente);
    } else {
        const char* titulo = "ERROR";
        int titulo_x = x_inicio + (menu_ancho - strlen(titulo)) / 2;

        mvprintw(y_inicio, titulo_x, "%s", titulo);
        mvprintw(y_inicio + 1, x_inicio, "%s", string(menu_ancho, '=').c_str());
        mvprintw(y_inicio + 3, x_inicio, "Cliente con DNI %d no encontrado.", dni);

        mvprintw(y_inicio + 5, x_inicio, "Presiona cualquier tecla para continuar...");
        refresh();
        getch();
    }
}

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
}

void Contrato::cerrarContrato(){
    fin = system_clock::now();
    duration<float> tiempoReal=fin - inicio;
    duration<float> exceso=tiempoReal-tiempoEstablecido;

    // Obtener dimensiones de la pantalla
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Calcular dimensiones del menú
    int menu_ancho = 60;
    int menu_alto = 12;

    // Centrar el menú
    int y_inicio = ((max_y - menu_alto) / 2)+5;
    int x_inicio = (max_x - menu_ancho) / 2;


    if(exceso.count() > 0){
        float HorasExtra=exceso.count() / 3600.f;
        costo+=HorasExtra*cargoExtraporHora;
        const char* msg_t = "Tiempo excedido: %.2f horas";
        int msg_x1 = x_inicio + (menu_ancho - strlen(msg_t)) / 2;
        mvprintw(y_inicio+1, msg_x1, msg_t, HorasExtra);

        const char* msg_costoad = "Cargo adicional: $%.2f";
        int msg_x2 = x_inicio + (menu_ancho - strlen(msg_costoad)) / 2;
        mvprintw(y_inicio+2, msg_x2, msg_costoad , costo);
    }
    float HorasEstablecidas=tiempoEstablecido.count() / 3600.f;
    costo+=vehiculo->getPrecioBase()*HorasEstablecidas;
    const char* msg_costo = "Contrato cerrado. Costo total: $%.2f";
    int msg_x = x_inicio + (menu_ancho - strlen(msg_costo)) / 2;
    mvprintw(y_inicio+3, msg_x, msg_costo, costo);
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

size_t Historial::getsize() {
    return lista_contratos.size();
}

void Historial::agregarContrato(Contrato *contratoAgregar) {
    lista_contratos.push_back(contratoAgregar);
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

    // Obtener dimensiones de la pantalla
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Calcular dimensiones del menú
    int menu_ancho = 60;
    int menu_alto = 10;

    // Centrar el menú
    int y_inicio = (max_y - menu_alto) / 2;
    int x_inicio = (max_x - menu_ancho) / 2;

    // Título
    const char* titulo = "CARGANDO DATOS";
    int titulo_x = x_inicio + (menu_ancho - strlen(titulo)) / 2;

    mvprintw(y_inicio, titulo_x, "%s", titulo);
    mvprintw(y_inicio + 1, x_inicio, "%s", string(menu_ancho, '=').c_str());

    int y = y_inicio + 3;

    mvprintw(y++, x_inicio, "Cargando datos desde la base de datos...");
    refresh();

    clientes = database->cargarClientes();
    mvprintw(y++, x_inicio, "Clientes cargados: %lu", clientes.size());
    refresh();

    vehiculos = database->cargarVehiculos();
    mvprintw(y++, x_inicio, "Vehiculos cargados: %lu", vehiculos.size());
    refresh();

    contratos_activos = database->cargarContratosActivos();
    mvprintw(y++, x_inicio, "Contratos cargados: %lu", contratos_activos.size());
    refresh();

    database->cargarHistorial(&historial);
    mvprintw(y++, x_inicio, "Contratos cargados en el historial: %lu", historial.getsize());
    refresh();

    const char* msg_continuar = "Presiona cualquier tecla para continuar...";
    int continuar_x = x_inicio + (menu_ancho - strlen(msg_continuar)) / 2;
    mvprintw(y + 2, continuar_x, "%s", msg_continuar);
    refresh();

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
        //mvprintw(y++, 0, "Error: Cliente con DNI %d no encontrado.", dni);
        //mvprintw(y+1, 0, "Presiona cualquier tecla para continuar...");
        //getch();
        return nullptr;
    }

    Vehiculo* vehiculo = buscarVehiculo(patente);
    if (vehiculo == nullptr) {
        //mvprintw(y++, 0, "Error: Vehiculo con patente %s no encontrado.", patente.c_str());
        //mvprintw(y+1, 0, "Presiona cualquier tecla para continuar...");
        //getch();
        return nullptr;
    }

    if (!vehiculo->getActivo()) {
        //mvprintw(y++, 0, "Error: Vehiculo no disponible.");
        //mvprintw(y+1, 0, "Presiona cualquier tecla para continuar...");
        //getch();
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
    // COMENTAR O ELIMINAR ESTE BLOQUE COMPLETO:
    // clear();
    // int y = 0;

    Contrato* contrato = buscarContratoActivo(id_contrato);

    if (contrato == nullptr) {
        // COMENTAR O ELIMINAR ESTAS LÍNEAS:
        // mvprintw(y++, 0, "Error: Contrato #%d no encontrado.", id_contrato);
        // mvprintw(y++, 0, "Presiona cualquier tecla para continuar...");
        // getch();
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

    return true; //
}

void SistemaAlquiler::mostrarHistorialCompleto() {
    clear();
    historial.mostrarHistorial();
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