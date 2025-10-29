#KMEANS - ALgo

#!/usr/bin/env python3

# ----------------------Librerias y modulos ----------------------

import pandas as pd
import sqlite3
from sklearn.cluster import KMeans
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import accuracy_score
import matplotlib.pyplot as plt
import numpy as np

#---------------------- Conexión a la base de datos ----------------------

conn = sqlite3.connect("alquiler.db")
df_cliente = pd.read_sql_query("SELECT * FROM Cliente", conn)
df_contrato = pd.read_sql_query("SELECT * FROM Contrato", conn)
df_vehiculo = pd.read_sql_query("SELECT * FROM Vehiculo", conn)

conn.close() #Cerrar conexión

#---------------------- Impresion ----------------------

print("Datos leídos de la base:\n")
print(df_cliente.head(), "\n")
print(df_contrato.head(), "\n")
print(df_vehiculo.head(), "\n")

# --------------------- Preparación de datos ----------------------
#Vamos a trbajar con el dataframe df_contrato

# Unimos solo para tener el año
df_merge = pd.merge(df_contrato, df_vehiculo, left_on='patente_vehiculo', right_on='patente')

# 3. Seleccionar solo las columnas que interesan
df = df_merge[['anio', 'tiempo_establecido']].dropna()

# Convertir a tipo numérico
df['anio'] = pd.to_numeric(df['anio'], errors='coerce')
df['tiempo_establecido'] = pd.to_numeric(df['tiempo_establecido'], errors='coerce')

# Eliminar filas con valores faltantes
df = df.dropna()

# 4. Crear el modelo de regresión lineal
X = df[['tiempo_establecido']]  # variable independiente
y = df['anio']                  # variable dependiente

modelo = LinearRegression()
modelo.fit(X, y)

# 5. Pedir un tiempo estimado y margen
tiempo_objetivo = float(input("Ingrese el tiempo estimado de alquiler: "))
margen = float(input("Ingrese el margen de años aceptable: "))

# 6. Calcular el año estimado
anio_estimado = modelo.predict([[tiempo_objetivo]])[0]

print(f"\nAño estimado de vehículo más probable: {int(anio_estimado)}")

# 7. Filtrar vehículos dentro del margen
min_anio = anio_estimado - margen
max_anio = anio_estimado + margen

vehiculos_filtrados = df_vehiculo[
    (df_vehiculo['anio'] >= min_anio) & (df_vehiculo['anio'] <= max_anio)
]

# 8. Mostrar resultados
print(f"\nVehículos dentro del rango {min_anio:.0f} - {max_anio:.0f}:")
print(vehiculos_filtrados[['patente', 'marca', 'anio', 'precioBase']])

plt.figure(figsize=(8, 6))
plt.scatter(X, y, color='blue', label='Datos reales')
plt.plot(X, modelo.predict(X), color='red', linewidth=2, label='Regresión lineal')
plt.axhline(y=anio_estimado, color='green', linestyle='--', label=f'Año estimado: {anio_estimado:.1f}')
plt.title("Relación entre tiempo de alquiler y año del vehículo")
plt.xlabel("Tiempo de alquiler")
plt.ylabel("Año del vehículo")
plt.legend()
plt.grid(True)
plt.show()

