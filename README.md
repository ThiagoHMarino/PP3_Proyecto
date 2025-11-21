\subsection*{Sistema de Alquiler de Vehículos Inteligente}

Sistema de gestión de alquiler de vehículos desarrollado en \textbf{C++} con integración de base de datos \textbf{SQLite} y análisis predictivo mediante \textbf{Python}.

\subsection*{Características Principales}

\textbf{Sistema Principal (C++)}
\begin{itemize}
    \item Gestión completa de clientes, vehículos y contratos.
    \item Interfaz de usuario interactiva con menú navegable (PDCurses).
    \item Validación robusta de datos de entrada.
    \item Cálculo automático de costos con recargos por tiempo excedido.
    \item Persistencia de datos mediante SQLite.
    \item Historial completo de transacciones.
\end{itemize}

\textbf{Módulo de Inteligencia Artificial (Python)}
\begin{itemize}
    \item Predicción de demanda basada en datos históricos.
    \item Recomendación de vehículos según tiempo de alquiler.
    \item Agrupamiento (clustering) de vehículos con características similares.
    \item Visualización de patrones de uso.
\end{itemize}

\newpage
\subsection*{Requisitos}

\textbf{Para el Sistema C++:}
\begin{itemize}
    \item CMake 3.27 o superior.
    \item Compilador compatible con C++17.
    \item PDCurses (Windows) o NCurses (Linux).
    \item SQLite3.
    \item vcpkg (gestor de paquetes recomendado).
\end{itemize}

\textbf{Para el Módulo de IA (Python):}
\begin{itemize}
    \item Python 3.8+
    \item pandas
    \item numpy
    \item scikit-learn
    \item matplotlib
\end{itemize}

\newpage
\subsection*{Instalación}

\textbf{1. Clonar el Repositorio:}
\begin{verbatim}
https://github.com/ThiagoHMarino/PP3_Proyecto.git
\end{verbatim}

\textbf{2. Instalar Dependencias C++ (Windows con vcpkg):}
\begin{verbatim}
vcpkg install pdcurses:x64-windows
vcpkg install sqlite3:x64-windows
vcpkg integrate install
\end{verbatim}

\textbf{3. Compilar el Proyecto:}
\begin{verbatim}
mkdir build
cd build
cmake ..
cmake --build .
\end{verbatim}

\textbf{4. Instalar Dependencias Python:}
\begin{verbatim}
pip install pandas numpy scikit-learn matplotlib
\end{verbatim}

\newpage
\subsection*{Uso del Sistema}

\textbf{Ejecución:}
\begin{verbatim}
./PP3_Proyecto  # En Windows: PP3_Proyecto.exe
\end{verbatim}

\textbf{Controles del Menú:}
\begin{itemize}
    \item Flechas ↑↓: Navegar entre opciones.
    \item Enter: Seleccionar opción.
\end{itemize}

\textbf{Opciones del Sistema:}
\begin{enumerate}
    \item Registrar Cliente.
    \item Registrar Vehículo.
    \item Listar Clientes.
    \item Listar Vehículos Disponibles.
    \item Listar Todos los Vehículos.
    \item Crear Contrato.
    \item Cerrar Contrato.
    \item Ver Contratos Activos.
    \item Ver Historial Completo.
    \item Limpiar Base de Datos.
\end{enumerate}

\newpage
\subsection*{Base de Datos (SQLite)}

\textbf{Estructura Principal:}
\\[0.5cm]
\textbf{Tabla \texttt{Cliente}}
\begin{verbatim}
dni INTEGER PRIMARY KEY
nombre TEXT NOT NULL
apellido TEXT NOT NULL
edad INTEGER NOT NULL
\end{verbatim}

\textbf{Tabla \texttt{Vehiculo}}
\begin{verbatim}
patente TEXT PRIMARY KEY
marca TEXT NOT NULL
anio INTEGER NOT NULL
precioBase REAL NOT NULL
disponible INTEGER DEFAULT 1
cilindradas INTEGER DEFAULT 0
puertas INTEGER DEFAULT 0
\end{verbatim}

\textbf{Tabla \texttt{Contrato}}
\begin{verbatim}
id_contrato INTEGER PRIMARY KEY AUTOINCREMENT
dni_cliente INTEGER (FK)
patente_vehiculo TEXT (FK)
tiempo_establecido REAL
tiempo_inicio INTEGER
costo REAL DEFAULT 0
cargo_extra REAL DEFAULT 0
activo INTEGER DEFAULT 1
\end{verbatim}

\textbf{Consulta Ejemplo:}
\begin{verbatim}
SELECT * FROM Cliente;
\end{verbatim}

\newpage
\subsection*{Módulo de Inteligencia Artificial (Python)}

\textbf{1. Predicción de Vehículo Óptimo (\texttt{prediction.py}):}
\begin{itemize}
    \item Predice el año de vehículo ideal según el tiempo de alquiler.
    \item Basado en regresión lineal.
\end{itemize}

\textbf{2. Análisis de Clusters (\texttt{prediction2.py}):}
\begin{itemize}
    \item Agrupa vehículos por año, precio y características.
    \item Aplica K-Means y genera gráficos de patrones de uso.
\end{itemize}

\newpage

\subsection*{Sistema de Alquiler de Vehículos Inteligente}

Sistema de gestión de alquiler de vehículos desarrollado en \textbf{C++} con integración de base de datos \textbf{SQLite} y análisis predictivo mediante \textbf{Python}.

\subsection*{Características Principales}

\textbf{Sistema Principal (C++)}
\begin{itemize}
    \item Gestión completa de clientes, vehículos y contratos.
    \item Interfaz de usuario interactiva con menú navegable (PDCurses).
    \item Validación robusta de datos de entrada.
    \item Cálculo automático de costos con recargos por tiempo excedido.
    \item Persistencia de datos mediante SQLite.
    \item Historial completo de transacciones.
\end{itemize}

\textbf{Módulo de Inteligencia Artificial (Python)}
\begin{itemize}
    \item Predicción de demanda basada en datos históricos.
    \item Recomendación de vehículos según tiempo de alquiler.
    \item Agrupamiento (clustering) de vehículos con características similares.
    \item Visualización de patrones de uso.
\end{itemize}

\newpage
\subsection*{Requisitos}

\textbf{Para el Sistema C++:}
\begin{itemize}
    \item CMake 3.27 o superior.
    \item Compilador compatible con C++17.
    \item PDCurses (Windows) o NCurses (Linux).
    \item SQLite3.
    \item vcpkg (gestor de paquetes recomendado).
\end{itemize}

\textbf{Para el Módulo de IA (Python):}
\begin{itemize}
    \item Python 3.8+
    \item pandas
    \item numpy
    \item scikit-learn
    \item matplotlib
\end{itemize}

\newpage
\subsection*{Instalación}

\textbf{1. Clonar el Repositorio:}
\begin{verbatim}
https://github.com/ThiagoHMarino/PP3_Proyecto.git
\end{verbatim}

\textbf{2. Instalar Dependencias C++ (Windows con vcpkg):}
\begin{verbatim}
vcpkg install pdcurses:x64-windows
vcpkg install sqlite3:x64-windows
vcpkg integrate install
\end{verbatim}

\textbf{3. Compilar el Proyecto:}
\begin{verbatim}
mkdir build
cd build
cmake ..
cmake --build .
\end{verbatim}

\textbf{4. Instalar Dependencias Python:}
\begin{verbatim}
pip install pandas numpy scikit-learn matplotlib
\end{verbatim}

\newpage
\subsection*{Uso del Sistema}

\textbf{Ejecución:}
\begin{verbatim}
./PP3_Proyecto  # En Windows: PP3_Proyecto.exe
\end{verbatim}

\textbf{Controles del Menú:}
\begin{itemize}
    \item Flechas ↑↓: Navegar entre opciones.
    \item Enter: Seleccionar opción.
\end{itemize}

\textbf{Opciones del Sistema:}
\begin{enumerate}
    \item Registrar Cliente.
    \item Registrar Vehículo.
    \item Listar Clientes.
    \item Listar Vehículos Disponibles.
    \item Listar Todos los Vehículos.
    \item Crear Contrato.
    \item Cerrar Contrato.
    \item Ver Contratos Activos.
    \item Ver Historial Completo.
    \item Limpiar Base de Datos.
\end{enumerate}

\newpage
\subsection*{Base de Datos (SQLite)}

\textbf{Estructura Principal:}
\\[0.5cm]
\textbf{Tabla \texttt{Cliente}}
\begin{verbatim}
dni INTEGER PRIMARY KEY
nombre TEXT NOT NULL
apellido TEXT NOT NULL
edad INTEGER NOT NULL
\end{verbatim}

\textbf{Tabla \texttt{Vehiculo}}
\begin{verbatim}
patente TEXT PRIMARY KEY
marca TEXT NOT NULL
anio INTEGER NOT NULL
precioBase REAL NOT NULL
disponible INTEGER DEFAULT 1
cilindradas INTEGER DEFAULT 0
puertas INTEGER DEFAULT 0
\end{verbatim}

\textbf{Tabla \texttt{Contrato}}
\begin{verbatim}
id_contrato INTEGER PRIMARY KEY AUTOINCREMENT
dni_cliente INTEGER (FK)
patente_vehiculo TEXT (FK)
tiempo_establecido REAL
tiempo_inicio INTEGER
costo REAL DEFAULT 0
cargo_extra REAL DEFAULT 0
activo INTEGER DEFAULT 1
\end{verbatim}

\textbf{Consulta Ejemplo:}
\begin{verbatim}
SELECT * FROM Cliente;
\end{verbatim}

\newpage
\subsection*{Módulo de Inteligencia Artificial (Python)}

\textbf{1. Predicción de Vehículo Óptimo (\texttt{prediction.py}):}
\begin{itemize}
    \item Predice el año de vehículo ideal según el tiempo de alquiler.
    \item Basado en regresión lineal.
\end{itemize}

\textbf{2. Análisis de Clusters (\texttt{prediction2.py}):}
\begin{itemize}
    \item Agrupa vehículos por año, precio y características.
    \item Aplica K-Means y genera gráficos de patrones de uso.
\end{itemize}

\newpage
\subsection*{Estructura del Proyecto}

\begin{verbatim}
Proyecto-Alquiler-Vehiculos/
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
\end{verbatim}

