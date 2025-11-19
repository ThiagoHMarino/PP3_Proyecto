#!/usr/bin/env python3

import sqlite3
import pandas as pd
from sklearn.cluster import KMeans
from sklearn.preprocessing import StandardScaler
import sys
import shutil

# ----- centrado-----
def center(text):
    width = shutil.get_terminal_size().columns
    return text.center(width)

# ===========================
# 1. Recibir path
# ===========================
db = sys.argv[1] if len(sys.argv) > 1 else "alquiler.db"

conn = sqlite3.connect(db)
df_cliente = pd.read_sql_query("SELECT * FROM cliente", conn)
df_contrato = pd.read_sql_query("SELECT * FROM contrato", conn)
conn.close()

print("\n" + center("========== CLUSTERING DE CLIENTES ==========\n"))

# Crear gasto total por cliente
df_contrato["gasto"] = df_contrato["costo"] + df_contrato["cargo_extra"]

df = df_contrato.groupby("dni_cliente")["gasto"].sum().reset_index()
df = df.merge(df_cliente, left_on="dni_cliente", right_on="dni")

# Edades como int (no float)
df["edad"] = df["edad"].astype(int)

# Variables para clustering
X = df[["edad", "gasto"]]

scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

kmeans = KMeans(n_clusters=3, random_state=0)
df["cluster"] = kmeans.fit_predict(X_scaled)

# ===========================
# Mostrar resultados
# ===========================
for i in range(3):
    grupo = df[df["cluster"] == i]

    #print(center(f"\n=== CLUSTER {i+1} ==="))
    print("\n")
    print(center(f"=== CLUSTER {i+1} ===\n"))
    print(center(f"Cantidad de clientes: {len(grupo)}"))

    if len(grupo) > 0:
        print(center(f"Edad promedio: {grupo['edad'].mean()}"))
        print(center(f"Gasto promedio: ${grupo['gasto'].mean():.2f}"))
    else:
        print(center("Edad promedio: N/A"))
        print(center("Gasto promedio: N/A"))

    # Tipo de cliente
    if len(grupo) > 0:
        prom = grupo["gasto"].mean()
        if prom < 5000:
            tipo = "Clientes de bajo gasto."
        elif prom < 15000:
            tipo = "Clientes normales."
        else:
            tipo = "Clientes de alto gasto."
    else:
        tipo = "Sin datos."

    print(center(f"Tipo: {tipo}\n"))

    print(center("Lista de clientes:"))

    for _, row in grupo.iterrows():
        linea = f" - {row['nombre']} {row['apellido']} (DNI {row['dni']})"
        print(center(linea))

print("\n" + center("==========================================\n"))

