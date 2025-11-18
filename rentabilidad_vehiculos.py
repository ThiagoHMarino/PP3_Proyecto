import sqlite3
import pandas as pd
from sklearn.linear_model import LinearRegression
import matplotlib.pyplot as plt
import numpy as np

def rentabilidad_vehiculos(db_path):
    conn = sqlite3.connect(db_path)

    df_contrato = pd.read_sql_query("SELECT * FROM contrato", conn)
    df_vehiculo = pd.read_sql_query("SELECT * FROM vehiculos", conn)

    conn.close()

    # Ingresos por cada contrato
    df_contrato["ingreso_total"] = df_contrato["costo"] + df_contrato["cargo_extra"]

    # Merge vehículo ↔ contrato
    df = df_contrato.merge(df_vehiculo, left_on="patente_vehiculo", right_on="patente")

    # Rentabilidad real
    df["rentabilidad"] = df["ingreso_total"] - df["precioBase"]

    # Modelo: Año → Rentabilidad
    X = df[["anio"]]
    y = df["rentabilidad"]

    model = LinearRegression()
    model.fit(X, y)

    # Predicción para gráfico (línea)
    X_line = np.linspace(df["anio"].min(), df["anio"].max(), 100).reshape(-1, 1)
    y_line = model.predict(X_line)

    plt.scatter(df["anio"], df["rentabilidad"])
    plt.plot(X_line, y_line)
    plt.xlabel("Año del vehículo")
    plt.ylabel("Rentabilidad")
    plt.title("Regresión lineal de rentabilidad vs año")
    plt.show()

    # Vehículos menos rentables
    menos_rentables = df.sort_values("rentabilidad").head()
    print("Vehículos menos rentables:")
    print(menos_rentables[["patente", "anio", "rentabilidad"]])