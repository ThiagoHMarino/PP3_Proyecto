
Sistema de Alquiler de Vehículos Inteligente
Sistema de gestión de alquiler de vehículos desarrollado en C++ con integración de base de datos SQLite y análisis predictivo mediante Python.

Características Principales

Sistema Principal (C++)
Gestión completa de clientes, vehículos y contratos.
Interfaz de usuario interactiva con menú navegable (PDCurses).
Validación robusta de datos de entrada.
Cálculo automático de costos con recargos por tiempo excedido.
Persistencia de datos mediante SQLite.
Historial completo de transacciones.
Módulo de Inteligencia Artificial (Python)
Predicción de demanda basada en datos históricos.
Recomendación de vehículos según tiempo de alquiler.
Agrupamiento (clustering) de clientes con características similares.
Visualización de patrones de uso.

Requisitos

Para el Sistema C++:
CMake 3.27 o superior.

Compilador compatible con C++17.

PDCurses (Windows) o NCurses (Linux).
SQLite3.

vcpkg (gestor de paquetes recomendado).

Para el Módulo de IA (Python):
Python 3.8+

pandas
numpy
scikit-learn (sklearn)
matplotlib

------------------->Instalación<----------------
1. Clonar el Repositorio:
https://github.com/ThiagoHMarino/PP3_Proyecto.git
2. Instalar Dependencias C++ (Windows con vcpkg):
vcpkg install pdcurses:x64-windows
vcpkg install sqlite3:x64-windows
vcpkg integrate install
3. Compilar el Proyecto:
mkdir build
cd build
cmake ..
cmake --build .
4. Instalar Dependencias Python:
pip install pandas numpy scikit-learn matplotlib
Uso del Sistema
Ejecución:
./PP3_Proyecto     # En Windows: PP3_Proyecto.exe
Controles del Menú:
Flechas ↑↓: Navegar entre opciones.

Enter: Seleccionar opción.

Opciones del Sistema:

Registrar Cliente.
Registrar Vehículo.
Listar Clientes.
Listar Vehículos Disponibles.
Listar Todos los Vehículos.
Crear Contrato.
Cerrar Contrato.
Ver Contratos Activos.
Ver Historial Completo.
Limpiar Base de Datos.


Base de Datos (SQLite)

Estructura Principal:

Tabla Cliente
dni INTEGER PRIMARY KEY
nombre TEXT NOT NULL
apellido TEXT NOT NULL
edad INTEGER NOT NULL

Tabla Vehiculo
patente TEXT PRIMARY KEY
marca TEXT NOT NULL
anio INTEGER NOT NULL
precioBase REAL NOT NULL
disponible INTEGER DEFAULT 1
cilindradas INTEGER DEFAULT 0
puertas INTEGER DEFAULT 0

Tabla Contrato
id_contrato INTEGER PRIMARY KEY AUTOINCREMENT
dni_cliente INTEGER (FK)
patente_vehiculo TEXT (FK)
tiempo_establecido REAL
tiempo_inicio INTEGER
costo REAL DEFAULT 0
cargo_extra REAL DEFAULT 0
activo INTEGER DEFAULT 1
│
├── src/
│   ├── main.cpp              # Punto de entrada
│   ├── menu.cpp/h            # Interfaz de usuario
│   ├── Archivo.cpp/h         # Clases principales
│   ├── database.cpp/h        # Gestión de SQLite
│   └── sqlite3.c/h           # SQLite embebido
│
├── prediction.py             # IA: Predicción de vehículos
├── prediction2.py            # IA: Clustering
├── alquiler.db               # Base de datos
├── CMakeLists.txt            # Configuración de compilación
└── README.md                 # Documentación
Este proyecto fue desarrollado como trabajo académico para la materia Paradigmas de Programación III de la Facultad de Ingeniería del Ejército (FIE).
