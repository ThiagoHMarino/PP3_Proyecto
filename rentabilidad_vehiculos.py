#!/usr/bin/env python3

import sqlite3
import pandas as pd
from sklearn.linear_model import LinearRegression
import numpy as np
import matplotlib.pyplot as plt
import sys
import shutil

# Para centrar texto según el tamaño de la terminal
cols = shutil.get_terminal_size().columns
def center(t):
    return str(t).center(cols)

db = sys.argv[1] if len(sys.argv) > 1 else "alquiler.db"

conn = sqlite3.connect(db)
df_contrato = pd.read_sql_query("SELECT * FROM contrato", conn)
df_veh = pd.read_sql_query("SELECT * FROM Vehiculo", conn)
conn.close()

print("\n" + center("========== RENTABILIDAD Y REGRESIÓN LINEAL ==========\n"))

# ---------------------------------------------
# NUEVO: Filtrar solo contratos terminados
# ---------------------------------------------
# Regla: contrato terminado = activo = 0
df_contrato = df_contrato[df_contrato["activo"] == 0]

# Calcular rentabilidad REAL
df_contrato["rentabilidad"] = df_contrato["costo"] + df_contrato["cargo_extra"]

# Agrupar por patente
df_r = df_contrato.groupby("patente_vehiculo")["rentabilidad"].sum().reset_index()

# Unir con tabla vehículos
df = df_r.merge(df_veh, left_on="patente_vehiculo", right_on="patente")

# Si no queda nada para analizar
if df.empty:
    print(center("No hay contratos finalizados para calcular rentabilidad."))
    sys.exit(0)

# Variables del modelo
X = df[["anio"]]
y = df["rentabilidad"]

modelo = LinearRegression()
modelo.fit(X, y)

print(center("Datos utilizados:\n"))
print(df[["patente", "anio", "rentabilidad"]], "\n")

print(center(f"Coeficiente (pendiente): {modelo.coef_[0]}"))
print(center(f"Intercepto: {modelo.intercept_}"))

if modelo.coef_[0] > 0:
    print("\n" + center("Interpretación: Vehículos más nuevos generan mayor rentabilidad.") + "\n")
else:
    print("\n" + center("Interpretación: Vehículos más viejos generan mayor rentabilidad.") + "\n")

# Gráfico
años = np.linspace(df["anio"].min(), df["anio"].max(), 30).reshape(-1, 1)
pred = modelo.predict(años)

plt.scatter(df["anio"], df["rentabilidad"])
plt.plot(años, pred)
plt.xlabel("Año del vehículo")
plt.ylabel("Rentabilidad total")
plt.title("Regresión lineal de rentabilidad vs año")
plt.grid(True)
plt.show()

