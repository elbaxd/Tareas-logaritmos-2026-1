"""Graficos de consultas: lecturas a disco y puntos encontrados.

Lee query_stats.csv (columnas: tree, s, reads_avg, reads_std, points_avg,
points_std) y produce dos PNGs en out_dir/:

    query_reads.png   -- s vs lecturas promedio (4 curvas, una por arbol).
    query_points.png  -- s vs puntos promedio con barras de error (std).

Uso:
    python plot_queries.py <query_stats.csv> <out_dir>
"""

import os
import sys

import matplotlib.pyplot as plt
import pandas as pd


def plot_reads(df: pd.DataFrame, out_path: str) -> None:
    """Dibuja s vs lecturas promedio con barras de error."""
    fig, ax = plt.subplots(figsize=(10, 6))
    for tree, group in df.groupby("tree"):
        group = group.sort_values("s")
        ax.errorbar(
            group["s"],
            group["reads_avg"],
            yerr=group["reads_std"],
            marker="o",
            capsize=4,
            label=tree,
        )
    ax.set_xlabel("Lado del cuadrado de consulta (s)")
    ax.set_ylabel("Lecturas a disco (promedio)")
    ax.set_title("Lecturas por consulta vs s")
    ax.grid(True, linestyle="--", alpha=0.4)
    ax.legend()
    fig.tight_layout()
    fig.savefig(out_path, dpi=150)
    print(f"Guardado: {out_path}")


def plot_points(df: pd.DataFrame, out_path: str) -> None:
    """Dibuja s vs puntos encontrados promedio con barras de error."""
    fig, ax = plt.subplots(figsize=(10, 6))
    for tree, group in df.groupby("tree"):
        group = group.sort_values("s")
        ax.errorbar(
            group["s"],
            group["points_avg"],
            yerr=group["points_std"],
            marker="o",
            capsize=4,
            label=tree,
        )
    ax.set_xlabel("Lado del cuadrado de consulta (s)")
    ax.set_ylabel("Puntos encontrados (promedio +/- std)")
    ax.set_title("Puntos encontrados por consulta vs s")
    ax.grid(True, linestyle="--", alpha=0.4)
    ax.legend()
    fig.tight_layout()
    fig.savefig(out_path, dpi=150)
    print(f"Guardado: {out_path}")


def main(csv_path: str, out_dir: str) -> None:
    """Carga el CSV y genera los dos graficos en out_dir."""
    df = pd.read_csv(csv_path)
    os.makedirs(out_dir, exist_ok=True)
    plot_reads(df, os.path.join(out_dir, "query_reads.png"))
    plot_points(df, os.path.join(out_dir, "query_points.png"))


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(__doc__)
        sys.exit(1)
    main(sys.argv[1], sys.argv[2])
