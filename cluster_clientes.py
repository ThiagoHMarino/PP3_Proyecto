import sqlite3
import pandas as pd
from sklearn.cluster import KMeans

DB = "mi_base.db"

print("\n===============================")
print("   CLUSTERING DE CLIENTES")
print("===============================\n")

# ---------------------------
# Cargar datos desde SQLite
# ---------------------------

con = sqlite3.connect(DB)

query = """
SELECT 
    c.dni,
    cli.nombre,
    cli.apellido,
    cli.edad,
    SUM(c.costo) AS total_costo,
    SUM(c.cargo_extra) AS total_extra
FROM contrato c
JOIN clientes cli ON cli.dni = c.dni_cliente
GROUP BY c.dni;
"""

df = pd.read_sql_query(query, con)
con.close()

# Si algún cliente no tiene valores → ponerlos en 0
df["total_costo"] = df["total_costo"].fillna(0)
df["total_extra"] = df["total_extra"].fillna(0)

# Datos numéricos para clustering
X = df[["total_costo", "total_extra"]]

k = 3
kmeans = KMeans(n_clusters=k, random_state=42)
df["cluster"] = kmeans.fit_predict(X)

# ---------------------------
# Mostrar resultados
# ---------------------------

for i in range(k):
    grupo = df[df["cluster"] == i]
    print(f"\n--- CLUSTER {i} ---")
    print(f"Clientes en este grupo: {len(grupo)}")
    print(f"Promedio costo total:  ${grupo['total_costo'].mean():.2f}")
    print(f"Promedio cargos extra: ${grupo['total_extra'].mean():.2f}")

    if grupo["total_extra"].mean() < 50:
        print("Interpretación: Clientes responsables ✔️")
    elif grupo["total_extra"].mean() < 200:
        print("Interpretación: Clientes con retrasos ocasionales ")
    else:
        print("Interpretación: Clientes problemáticos ")
