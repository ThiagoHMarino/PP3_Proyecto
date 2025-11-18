#!/usr/bin/env python3

import sqlite3
import pandas as pd
from sklearn.preprocessing import StandardScaler
from sklearn.neighbors import NearestNeighbors
import sys

db = sys.argv[1] if len(sys.argv) > 1 else "alquiler.db"

conn = sqlite3.connect(db)
df = pd.read_sql_query("SELECT * FROM Vehiculo", conn)
conn.close()

print("\n========== RECOMENDADOR DE VEHÍCULOS ==========\n")

patente = input("Ingrese la patente para recomendar similares: ").strip().upper()

# --- Caso 1: la patente NO existe → intentar sugerencias similares ---
if patente not in df["patente"].values:

    def parecido(a, b):
        if len(a) != len(b):
            return False
        mismatches = sum(1 for x, y in zip(a, b) if x != y)
        return mismatches <= 2  # permitido hasta 2 diferencias

    sugerencias = [p for p in df["patente"].values if parecido(patente, p)]

    if not sugerencias:
        print("\nLa patente no existe y no se encontraron parecidas.\n")
        exit()

    print("\nLa patente no existe. ¿Quizás quiso decir?:\n")
    for s in sugerencias:
        print(" -", s)
    print()
    exit()

# --- Caso 2: existe → usar KNN pero sin recomendar cosas absurdas ---

X = df[["cilindradas", "precioBase", "anio"]]

scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

nn = NearestNeighbors(n_neighbors=5)
nn.fit(X_scaled)

idx = df.index[df["patente"] == patente][0]

dist, ind = nn.kneighbors([X_scaled[idx]])

print("\nVehículo consultado:")
print(df.iloc[idx], "\n")

print("Recomendaciones:\n")

# Filtro adicional: no recomendar cosas muy diferentes
for i in ind[0][1:]:
    row = df.iloc[i]

    # Reglas para evitar sugerencias absurdas:
    if abs(row["anio"] - df.loc[idx, "anio"]) > 20:
        continue
    if abs(row["cilindradas"] - df.loc[idx, "cilindradas"]) > 400:
        continue
    if abs(row["precioBase"] - df.loc[idx, "precioBase"]) > 2000:
        continue

    print(f"- {row['patente']} | {row['marca']} | {row['anio']} | ${row['precioBase']}")

print("\n===============================================\n")

