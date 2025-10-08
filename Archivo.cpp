// IMPLEMENTACIONES

#include "Archivo.h"
#include <iostream>
#include <vector>

//CLIENTE
Cliente :: Cliente(string n, string a, int e, int d):nombre(n), apellido(a), edad(e), dni(d){}
string Cliente::getNombre() const {return nombre;}
string Cliente::getApellido() const {return apellido;}
int Cliente::getEdad() const{return edad;}
int Cliente::getDni() const {return dni;}
void Cliente::mostrarInfo() const {
    cout<<"Nombre: "<<nombre<< " Apellido: "<< apellido <<" Edad: "<<edad<<" Dni: "<<dni<<endl;
}

//VEHÍCULO
Vehiculo::Vehiculo(string m, string p, int a, float pB):marca(m), patente(p), anio(a), precioBase(pB), disponible{true}{vivas++;}
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

//AUTO
Auto::Auto(string m, string pat, int a, float pB, int puer):Vehiculo(m,pat,a,pB),puertas(puer){}

void Auto::mostrarInfo(){
    cout << "=====================================" << endl;
    cout << "AUTO" << endl;
    cout<<"Marca: "<<marca<<" Patente: "<<patente<<" Año:"<< anio
        <<" Precio Base: "<< precioBase <<" Disponibilidad: "<<disponible<< " PUERTAS: "<< puertas <<endl;
    cout << "=====================================" << endl;
}

//MOTO
Moto::Moto(string m, string pat, int a, float pB, int cil):Vehiculo(m,pat,a,pB),cilindradas(cil){}

void Moto::mostrarInfo(){
    cout << "=====================================" << endl;
    cout << "MOTO" << endl;
    cout<<"Marca: "<<marca<<" Patente: "<<patente<<" Año:"<< anio
        <<" Precio Base: "<< precioBase <<" Disponibilidad: "<<disponible<< " CILINDRADAS: "<< cilindradas <<endl;
    cout << "=====================================" << endl;
}

//CONTRATO
Contrato::Contrato(int id, Cliente c, Vehiculo* v, float tiempoHoras, float costoBase):
id_contrato(id),cliente(c),vehiculo(v),tiempoEstablecido(tiempoHoras),costo(costoBase){}

Cliente Contrato::getCliente() const{return cliente;}

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



