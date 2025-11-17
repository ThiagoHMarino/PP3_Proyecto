import pandas as pd
import os

BASE = "bd_vehiculos.json"

print("\n===============================")
print(" ANALISIS DE RENTABILIDAD")
print("===============================\n")

if not os.path.exists(BASE):
    print("ERROR: No se encontró el archivo de vehículos.")
    exit()

df = pd.read_json(BASE)

# Rentabilidad = ingresos totales - precio base
if "ingreso_total" not in df.columns:
    df["ingreso_total"] = 0

df["rentabilidad"] = df["ingreso_total"] - df["precio_base"]

# Ordenamos de menor a mayor
df_sorted = df.sort_values(by="rentabilidad")

# Mostrar los 5 menos rentables
print("Vehículos MENOS rentables:")
print("---------------------------")
for i, row in df_sorted.head(5).iterrows():
    print(f"Patente: {row['patente']}  | Marca: {row['marca']}  | Año: {row['anio']}")
    print(f"Precio base:     ${row['precio_base']}")
    print(f"Ingreso total:   ${row['ingreso_total']}")
    print(f"Rentabilidad:    ${row['rentabilidad']}")
    print("→ Sugerencia: Considerar ajuste de precio o promoción.\n")

# Agrupación por marca
print("\nRentabilidad por marca:\n------------------------")
print(df.groupby("marca")["rentabilidad"].mean())

# Agrupación por año
print("\nRentabilidad por año:\n------------------------")
print(df.groupby("anio")["rentabilidad"].mean())