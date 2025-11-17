import pandas as pd
import os

CLIENTES = "bd_clientes.json"
VEHICULOS = "bd_vehiculos.json"
CONTRATOS = "bd_contratos.json"

print("\n===============================")
print(" SUGERENCIAS INTELIGENTES")
print("===============================\n")

if not all(os.path.exists(f) for f in [CLIENTES, VEHICULOS, CONTRATOS]):
    print("ERROR: Falta algún archivo JSON.")
    exit()

df_cli = pd.read_json(CLIENTES)
df_veh = pd.read_json(VEHICULOS)
df_con = pd.read_json(CONTRATOS)

# Merge para análisis cruzado
df = df_con.merge(df_cli, on="dni").merge(df_veh, on="patente")

print("1) RELACIÓN ENTRE EDAD Y MARCA ELEGIDA")
print("--------------------------------------")
marca_por_edad = df.groupby("marca")["edad"].mean().sort_values()
print(marca_por_edad)
print("\nInterpretación:")
print("- Marcas asociadas a edades menores → buena idea promocionar en redes.")
print("- Marcas asociadas a edades mayores → paquetes de fidelización.\n")

print("2) TIEMPOS ESTABLECIDOS MÁS POPULARES")
print("--------------------------------------")
populares = df["tiempo_establecido"].value_counts()
print(populares)

print("\n3) QUE TIPO GENERA MÁS INGRESOS: AUTO VS MOTO")
print("---------------------------------------------")
print(df.groupby("tipo")["costo_final"].sum())

print("\n4) RECOMENDACIÓN DE PRECIOS")
print("------------------------------")
rentabilidad = df_veh["ingreso_total"] - df_veh["precio_base"]
if rentabilidad.mean() < 0:
    print("→ En promedio, los vehículos están generando pérdida. Considerar aumentar precios.")
elif rentabilidad.mean() < 2000:
    print("→ Rentabilidad baja: revisar marcas con peor rendimiento.")
else:
    print("→ Buen rendimiento general. Mantener precios.")