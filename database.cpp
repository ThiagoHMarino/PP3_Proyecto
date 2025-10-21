#include "database.h"
#include <sqlite3.h>
#include <string>
#include <iostream>
//
DataBase :: DataBase(string nBD): datab(nullptr), nombreBD(nBD) {
    int retorno = sqlite3_open(nombreBD.c_str(), &datab);

    if(retorno!=SQLITE_OK){ //SQLITE_OK: operacion de exito.
        //hubo error:
        cout << "Error al abrir la BD: " << sqlite3_errmsg(datab) << endl; //sqlite3_errmsg(datab): mensaje de error.
        datab = nullptr;
    }else{
        cout << "BASE DE DATOS: " << nombreBD << " abierta con éxito!" << endl;
    }
}

DataBase :: ~DataBase(){
    if (datab!=nullptr) {
        sqlite3_close(datab);
        cout << "BD cerrada." << endl;
    }
}
