import pandas as pd
from sklearn.cluster import KMeans
import json
import os

# Ruta del JSON (el que genera tu C++)
BASE = "bd_clientes.json"

print("\n===============================")
print("   CLUSTERING DE CLIENTES")
print("===============================\n")

if not os.path.exists(BASE):
    print("ERROR: No se encontró el archivo de clientes.")
    exit()

df = pd.read_json(BASE)

# Si algún cliente nunca alquiló, se completa con 0
df["cargos_extra"] = df["cargos_extra"].fillna(0)
df["ingreso_total"] = df["ingreso_total"].fillna(0)

# Datos para clustering
X = df[["cargos_extra", "ingreso_total"]]

# Número de clusters
k = 3
kmeans = KMeans(n_clusters=k, random_state=42)
df["cluster"] = kmeans.fit_predict(X)

# Mostramos resultados claros
for i in range(k):
    grupo = df[df["cluster"] == i]
    print(f"--- CLUSTER {i} ---")
    print(f"Clientes en el grupo: {len(grupo)}")
    print(f"Promedio cargos extra:  ${grupo['cargos_extra'].mean():.2f}")
    print(f"Promedio ingreso total: ${grupo['ingreso_total'].mean():.2f}")
    
    # Interpretación simple
    if grupo["cargos_extra"].mean() < 50:
        print("Interpretación: Clientes responsables y confiables.")
    elif grupo["cargos_extra"].mean() < 200:
        print("Interpretación: Clientes con retrasos ocasionales.")
    else:
        print("Interpretación: Clientes problemáticos o poco fiables.")
    print()
