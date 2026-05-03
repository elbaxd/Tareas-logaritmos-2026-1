"""Genera el grafico de tiempos de construccion del R-tree.

Lee build_times.csv (columnas: dataset, method, N, time_ms) y dibuja una
linea por combinacion dataset/method en un unico grafico, con N en el eje X
(escala log base 2) y tiempo en el eje Y (escala log).

Uso:
    python plot_construction.py <build_times.csv> <out.png>
"""

import sys

import matplotlib.pyplot as plt
import pandas as pd


def main(csv_path: str, out_path: str) -> None:
    """Carga el CSV y genera el grafico.

    Parametros:
        csv_path: ruta al archivo build_times.csv.
        out_path: ruta donde guardar el PNG.
    """
    df = pd.read_csv(csv_path)
    fig, ax = plt.subplots(figsize=(10, 6))
    for (dataset, method), group in df.groupby(["dataset", "method"]):
        group = group.sort_values("N")
        ax.plot(
            group["N"],
            group["time_ms"],
            marker="o",
            label=f"{dataset} / {method}",
        )
    ax.set_xscale("log", base=2)
    ax.set_yscale("log")
    ax.set_xlabel("N (cantidad de puntos)")
    ax.set_ylabel("Tiempo de construccion (ms)")
    ax.set_title("Tiempo de construccion del R-tree vs N")
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    ax.legend()
    fig.tight_layout()
    fig.savefig(out_path, dpi=150)
    print(f"Guardado: {out_path}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(__doc__)
        sys.exit(1)
    main(sys.argv[1], sys.argv[2])
