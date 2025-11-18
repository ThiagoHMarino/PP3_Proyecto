#!/usr/bin/env python3

import sqlite3
import pandas as pd
from sklearn.cluster import KMeans
from sklearn.preprocessing import StandardScaler
import sys

# ===========================
# 1. Recibir path
# ===========================
db = sys.argv[1] if len(sys.argv) > 1 else "alquiler.db"

conn = sqlite3.connect(db)
df_cliente = pd.read_sql_query("SELECT * FROM cliente", conn)
df_contrato = pd.read_sql_query("SELECT * FROM contrato", conn)
conn.close()

print("\n========== CLUSTERING DE CLIENTES ==========\n")

# Crear gasto total por cliente
df_contrato["gasto"] = df_contrato["costo"] + df_contrato["cargo_extra"]

df = df_contrato.groupby("dni_cliente")["gasto"].sum().reset_index()
df = df.merge(df_cliente, left_on="dni_cliente", right_on="dni")

# Variables
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

    print(f"\n=== CLUSTER {i} ===")
    print(f"Cantidad de clientes: {len(grupo)}")
    print(f"Edad promedio: {grupo['edad'].mean():.2f}")
    print(f"Gasto promedio: ${grupo['gasto'].mean():.2f}")

    if grupo["gasto"].mean() < 5000:
        print("Tipo: Clientes de bajo gasto.")
    elif grupo["gasto"].mean() < 15000:
        print("Tipo: Clientes normales.")
    else:
        print("Tipo: Clientes de alto gasto.")

    print("\nLista de clientes:")
    for _, row in grupo.iterrows():
        print(f" - {row['nombre']} {row['apellido']} (DNI {row['dni']})")

print("\n==========================================\n")

