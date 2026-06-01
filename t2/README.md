# Tarea 2 - Arboles AVL y Splay Tree

## Requisitos

- `g++` (soporte C++17, GCC >= 9)
- `make`
- `python3`, `pip`, `venv` (solo para graficos)

## Compilar

```bash
make
```

## Ejecutar experimentos

### Experimentos base (escenarios A, B, C, D) + teoremas (Sequential Access, Working Set)

```bash
./bin/trees run --c 5 --lambda 0.01 --seed 42 --out-dir results/
```

### Incluir bonus (Traversal Conjecture)

```bash
./bin/trees run --c 5 --lambda 0.01 --seed 42 --out-dir results/ --bonus
```

### Parametros

- `--c`: constante C en {1, ..., 10} (default 5)
- `--lambda`: parametro de la distribucion exponencial [0.001, 0.05] (default 0.01)
- `--seed`: semilla RNG (default 42)
- `--out-dir`: directorio de salida (default `results/`)

### Archivos generados en `results/`

- `base_results.csv`: tiempos de los 4 escenarios base
- `base_per_search.csv`: tiempos por busqueda individual (ns)
- `seq_results.csv`: tiempos del Sequential Access Theorem
- `ws_results.csv`: tiempos del Working Set Theorem
- `traversal_results.csv` (si --bonus): tiempos de la Traversal Conjecture

## Graficos

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r plots/requirements.txt

python3 plots/plot_base.py
python3 plots/plot_seq.py
python3 plots/plot_ws.py
python3 plots/plot_traversal.py  # solo si hay bonus
```

## Estructura del codigo

- `src/node.hpp`: nodo BST compartido
- `src/avl.hpp`, `src/avl.cpp`: implementacion del arbol AVL
- `src/splay.hpp`, `src/splay.cpp`: implementacion del Splay Tree
- `src/dataset.hpp`, `src/dataset.cpp`: generacion de datos
- `src/experiment.hpp`, `src/experiment.cpp`: experimentos
- `src/main.cpp`: punto de entrada
