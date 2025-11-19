#!/usr/bin/env python3

import sqlite3
import pandas as pd
from sklearn.preprocessing import StandardScaler
from sklearn.neighbors import NearestNeighbors
import sys
import shutil

# ----- centrado -----
def center(text):
    width = shutil.get_terminal_size().columns
    return text.center(width)

# ----- abrir base -----
db = sys.argv[1] if len(sys.argv) > 1 else "alquiler.db"

conn = sqlite3.connect(db)
df = pd.read_sql_query("SELECT * FROM Vehiculo", conn)
conn.close()

print("\n" + center("========== RECOMENDADOR DE VEHÍCULOS ==========\n"))

patente = input(center("Ingrese la patente para recomendar similares: ")).strip().upper()


# ========================================================
# Caso 1: patente NO existe → sugerencias similares
# ========================================================
if patente not in df["patente"].values:

    def parecido(a, b):
        if len(a) != len(b):
            return False
        mismatches = sum(1 for x, y in zip(a, b) if x != y)
        return mismatches <= 2

    sugerencias = [p for p in df["patente"].values if parecido(patente, p)]

    if not sugerencias:
        print(center("\nLa patente no existe y no se encontraron parecidas.\n"))
        sys.exit()

    print(center("\nLa patente no existe. ¿Quizás quiso decir?:\n"))
    for s in sugerencias:
        print(center(" - " + s))
    print()
    sys.exit()


# ========================================================
# Caso 2: patente existe → KNN con filtros
# ========================================================
X = df[["cilindradas", "precioBase", "anio"]]

scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

nn = NearestNeighbors(n_neighbors=5)
nn.fit(X_scaled)

idx = df.index[df["patente"] == patente][0]

dist, ind = nn.kneighbors([X_scaled[idx]])

print(center("\nVehículo consultado:\n"))
print(center(str(df.iloc[idx])) + "\n")

print(center("Recomendaciones:\n"))

for i in ind[0][1:]:
    row = df.iloc[i]

    # Evitar cosas absurdas
    if abs(row["anio"] - df.loc[idx, "anio"]) > 20:
        continue
    if abs(row["cilindradas"] - df.loc[idx, "cilindradas"]) > 400:
        continue
    if abs(row["precioBase"] - df.loc[idx, "precioBase"]) > 2000:
        continue

    linea = f"- {row['patente']} | {row['marca']} | {row['anio']} | ${row['precioBase']}"
    print(center(linea))

print("\n" + center("===============================================\n"))

