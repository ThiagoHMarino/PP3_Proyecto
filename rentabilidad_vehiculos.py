#!/usr/bin/env python3

import sqlite3
import pandas as pd
from sklearn.linear_model import LinearRegression
import numpy as np
import matplotlib.pyplot as plt
import sys

db = sys.argv[1] if len(sys.argv) > 1 else "alquiler.db"

conn = sqlite3.connect(db)
df_contrato = pd.read_sql_query("SELECT * FROM contrato", conn)
df_veh = pd.read_sql_query("SELECT * FROM Vehiculo", conn)
conn.close()

print("\n========== RENTABILIDAD Y REGRESIÓN LINEAL ==========\n")

df_contrato["rentabilidad"] = df_contrato["costo"] + df_contrato["cargo_extra"]

df_r = df_contrato.groupby("patente_vehiculo")["rentabilidad"].sum().reset_index()
df = df_r.merge(df_veh, left_on="patente_vehiculo", right_on="patente")

X = df[["anio"]]
y = df["rentabilidad"]

modelo = LinearRegression()
modelo.fit(X, y)

print("Datos utilizados:")
print(df[["patente", "anio", "rentabilidad"]], "\n")

print("Coeficiente:", modelo.coef_[0])
print("Intercepto:", modelo.intercept_)

if modelo.coef_[0] > 0:
    print("\nInterpretación: Vehículos más nuevos generan mayor rentabilidad.\n")
else:
    print("\nInterpretación: Vehículos viejos generan mayor rentabilidad.\n")

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
