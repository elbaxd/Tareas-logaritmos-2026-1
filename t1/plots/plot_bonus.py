"""Scatterfield del bonus.

Lee bonus.csv (columnas: x, y) y genera un scatterplot. Permite anotar la
ubicacion elegida con --label "Nombre" --center lon lat.

Uso:
    python plot_bonus.py <bonus.csv> <out.png> [--label "Madrid"] \\
                        [--center -3.7 40.4]
"""

import argparse

import matplotlib.pyplot as plt
import pandas as pd


def main() -> None:
    """Parsea argumentos y genera el scatterplot."""
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("csv", help="CSV con columnas x, y")
    p.add_argument("out", help="ruta del PNG de salida")
    p.add_argument("--label", default="",
                   help="etiqueta para anotar la ubicacion central")
    p.add_argument("--center", nargs=2, type=float, default=None,
                   metavar=("LON", "LAT"),
                   help="coordenadas del centro a marcar con una estrella")
    args = p.parse_args()

    df = pd.read_csv(args.csv)
    fig, ax = plt.subplots(figsize=(8, 8))
    ax.scatter(df["x"], df["y"], s=2, alpha=0.5)
    ax.set_xlabel("Longitud")
    ax.set_ylabel("Latitud")
    title = f"Bonus: edificios encontrados ({len(df)} puntos)"
    if args.label:
        title += f" - {args.label}"
    ax.set_title(title)
    if args.center is not None:
        ax.scatter(
            *args.center,
            color="red",
            marker="*",
            s=200,
            label=args.label or "centro",
        )
        ax.legend()
    ax.set_aspect("equal", adjustable="datalim")
    ax.grid(True, linestyle="--", alpha=0.4)
    fig.tight_layout()
    fig.savefig(args.out, dpi=150)
    print(f"Guardado: {args.out}")


if __name__ == "__main__":
    main()
