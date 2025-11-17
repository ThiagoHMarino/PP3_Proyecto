import sqlite3
import pandas as pd
from sklearn.neighbors import NearestNeighbors

DB = "mi_base.db"

print("\n===============================")
print("     SISTEMA DE SUGERENCIAS")
print("===============================\n")

# ---------------------------
# Cargar datos combinados
# ---------------------------

con = sqlite3.connect(DB)

query = """
SELECT 
    c.dni_cliente,
    cli.edad,
    co.patente,
    veh.precioBase,
    co.costo,
    co.cargo_extra,
    co.tiempo_establecido
FROM contrato co
JOIN clientes cli ON cli.dni = co.dni_cliente
JOIN vehiculos veh ON veh.patente = co.patente;
"""

df = pd.read_sql_query(query, con)
con.close()

# ---------------------------
# Preparar datos
# ---------------------------

# Para similitud de clientes usamos:
# edad, costo, cargo extra, duracion
X = df[["edad", "costo", "cargo_extra", "tiempo_establecido"]]

# Entrenamos KNN
knn = NearestNeighbors(n_neighbors=3)
knn.fit(X)

# ---------------------------
# Función de recomendación
# ---------------------------

def recomendar(edad, costo_prev, extra_prev, tiempo_prev):
    nuevo = [[edad, costo_prev, extra_prev, tiempo_prev]]
    distancias, indices = knn.kneighbors(nuevo)

    similares = df.iloc[indices[0]]

    # El vehículo más repetido entre los similares
    recomendacion = (similares["patente"]
                     .value_counts()
                     .idxmax())

    print(f"\nCliente similar → Recomendar vehículo: {recomendacion}")

# ---------------------------
# Ejemplo
# ---------------------------

print("Ejemplo de recomendación para:")
print("- Edad: 30")
print("- Costo típico: 12000")
print("- Extra promedio: 50")
print("- Tiempo: 7 días")

recomendar(30, 12000, 50, 7)
