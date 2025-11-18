import sqlite3
import pandas as pd
from sklearn.neighbors import NearestNeighbors

def recomendar_vehiculo(db_path, dni_cliente_objetivo, k=3):
    conn = sqlite3.connect(db_path)

    df_cliente = pd.read_sql_query("SELECT * FROM cliente", conn)
    df_contrato = pd.read_sql_query("SELECT * FROM contrato", conn)
    df_vehiculos = pd.read_sql_query("SELECT * FROM vehiculos", conn)

    conn.close()

    # Merge global
    df = df_contrato.merge(df_cliente, left_on="dni_cliente", right_on="dni")

    df["gasto_total"] = df["costo"] + df["cargo_extra"]

    # Vector del cliente
    X = df[["edad", "gasto_total", "tiempo_establecido"]]

    neigh = NearestNeighbors(n_neighbors=k)
    neigh.fit(X)

    # Encontrar cliente objetivo
    cliente_ref = df[df["dni"] == dni_cliente_objetivo]

    if cliente_ref.empty:
        print("Cliente no encontrado")
        return

    vector_cliente = cliente_ref[["edad", "gasto_total", "tiempo_establecido"]].values

    # Vecinos más similares
    distancias, indices = neigh.kneighbors(vector_cliente)

    vecinos = df.iloc[indices[0]]

    # Vehículos usados por los vecinos
    recomendaciones = vecinos["patente_vehiculo"].value_counts().head(3).index.tolist()

    print("Vehículos recomendados:", recomendaciones)

    # Mostrar información de esos vehículos
    print(df_vehiculos[df_vehiculos["patente"].isin(recomendaciones)])