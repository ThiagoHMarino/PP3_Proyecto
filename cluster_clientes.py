import sqlite3
import pandas as pd
from sklearn.cluster import KMeans
from sklearn.preprocessing import StandardScaler
import matplotlib.pyplot as plt

def cluster_clientes(db_path):
    conn = sqlite3.connect(db_path)

    # Cargar tablas
    df_cliente = pd.read_sql_query("SELECT * FROM cliente", conn)
    df_contrato = pd.read_sql_query("SELECT * FROM contrato", conn)

    conn.close()

    # Merge por DNI
    df = df_contrato.merge(df_cliente, left_on="dni_cliente", right_on="dni")

    # Gasto total del contrato
    df["gasto_total"] = df["costo"] + df["cargo_extra"]

    # Datos para cluster
    X = df[["edad", "gasto_total", "tiempo_establecido"]]

    scaler = StandardScaler()
    X_scaled = scaler.fit_transform(X)

    # Clustering
    kmeans = KMeans(n_clusters=3, random_state=0)
    df["cluster"] = kmeans.fit_predict(X_scaled)

    print(df[["dni", "edad", "gasto_total", "tiempo_establecido", "cluster"]])

    # Gráfico
    plt.scatter(df["edad"], df["gasto_total"], c=df["cluster"])
    plt.xlabel("Edad")
    plt.ylabel("Gasto total")
    plt.title("Clusters de clientes")
    plt.show()