# python3 graph.py <carpeta_con_los_csv>
# python3 graph.py results_fm

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os


def plot_time(df, output_path, title):
    plt.figure(figsize=(9, 6))
    plt.yscale("log")

    # Colores consistentes
    colors = {"SA": "tab:blue", "FM": "tab:orange"}

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

        plt.plot(x, y, "o-", label=f"{algorithm} (Tiempo)", color=colors.get(algorithm))
        plt.fill_between(x, y_low, y_high, alpha=0.2, color=colors.get(algorithm))

    plt.title(f"{title}: Desempeño de SA y FMIndex en búsqueda de diferentes largos de patrones", pad=15)
    plt.xlabel("Largo del patrón")
    plt.ylabel("Tiempo (ns) [Log]")
    plt.legend()
    plt.tight_layout()
    plt.savefig(output_path)
    plt.close()


def plot_sizes_comparison(size_data, output_dir):
    if not size_data:
        return

    df_sizes = pd.DataFrame(size_data)

    plt.figure(figsize=(9, 6))

    # Posiciones para las barras
    x = np.arange(len(df_sizes))
    width = 0.35

    # Crear barras
    rects1 = plt.bar(x - width / 2, df_sizes["SA_MB"], width, label="Suffix Array", color="tab:blue")
    rects2 = plt.bar(x + width / 2, df_sizes["FM_MB"], width, label="FM-Index", color="tab:orange")

    # Etiquetas y títulos
    plt.xlabel("Dataset / Texto", fontsize=12)
    plt.ylabel("Tamaño en Memoria (MB)", fontsize=12)
    plt.title("Comparación de Uso de Memoria: SA vs FM", fontsize=14)
    plt.xticks(x, df_sizes["Dataset"].tolist(), rotation=45, ha="right")
    plt.legend()

    # Añadir etiquetas de valor encima de las barras
    def autolabel(rects):
        for rect in rects:
            height = rect.get_height()
            plt.annotate(
                f"{height:.1f}",
                xy=(rect.get_x() + rect.get_width() / 2, height),
                xytext=(0, 3),  # 3 points vertical offset
                textcoords="offset points",
                ha="center",
                va="bottom",
                fontsize=9,
            )

    autolabel(rects1)
    autolabel(rects2)

    plt.grid(axis="y", linestyle="--", alpha=0.5)
    plt.tight_layout()

    output_path = os.path.join(output_dir, "comparison_sizes.png")
    plt.savefig(output_path)
    plt.close()
    print(f"Gráfico de tamaños guardado en: {output_path}")


def main(folder_path):
    if not os.path.exists(folder_path):
        print(f"Error: La carpeta '{folder_path}' no existe.")
        return

    files = [f for f in os.listdir(folder_path) if f.endswith(".csv")]

    if not files:
        print("No se encontraron archivos CSV en la carpeta.")
        return

    # Genera la carpeta "gráficos"
    output_dir = os.path.join(os.getcwd(), "graficos")
    os.makedirs(output_dir, exist_ok=True)
    print(f"Guardando gráficos en: {output_dir}\n")

    # Lista para guardar datos de tamaño
    all_sizes = []

    for csv_file in files:
        full_path = os.path.join(folder_path, csv_file)
        print(f"Procesando: {csv_file}")

        df = pd.read_csv(full_path)
        base_name = os.path.splitext(csv_file)[0]

        # Graficar tiempo vs largo de patron
        output_file_time = os.path.join(output_dir, f"{base_name}_time.png")
        plot_time(df, output_file_time, base_name)

        # Extraer tamaños (MB) para el gráfico comparativo final
        sa_subset = df[df["type"] == "SA"]
        fm_subset = df[df["type"] == "FM"]

        size_sa = sa_subset["size_mb"].iloc[0] if not sa_subset.empty else 0
        size_fm = fm_subset["size_mb"].iloc[0] if not fm_subset.empty else 0

        all_sizes.append({"Dataset": base_name, "SA_MB": size_sa, "FM_MB": size_fm})

    # Generar gráfico comparativo de tamaños (con todos los datasets)
    plot_sizes_comparison(all_sizes, output_dir)

    print("\nProceso finalizado.")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python3 graph.py <carpeta_con_csv>")
        sys.exit(1)

    main(sys.argv[1])

