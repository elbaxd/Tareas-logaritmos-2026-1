# Tarea 2 - Diseño y Análisis de Algoritmos

## Instrucciones de ejecución

## Requisitos

- `g++` (soporte C++17, se recomienda GCC >= 9)
- `make`
- `python3, pip, venv`

## 1. Obtener datasets

Descargar desde [tarea1-cc4102-2026-1/releases/tag/Datos](https://github.com/claugaete/tarea1-cc4102-2026-1/releases/tag/Datos) los archivos `random.bin`, `europa.bin` y `europa_bonus.bin` y dejarlos en `data/`.

## 2. Compilar

```bash
make
```

## 3. Entorno Python para gráficos

Esta parte es solo necesaria para generar los gráficos, el programa en `cpp` correrá sin necesidad de esto.

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r plots/requirements.txt
```

## 4. Verificación inicial

```bash
./bin/rtree build --method nearest-x --in data/random.bin --N 32768 --out /tmp/tree_small.bin
./bin/rtree query --tree /tmp/tree_small.bin --rect 0.0 0.01 0.0 0.01
```

Con `s=0.01` sobre el dataset aleatorio se deben obtener aproximadamente el 0.01% de los puntos (~3 para N=32768).

En nuestras pruebas retornó 2, que coincide con lo señalado.

## 5. Experimentación

```bash
./bin/rtree experiment --random data/random.bin --europa data/europa.bin --out-dir results/
```

Esto construye los arboles y corre todas las consultas, tomará varios minutos. Los arboles finales (`N=2^24`) pesan ~1.3 GB. Luego de correr, se generan en `results/`:

Esto genera:
 - `results/tree_random_nearest-x.bin`: Árbol de dataset `random` con _X-Nearest_
 - `results/tree_random_str.bin`: Árbol de dataset `random` con _STR_
 - `results/tree_europa_nearest-x.bin`: Árbol de dataset `europa` con _X-Nearest_
 - `results/tree_europa_str.bin`: Árbol de dataset `europa` con _STR_
 - `results/build_times.csv`: Archivo con estadísticas de tiempos de construccion de los árboles
 - `results/query_stats.csv`: Archivo de estadísticas de consultas, usado para generar los gráficos

## 6. Bonus

Para las pruebas escogimos con [Vienna, Austria](https://geohack.toolforge.org/geohack.php?pagename=Vienna&params=48.2083_N_16.3725_E_type:adm1st_region:AT-9), con coordenadas:
 - Latitud: 48.2083
 - Longitud: 16.3725

```bash
./bin/rtree bonus --in data/europa_bonus.bin --rect 16.1 16.5 48.0 48.3 --out results/bonus_vienna.csv
```

Esto generará:
 - `results/bonus_vienna.csv`: Archivo de estadísticas de consultas del bonus, usado para generar los gráficos

## 7. Generar graficos

1. Tiempos de construcción:

```bash
python plots/plot_construction.py results/build_times.csv results/build_times.png
```

Esto genera `results/build_times.png`.

2. Lecturas y puntos por consultas:

```bash
python plots/plot_queries.py results/query_stats.csv results/
```

Esto  genera `results/query_reads.png` y `results/query_points.png`.

3. Gráfico bonus:

```bash
python plots/plot_bonus.py results/bonus_madrid.csv results/bonus_vienna.png --label "Vienna" --center 16.3 48.2
```
