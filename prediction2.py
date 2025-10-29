import sqlite3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
from sklearn.cluster import KMeans
from sklearn.preprocessing import StandardScaler

# 1 Leer las bases desde SQLite
conn = sqlite3.connect("alquiler.db")
df_contrato = pd.read_sql_query("SELECT * FROM Contrato", conn)
df_vehiculo = pd.read_sql_query("SELECT * FROM Vehiculo", conn)
conn.close()

# 2 Unir temporalmente en memoria
df_merge = pd.merge(df_contrato, df_vehiculo, left_on='patente_vehiculo', right_on='patente')

# 3 Limpieza básica (evitar nulos)
df_merge = df_merge.dropna(subset=['anio', 'tiempo_establecido', 'precioBase', 'cilindradas'])

# 4 Regresión lineal: estimar tiempo según año
X = df_merge[['anio']]
y = df_merge['tiempo_establecido']

modelo = LinearRegression()
modelo.fit(X, y)

# Predicciones
df_merge['tiempo_estimado'] = modelo.predict(X)

# 5 K-Means: agrupar por características similares
features = df_merge[['anio', 'precioBase', 'cilindradas', 'tiempo_establecido']]
scaler = StandardScaler()
X_scaled = scaler.fit_transform(features)

kmeans = KMeans(n_clusters=3, random_state=42)
df_merge['cluster'] = kmeans.fit_predict(X_scaled)

# 6 Gráfico combinado: regresión + clusters
plt.figure(figsize=(8,6))
for c in range(3):
    grupo = df_merge[df_merge['cluster'] == c]
    plt.scatter(grupo['anio'], grupo['tiempo_establecido'], label=f'Cluster {c}')

# Línea de regresión
x_line = np.linspace(df_merge['anio'].min(), df_merge['anio'].max(), 100).reshape(-1,1)
y_line = modelo.predict(x_line)
plt.plot(x_line, y_line, color='black', linewidth=2, label='Regresión Lineal')

plt.title('K-Means + Regresión Lineal')
plt.xlabel('Año del vehículo')
plt.ylabel('Tiempo de alquiler (segundos)')
plt.legend()
plt.grid(True)
plt.show(block=False)
plt.pause(5)
plt.close()

print(df_merge[['patente', 'anio', 'tiempo_establecido', 'cluster']].head())