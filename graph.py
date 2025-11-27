# python3 graph.py <carpeta_con_los_csv>
# python3 graph.py results_fm

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

def plot(df, output_path, title):
    plt.figure(figsize=(9, 6))
    plt.yscale("log")
    
    for algorithm in ["SA", "FM"]:
        subset = df[df["type"] == algorithm]
        if subset.empty:
            continue

        x = subset["pattern_len"]
        y = subset["time_ns"]
        std = np.sqrt(subset["time_var"])

        # Límites de la banda
        y_low = y - std
        y_high = y + std

        # Línea central
        plt.plot(x, y, "o-", label=algorithm)

        # Banda de varianza
        plt.fill_between(x, y_low, y_high, alpha=0.2)

    plt.title(f"{title}: Desempeño de SA y FMIndex en búsqueda de diferentes largos de patrones", pad=15)
    plt.xlabel("Largo del patrón")
    plt.ylabel("Tiempo (ns)")
    plt.legend()
    plt.tight_layout()
    plt.savefig(output_path)
    plt.close()


def main(folder_path):
    files = [f for f in os.listdir(folder_path) if f.endswith(".csv")]

    if not files:
        print("No se encontraron archivos CSV en la carpeta.")
        return

    # Genera la carpeta "graficos"
    output_dir = os.path.join(os.getcwd(), "graficos")
    os.makedirs(output_dir, exist_ok=True)

    print(f"Guardando gráficos comparativos en: {output_dir}\n")

    for csv_file in files:
        full_path = os.path.join(folder_path, csv_file)
        print(f"Procesando: {csv_file}")

        df = pd.read_csv(full_path)
        base_name = os.path.splitext(csv_file)[0]

        output_file = os.path.join(output_dir, f"{base_name}_comparacion.png")
        plot(df, output_file, base_name)

    print("\nSe generaron todos los gráficos")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python3 plot_from_folder.py carpeta_con_csv")
        sys.exit(1)

    main(sys.argv[1])
