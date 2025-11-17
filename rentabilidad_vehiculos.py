import sqlite3
import pandas as pd
from sklearn.linear_model import LinearRegression
import matplotlib.pyplot as plt
import numpy as np

DB = "mi_base.db"

print("\n=============================================")
print(" ANALISIS DE RENTABILIDAD + REGRESIÓN LINEAL")
print("=============================================\n")

# ================================
# 1. Cargar datos de SQLite
# ================================
con = sqlite3.connect(DB)

query = """
SELECT 
    v.patente,
    v.anio,
    v.precioBase,
    SUM(c.costo + c.cargo_extra) AS ingreso_total
FROM vehiculos v
LEFT JOIN contrato c ON c.patente = v.patente
GROUP BY v.patente;
"""

df = pd.read_sql_query(query, con)
con.close()

df["ingreso_total"] = df["ingreso_total"].fillna(0)
df["rentabilidad"] = df["ingreso_total"] - df["precioBase"]

print(df)

# ================================
# 2. Preparar datos para regresión
# ================================
X = df[["anio"]]
y = df["rentabilidad"]

modelo = LinearRegression()
modelo.fit(X, y)

# ================================
# 3. Mostrar coeficientes
# ================================
print("\nModelo entrenado:")
print("-----------------")
print(f"Coeficiente (pendiente): {modelo.coef_[0]:.2f}")
print(f"Intersección: {modelo.intercept_:.2f}")

def predecir(anio):
    pred = modelo.predict([[anio]])[0]
    print(f"\nRentabilidad estimada para año {anio}: ${pred:.2f}")

# Ejemplo
predecir(2018)

# ================================
# 4. Graficar datos reales + recta
# ================================
plt.figure(figsize=(8, 5))

# Datos reales
plt.scatter(df["anio"], df["rentabilidad"])

# Recta de regresión
x_line = np.linspace(df["anio"].min(), df["anio"].max(), 100).reshape(-1, 1)
y_line = modelo.predict(x_line)
plt.plot(x_line, y_line)

plt.title("Rentabilidad vs Año (con regresión lineal)")
plt.xlabel("Año del vehículo")
plt.ylabel("Rentabilidad ($)")
plt.grid(True)

plt.show()
