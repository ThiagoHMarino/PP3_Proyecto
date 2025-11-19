import sqlite3
import pandas as pd
from sklearn.ensemble import IsolationForest
from shutil import get_terminal_size

def center(text):
    columns = get_terminal_size((80, 20)).columns
    return text.center(columns)

# ==============================
#   DETECCIÓN DE ANOMALÍAS
# ==============================
def detectar_anomalias():
    conn = sqlite3.connect("rentacar.db")

    # Cargar datos de alquileres (ejemplo adaptable)
    df = pd.read_sql_query("""
        SELECT 
            a.DNI,
            c.Nombre,
            c.Apellido,
            a.CostoTotal,
            a.DiasAlquiler
        FROM Alquileres a
        JOIN Clientes c ON a.DNI = c.DNI
    """, conn)

    conn.close()

    if df.empty:
        print(center("No hay datos de alquileres para analizar."))
        return

    # Calcular costo por día
    df["CostoPorDia"] = df["CostoTotal"] / df["DiasAlquiler"]

    # Modelo Isolation Forest
    modelo = IsolationForest(contamination=0.20, random_state=42)
    df["anomalia"] = modelo.fit_predict(df[["CostoPorDia"]])

    # -1 → anómalo, 1 → normal
    anomalos = df[df["anomalia"] == -1]
    normales = df[df["anomalia"] == 1]

    print("\n")
    print(center("=== DETECCIÓN DE ANOMALÍAS EN ALQUILERES ==="))
    print("\n")

    # Mostrar normales
    print(center("--- Registros Normales ---"))
    for _, row in normales.iterrows():
        linea = f"{row['Nombre']} {row['Apellido']} | ${row['CostoTotal']} | {row['DiasAlquiler']} días | ${row['CostoPorDia']:.2f}/día"
        print(center(linea))
    print("\n")

    # Mostrar anómalos
    print(center("--- REGISTROS SOSPECHOSOS (ANOMALÍAS) ---"))
    if anomalos.empty:
        print(center("No se detectaron anomalías."))
    else:
        for _, row in anomalos.iterrows():
            linea = f"⚠ {row['Nombre']} {row['Apellido']} | ${row['CostoTotal']} | {row['DiasAlquiler']} días | ${row['CostoPorDia']:.2f}/día"
            print(center(linea))

if __name__ == "__main__":
    detectar_anomalias()
