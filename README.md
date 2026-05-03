# R-tree con bulk-loading (Nearest-X y STR)

Implementacion en C++17 de un R-tree para puntos 2D guardados en memoria
externa, con dos metodos de bulk-loading (Nearest-X y Sort-Tile-Recursive)
y consultas del tipo "puntos contenidos en un rectangulo R".

Tarea 1 del curso CC4102 (Diseno y Analisis de Algoritmos), DCC, U. de Chile,
semestre 2026-1.

## Estructura

```
crons/rt/
|-- README.md
|-- REPORT.md            -- Reporte (introduccion, desarrollo, resultados, ...)
|-- Makefile
|-- mision.md            -- Enunciado original
|-- src/
|   |-- point.hpp        -- Point, MBR y predicados geometricos
|   |-- rtree_node.hpp   -- Entry, Node, constantes (B=204, NODE_SIZE=4096)
|   |-- rtree_node.cpp
|   |-- bulk_loader.hpp  -- buildNearestX, buildSTR
|   |-- bulk_loader.cpp
|   |-- io.hpp           -- readPointsBin, writeTreeBin, readNode (con contador)
|   |-- io.cpp
|   |-- query.hpp        -- queryRange (lectura por bloque)
|   |-- query.cpp
|   |-- experiment.cpp   -- Subcomandos 'experiment' y 'bonus'
|   `-- main.cpp         -- CLI
`-- plots/
    |-- requirements.txt
    |-- plot_construction.py
    |-- plot_queries.py
    `-- plot_bonus.py
```

## Requisitos

- `g++` con soporte de C++17 (testeado con g++ 11+).
- `make`.
- Python 3.9+ con `matplotlib`, `numpy` y `pandas` (solo para los graficos).

## Compilacion

```bash
make
```

Produce el ejecutable `bin/rtree`. Flags por defecto:
`-O2 -std=c++17 -Wall -Wextra`. Para una build mas agresiva:

```bash
make CXXFLAGS="-O3 -std=c++17 -DNDEBUG -march=native"
```

Para limpiar:

```bash
make clean
```

## Datasets

Descargar `random.bin`, `europa.bin` y `europa_bonus.bin` desde
[claugaete/tarea1-cc4102-2026-1](https://github.com/claugaete/tarea1-cc4102-2026-1)
y dejarlos en `data/`.

Cada archivo es una secuencia de pares `(float x, float y)` (8 bytes por punto)
sin header. `random.bin` y `europa.bin` estan normalizados a `[0,1] x [0,1]`;
`europa_bonus.bin` contiene las coordenadas reales (longitud, latitud) en
aprox. `[-11, 35] x [35, 72]`.

## Comandos

### Construir un arbol

```bash
./bin/rtree build --method nearest-x --in data/random.bin --N 1048576 --out tree.bin
./bin/rtree build --method str       --in data/random.bin --N 1048576 --out tree.bin
```

`--N 0` (o sin `--N`) usa todos los puntos del archivo.

### Consulta puntual

```bash
./bin/rtree query --tree tree.bin --rect 0.1 0.2 0.4 0.5
```

Imprime los puntos encontrados a stdout. A stderr imprime la cantidad total
de lecturas a disco.

### Experimentacion completa

```bash
./bin/rtree experiment \
    --random data/random.bin \
    --europa data/europa.bin \
    --out-dir results/
```

Recorre `N in {2^15, ..., 2^24}` con ambos metodos y ambos datasets,
midiendo tiempos de construccion. Para `N = 2^24` guarda los 4 arboles
serializados a disco. Luego ejecuta 100 consultas para cada
`s in {0.0025, 0.005, 0.01, 0.025, 0.05}` sobre cada arbol y reporta
lecturas y puntos encontrados.

Genera:

- `results/build_times.csv` -- columnas: `dataset, method, N, time_ms`.
- `results/query_stats.csv` -- columnas: `tree, s, reads_avg, reads_std, points_avg, points_std`.
- `results/tree_random_nearest-x.bin`, `tree_random_str.bin`,
  `tree_europa_nearest-x.bin`, `tree_europa_str.bin`.

### Bonus

```bash
./bin/rtree bonus \
    --in data/europa_bonus.bin \
    --rect -3.9 -3.5 40.3 40.6 \
    --out results/bonus_madrid.csv
```

Construye un R-tree con STR sobre el dataset no normalizado y consulta el
rectangulo dado. Escribe los puntos encontrados a `results/bonus_madrid.csv`.

## Graficos

Crear un virtualenv e instalar dependencias:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r plots/requirements.txt
```

Generar:

```bash
python plots/plot_construction.py results/build_times.csv results/build_times.png
python plots/plot_queries.py      results/query_stats.csv  results/
python plots/plot_bonus.py        results/bonus_madrid.csv results/bonus_madrid.png \
    --label "Madrid" --center -3.7 40.4
```

## Detalles de implementacion

- Tamano del nodo: 4096 bytes exactos. Garantizado por `static_assert` en
  `rtree_node.hpp`.
- Durante la construccion el arbol vive en RAM como `std::vector<Node>`. La
  posicion 0 se reserva para la raiz y se llena al final de la recursion.
  Los nodos hijos se empujan en posiciones `>= 1`.
- Las consultas siempre leen el arbol desde el archivo binario
  (`std::ifstream` + `seekg`) y cuentan lecturas con `g_disk_reads`,
  incrementado dentro de `readNode`.
- Para ordenar entradas se usa `x1 + x2` (en vez de `(x1 + x2) / 2`), que es
  monotonicamente equivalente al centro y evita la division.
- STR usa `S = ceil(sqrt(ceil(n/B)))` slices.

## Reporte

`REPORT.md` contiene introduccion, desarrollo, resultados, analisis y
conclusion. Los graficos referenciados se producen al correr la
experimentacion y los scripts de Python.
