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
    cout<<"Marca: "<<marca<<" Patente: "<<patente<<" Año:"<< anio
    <<" Precio Base: "<< precioBase <<" Disponibilidad: "<<disponible<<endl;
}
int Vehiculo::vivas=0;

//AUTO
Auto::Auto(string m, string pat, int a, float pB, int puer):Vehiculo(m,pat,a,pB),puertas(puer){}

void Auto::mostrarInfo(){
    cout << "AUTO" << endl;
    cout<<"Marca: "<<marca<<" Patente: "<<patente<<" Año:"<< anio
        <<" Precio Base: "<< precioBase <<" Disponibilidad: "<<disponible<< " PUERTAS: "<< puertas <<endl;
}

//MOTO
Moto::Moto(string m, string pat, int a, float pB, int cil):Vehiculo(m,p,a,pB),cilindradas(cil){}

void Moto::mostrarInfo(){
    cout << "MOTO" << endl;
    cout<<"Marca: "<<marca<<" Patente: "<<patente<<" Año:"<< anio
        <<" Precio Base: "<< precioBase <<" Disponibilidad: "<<disponible<< " CILINDRADAS: "<< cilindradas <<endl;
}




