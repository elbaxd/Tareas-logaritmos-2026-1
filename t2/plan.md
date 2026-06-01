# Plan de Implementación: AVL + Splay Tree

## Resumen

Implementar en C++17 dos árboles binarios de búsqueda autobalanceables (AVL y
Splay Tree) con sus operaciones fundamentales (`insert`, `search`, rotaciones),
generar datasets, ejecutar experimentos base y de verificación de teoremas
(Sequential Access, Working Set, Traversal Conjecture), y producir gráficos con
Python 3.

---

## 1. Estructura de directorios

```
t2/
├── src/
│   ├── node.hpp
│   ├── avl.hpp
│   ├── avl.cpp
│   ├── splay.hpp
│   ├── splay.cpp
│   ├── dataset.hpp
│   ├── dataset.cpp
│   ├── experiment.hpp
│   ├── experiment.cpp
│   └── main.cpp
├── plots/
│   ├── requirements.txt
│   ├── plot_base.py
│   ├── plot_seq.py
│   ├── plot_ws.py
│   └── plot_traversal.py
├── build/          (compile-time)
├── bin/            (compile-time)
├── results/        (runtime)
├── Makefile
├── README.md
├── REPORT.md
└── next-steps.md
```

---

## 2. Especificación archivo por archivo

### 2.1 `src/node.hpp` — Nodo BST compartido

```cpp
#pragma once

#include <cstdint>
#include <algorithm>

struct Node {
    uint32_t key;
    Node* left;
    Node* right;
    int height;  // solo usado por AVL; Splay lo ignora
};

inline int getHeight(Node* n) {
    return n ? n->height : -1;
}

inline void updateHeight(Node* n) {
    n->height = std::max(getHeight(n->left), getHeight(n->right)) + 1;
}

inline int bf(Node* n) {
    return getHeight(n->left) - getHeight(n->right);
}
```

---

### 2.2 `src/splay.hpp` — Declaración SplayTree

```cpp
#pragma once

#include "node.hpp"
#include <cstdint>

class SplayTree {
public:
    SplayTree() : root(nullptr) {}
    ~SplayTree() { clear(root); }

    void insert(uint32_t key);
    bool search(uint32_t key);
    void clear();
    Node* getRoot() const { return root; }

private:
    Node* root;

    static Node* rotateZig(Node* y);     // derecha
    static Node* rotateZag(Node* y);     // izquierda
    Node* splay(Node* node, uint32_t key);  // recursivo, modifica root de subárbol

    void clear(Node* n);
};
```

### 2.3 `src/splay.cpp` — Implementación SplayTree

**Rotaciones** (estáticas, todas retornan la nueva raíz del subárbol):

```
rotateZig(y):    y(x(A,B), C) → x(A, y(B,C))
rotateZag(y):    y(A, x(B,C)) → x(y(A,B), C)
```

**Observación**: AVL solo necesita `rotateZig` y `rotateZag`. Las rotaciones
dobles de Splay (Zig-Zig, Zig-Zag, Zag-Zig, Zag-Zag) se implementan dentro del
propio `splay`.

**Algoritmo `splay(root, key)`** (recursivo, retorna nueva raíz):

```
function splay(node, key):
    si node == nullptr o node->key == key: retornar node

    si key < node->key:
        si node->left == nullptr: retornar node
        si key < node->left->key:
            // Zig-Zig
            node->left->left = splay(node->left->left, key)
            node = rotateZig(rotateZig(node))
        si no, si key > node->left->key:
            // Zig-Zag
            node->left->right = splay(node->left->right, key)
            si node->left->right != nullptr:
                node->left = rotateZag(node->left)
            node = rotateZig(node)
        si no:  // key == node->left->key
            // Zig
            node = rotateZig(node)
        retornar node

    si key > node->key:
        simétrico al caso anterior (Zag-Zig, Zag-Zag, Zag)

Implementar exactamente el patrón descrito en la sección 6.3 de mision.md:
- Rotaciones dobles consecutivas (0 o más) + una posible rotación simple final.
- Las rotaciones simples (Zig, Zag) solo se hacen cuando `y` es la raíz
  (i.e., el nodo a splay es hijo directo de la raíz).
```

**`insert(key)`**:
1. Insertar como en ABB estándar (recursivo o iterativo)
2. `root = splay(root, key)`

**`search(key)`**:
1. Buscar como en ABB (se puede hacer con el mismo `splay`)
2. `root = splay(root, key)`
3. Si se encuentra: `root->key == key`
4. Si no se encuentra: ya se hizo splay del último visitado

Implementar search como: `splay(root, key)` y luego verificar `root && root->key == key`.

---

### 2.4 `src/avl.hpp` — Declaración AVL

```cpp
#pragma once

#include "node.hpp"
#include <cstdint>

class AVL {
public:
    AVL() : root(nullptr) {}
    ~AVL() { clear(root); }

    void insert(uint32_t key);
    bool search(uint32_t key) const;
    void clear();
    Node* getRoot() const { return root; }

private:
    Node* root;

    Node* insertRec(Node* node, uint32_t key);
    bool searchRec(Node* node, uint32_t key) const;

    static Node* rotateZig(Node* y);    // LL
    static Node* rotateZag(Node* y);    // RR
    Node* rotateLR(Node* z);            // Zig-Zag
    Node* rotateRL(Node* z);            // Zag-Zig

    Node* balance(Node* node);
    void clear(Node* n);
};
```

### 2.5 `src/avl.cpp` — Implementación AVL

**Rotaciones**: `rotateZig` y `rotateZag` son idénticas a las de Splay.

**`rotateLR(z)`**: `z(y(A, x(B,C)), D)`:
```
z->left = rotateZag(z->left);
return rotateZig(z);
```

**`rotateRL(z)`**: `z(A, y(x(B,C), D))`:
```
z->right = rotateZig(z->right);
return rotateZag(z);
```

**`insertRec(node, key)`:**

```
function insertRec(node, key):
    si node == nullptr:
        retornar new Node{key, nullptr, nullptr, 0}

    si key < node->key:
        node->left = insertRec(node->left, key)
    si no, si key > node->key:
        node->right = insertRec(node->right, key)
    si no:
        retornar node  // clave duplicada, no insertar

    updateHeight(node)
    retornar balance(node)
```

**`balance(node)`**:

```
function balance(node):
    factor = bf(node)

    si factor > 1:
        si bf(node->left) >= 0:
            retornar rotateZig(node)   // LL
        si no:
            retornar rotateLR(node)    // LR
    si factor < -1:
        si bf(node->right) <= 0:
            retornar rotateZag(node)   // RR
        si no:
            retornar rotateRL(node)    // RL

    retornar node
```

**`search(node, key)`**: búsqueda binaria recursiva o iterativa, sin
modificación de la estructura.

---

### 2.6 `src/dataset.hpp` / `src/dataset.cpp` — Generación de datasets

```cpp
#pragma once

#include <cstdint>
#include <vector>

std::vector<uint32_t> generateUniform(uint32_t n, uint64_t seed = 42);
std::vector<double> exponentialPMF(uint32_t n, double lambda);
```

**`generateUniform(n, seed)`**:
```
mt19937 rng(seed)
uniform_int_distribution<uint32_t> dist(0, UINT32_MAX)
vector<uint32_t> data(n)
for i in 0..n-1: data[i] = dist(rng)
return data
```

**`exponentialPMF(n, lambda)`**:
```
numerador: vector<double> P(n)
for i in 0..n-1:
    P[i] = exp(-lambda * i) * (1 - exp(-lambda))
denominador: sum = 1 - exp(-lambda * n)
for i in 0..n-1: P[i] /= sum
return P
```

Se necesita también una función para samplear:
```cpp
// En experiment.cpp se usa std::discrete_distribution sobre el PMF
std::discrete_distribution<int> dist(pmf.begin(), pmf.end());
int idx = dist(rng);
```

---

### 2.7 `src/experiment.hpp` / `src/experiment.cpp` — Experimentos

```cpp
#pragma once

#include <string>

void runBaseScenarios(const std::string& outDir, int C, double lambda, uint64_t seed);
void runSequentialAccess(const std::string& outDir, uint64_t seed);
void runWorkingSet(const std::string& outDir, int C, uint64_t seed);
void runTraversal(const std::string& outDir, uint64_t seed);
```

**Constantes en el código** (puestas como variables, documentadas en REPORT.md):
- `C`: entero en {1..10} a elección (default 5)
- `LAMBDA`: double en [0.001, 0.05] (default 0.01)
- `SEED`: uint64_t (default 42)

**Helper de timing**:
```cpp
#include <chrono>
using Clock = std::chrono::steady_clock;

double elapsedMs(Clock::time_point start, Clock::time_point end) {
    return std::chrono::duration<double, std::milli>(end - start).count();
}
```

#### Escenarios Base

```
BASE_N = {1024, 2048, 4096, 8192, 16384}   // = 2^10 .. 2^14
CONFIGS: A, B, C, D

for N in BASE_N:
    dataset = generateUniform(N)
    sorted_dataset = sort(copy(dataset))

    cmf = exponentialPMF(N, LAMBDA)  // solo para configs B, D

    for config in [A, B, C, D]:
        data_for_insert = (config == C o D) ? sorted_dataset : dataset

        // --- AVL ---
        avl = AVL()
        t0 = Clock::now()
        for key in data_for_insert: avl.insert(key)
        t1 = Clock::now()
        avl_insert_ms = elapsedMs(t0, t1)

        // --- Splay ---
        splay = SplayTree()
        t0 = Clock::now()
        for key in data_for_insert: splay.insert(key)
        t1 = Clock::now()
        splay_insert_ms = elapsedMs(t0, t1)

        // --- Búsquedas ---
        M = 10 * C * N
        if (config == A o C):  // uniform search
            keys = generateUniform(N, seed+1)  // fresh search keys
            // Repetir ciclicamente el vector de keys hasta M
        else:  // skewed search (B o D)
            // Usar cmf con discrete_distribution para samplear M keys

        // Buscar en AVL
        t0 = Clock::now()
        for i in 0..M-1: avl.search(search_keys[i % search_keys.size()])
        t1 = Clock::now()
        avl_search_ms = elapsedMs(t0, t1)

        // Buscar en Splay
        t0 = Clock::now()
        for i in 0..M-1: splay.search(search_keys[i % search_keys.size()])
        t1 = Clock::now()
        splay_search_ms = elapsedMs(t0, t1)

        avl.clear()
        splay.clear()

        // Escribir fila CSV
        append to base_results.csv: config,N,avl_insert_ms,avl_search_ms,
                                     splay_insert_ms,splay_search_ms
```

**Detalle de generación de search_keys**:

```
Para búsqueda uniforme (A, C):
    search_keys = generateUniform(N, seed + config_offset)  // N keys aleatorias
Para búsqueda sesgada (B, D):
    pmf = exponentialPMF(N, LAMBDA)
    discrete_distribution dist(pmf.begin(), pmf.end())
    search_keys = vector<uint32_t>(N)  // N keys, cada una = sorted_dataset[dist(rng)]
```

Luego, para alcanzar las `M` búsquedas, se recorre `search_keys` cíclicamente
(`search_keys[i % N]`).

**CSV**: `base_results.csv`

```csv
config,N,avl_insert_ms,avl_search_ms,splay_insert_ms,splay_search_ms
A,1024,0.234,1.567,0.345,0.890
...
```

#### Sequential Access Theorem

```
N = 2^25 = 33554432
dataset = generateUniform(N)
sort(dataset)

// Construir árboles
avl = AVL(); splay = SplayTree()
for key in dataset:
    avl.insert(key)
    splay.insert(key)

// Para m en {N/100, 2N/100, ..., 10N/100}
m_values = [N/100, 2N/100, ..., 10N/100]

for m in m_values:
    // Tomar los primeros m elementos del dataset ordenado (son crecientes)
    seq = dataset[0..m-1]

    // AVL
    t0 = Clock::now()
    for key in seq: avl.search(key)
    t1 = Clock::now()

    // Splay (reutilizar, splay modifica estructura y eso es correcto)
    t2 = Clock::now()
    for key in seq: splay.search(key)
    t3 = Clock::now()

    Escribir fila en seq_results.csv

avl.clear(); splay.clear()
```

**CSV**: `seq_results.csv`

```csv
m,avl_search_ms,splay_search_ms
335544,...
671088,...
...
```

#### Working Set Theorem

```
N = 2^25
dataset = generateUniform(N)
sort(dataset)

// Construir árboles (igual que sequential access)
avl = AVL(); splay = SplayTree()
for key in dataset: avl.insert(key); splay.insert(key)

M = 10 * C * N

W_values = {10, 100, 1000, 10000, 100000, 1000000}

for W in W_values:
    // Seleccionar W elementos aleatorios del dataset (working set)
    mt19937 rng(seed + W)
    shuffle(index 0..N-1)
    working_set = primeros W elementos del shuffle

    // Generar M búsquedas uniformes dentro del working set
    uniform_int_distribution pick(0, W-1)
    search_keys = vector<uint32_t>(M)
    for i in 0..M-1: search_keys[i] = working_set[pick(rng)]

    // AVL
    t0 = Clock::now()
    for key in search_keys: avl.search(key)
    t1 = Clock::now()

    // Splay
    t2 = Clock::now()
    for key in search_keys: splay.search(key)
    t3 = Clock::now()

    Escribir fila en ws_results.csv

    // NOTA: No limpiar árboles entre valores de W para que Splay mantenga
    // su working set en la raíz. Los elementos ya están en el árbol.
```

**CSV**: `ws_results.csv`

```csv
W,avl_search_ms,splay_search_ms
10,...
100,...
...
```

#### Bonus: Traversal Conjecture

```
N = 2^25

// Generar dataset
dataset = generateUniform(N)

// T1 con permutación original
T1 = SplayTree()
for key in dataset: T1.insert(key)

// T2 con distinta permutación
shuffle(dataset, seed+1)
T2 = SplayTree()
for key in dataset: T2.insert(key)

// Obtener secuencia en preorden de T1
function getPreorder(node, &seq):
    if !node: return
    seq.push_back(node->key)
    getPreorder(node->left, seq)
    getPreorder(node->right, seq)

seq = []
getPreorder(T1.getRoot(), seq)

// Buscar esa secuencia en T2, midiendo tiempos individuales
open: traversal_results.csv
step = 0
for key in seq:
    t0 = Clock::now()
    T2.search(key)   // splay(key) dentro
    t1 = Clock::now()
    Escribir fila: step, elapsedMs(t0, t1)
    step++

T1.clear(); T2.clear()
```

**CSV**: `traversal_results.csv`

```csv
step,search_time_ms
0,0.0012
1,0.0009
...
```

---

### 2.8 `src/main.cpp` — CLI

```cpp
#include <iostream>
#include <string>
#include "experiment.hpp"

const char* getOpt(int argc, char** argv, const std::string& flag) {
    for (int i = 1; i < argc - 1; ++i) {
        if (flag == argv[i]) return argv[i + 1];
    }
    return nullptr;
}

bool hasFlag(int argc, char** argv, const std::string& flag) {
    for (int i = 1; i < argc; ++i) {
        if (flag == argv[i]) return true;
    }
    return false;
}

void usage() {
    std::cerr << "Uso: ./bin/trees run [--c 5] [--lambda 0.01] [--seed 42] "
              << "[--out-dir results/] [--bonus]\n";
}

int cmdRun(int argc, char** argv) {
    const char* c_str = getOpt(argc, argv, "--c");
    const char* lambda_str = getOpt(argc, argv, "--lambda");
    const char* seed_str = getOpt(argc, argv, "--seed");
    const char* out_dir = getOpt(argc, argv, "--out-dir");
    bool bonus = hasFlag(argc, argv, "--bonus");

    int C = c_str ? std::stoi(c_str) : 5;
    double lambda = lambda_str ? std::stod(lambda_str) : 0.01;
    uint64_t seed = seed_str ? std::stoull(seed_str) : 42ULL;
    std::string dir = out_dir ? out_dir : "results";

    // Crear directorio si no existe
    std::string mkdir_cmd = "mkdir -p " + dir;
    std::system(mkdir_cmd.c_str());

    runBaseScenarios(dir, C, lambda, seed);
    runSequentialAccess(dir, seed);
    runWorkingSet(dir, C, seed);
    if (bonus) runTraversal(dir, seed);

    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) { usage(); return 1; }
    const std::string cmd = argv[1];

    try {
        if (cmd == "run") return cmdRun(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }

    usage();
    return 1;
}
```

---

### 2.9 `Makefile`

```makefile
CXX      ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra
LDFLAGS  ?=

SRC_DIR   := src
BIN_DIR   := bin
BUILD_DIR := build

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
TARGET  := $(BIN_DIR)/trees

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR) $(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)
```

---

### 2.10 Python plotting scripts

**`plots/requirements.txt`**:
```
matplotlib>=3.5
numpy>=1.20
pandas>=1.3
```

**`plots/plot_base.py`**:
```
Entrada: ../results/base_results.csv
Salida:  ../results/base_scenarios.png (4 subplots, uno por config A/B/C/D)

Para cada configuración, gráfico de línea:
- Eje X: N (1024 a 16384)
- Eje Y: tiempo de búsqueda (ms)
- Dos líneas: AVL y Splay
- Escala: log-log (N vs tiempo)
- Leyenda, etiquetas, título

Opcional: incluir las cotas teóricas O(log N) multiplicadas por constante.
```

**`plots/plot_seq.py`**:
```
Entrada: ../results/seq_results.csv
Salida:  ../results/sequential_access.png

Gráfico de línea:
- Eje X: m (número de búsquedas)
- Eje Y: tiempo (ms)
- Líneas: AVL y Splay
- Incluir curvas teóricas:
    - AVL: c1 * m * log2(N)        (constante a determinar)
    - Splay: c2 * m * log2(N/m)     (constante a determinar)
- Leyenda, etiquetas
```

**`plots/plot_ws.py`**:
```
Entrada: ../results/ws_results.csv
Salida:  ../results/working_set.png

Gráfico de línea:
- Eje X: W (escala log, 10 a 10^6)
- Eje Y: tiempo (ms, escala log)
- Líneas: AVL y Splay
- Incluir curvas teóricas:
    - AVL: c1 * log2(N) (constante, independiente de W)
    - Splay: c2 * log2(W)
- Leyenda, etiquetas
```

**`plots/plot_traversal.py`**:
```
Entrada: ../results/traversal_results.csv
Salida:  ../results/traversal.png

Gráfico de línea:
- Eje X: step (índice de la búsqueda)
- Eje Y: tiempo de búsqueda (ms)
- Línea: tiempo por búsqueda
- Opcional: media móvil para suavizar

Análisis: el costo total debe ser O(n) según la conjetura.
Calcular tiempo total como sum(search_time_ms).
```

---

### 2.11 `README.md`

```markdown
# Tarea 2 - Árboles AVL y Splay Tree

## Requisitos

- `g++` (soporte C++17, GCC >= 9)
- `make`
- `python3`, `pip`, `venv` (solo para gráficos)

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

### Parámetros

- `--c`: constante C en {1, ..., 10} (default 5)
- `--lambda`: parámetro de la distribución exponencial [0.001, 0.05] (default 0.01)
- `--seed`: semilla RNG (default 42)
- `--out-dir`: directorio de salida (default `results/`)

### Archivos generados en `results/`

- `base_results.csv`: tiempos de los 4 escenarios base
- `seq_results.csv`: tiempos del Sequential Access Theorem
- `ws_results.csv`: tiempos del Working Set Theorem
- `traversal_results.csv` (si --bonus): tiempos de la Traversal Conjecture

## Gráficos

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r plots/requirements.txt

python3 plots/plot_base.py     # results/base_scenarios.png
python3 plots/plot_seq.py      # results/sequential_access.png
python3 plots/plot_ws.py       # results/working_set.png
python3 plots/plot_traversal.py  # results/traversal.png (si hay bonus)
```

## Estructura del código

- `src/node.hpp`: nodo BST compartido
- `src/avl.hpp`, `src/avl.cpp`: implementación del árbol AVL
- `src/splay.hpp`, `src/splay.cpp`: implementación del Splay Tree
- `src/dataset.hpp`, `src/dataset.cpp`: generación de datos
- `src/experiment.hpp`, `src/experiment.cpp`: experimentos
- `src/main.cpp`: punto de entrada
```

---

### 2.12 `REPORT.md` — Esqueleto

```markdown
# Reporte - Árboles AVL y Splay Tree

> Tarea 2, curso CC4102 (Diseño y Análisis de Algoritmos), DCC, U. de Chile, 2026-1.

## 1. Introducción

> _Presentación del tema en estudio y resumen de lo que se abordará en el informe.
> Mencionar brevemente AVL vs Splay Tree y las diferencias fundamentales (balance
> estricto vs amortizado)._

## 2. Desarrollo

### 2.1 Decisiones de implementación

#### Nodo BST compartido

> _Describir el struct Node y por qué se comparte entre AVL y Splay (campo height
> ignorado por Splay)._

#### Rotaciones primitivas

> _Explicar cómo se implementaron Zig, Zag y cómo se construyen las rotaciones
> dobles (Zig-Zig, Zig-Zag, Zag-Zig, Zag-Zag). Mencionar que AVL solo usa Zig,
> Zag, Zig-Zag (LR), Zag-Zig (RL) mientras que Splay usa las seis._

#### Splay Tree

> _Describir el algoritmo splay recursivo y cómo se integra en insert y search._

#### AVL

> _Describir el balanceo por factor de equilibrio, los 4 casos (LL, RR, LR, RL)
> y cómo se actualizan alturas._

### 2.2 Constantes

- `C = _` (elegir entre 1 y 10)
- `lambda = _` (elegir entre 0.001 y 0.05)
- `semilla = 42`

## 3. Resultados

### 3.1 Entorno de ejecución

> _Llenar con los datos del entorno._

| Atributo | Valor |
|---|---|
| Sistema operativo | _(placeholder)_ |
| CPU | _(placeholder)_ |
| Memoria RAM | _(placeholder)_ |
| Cache L1 / L2 / L3 | _(placeholder)_ |
| Disco | _(placeholder)_ |
| Compilador y flags | g++ -O2 -std=c++17 |

### 3.2 Escenarios base

<!-- Generado por: python plots/plot_base.py -->

![Escenarios base](results/base_scenarios.png)

> _Incluir tabla con tiempos de inserción y búsqueda para cada N y configuración._

### 3.3 Sequential Access Theorem

<!-- Generado por: python plots/plot_seq.py -->

![Sequential Access](results/sequential_access.png)

### 3.4 Working Set Theorem

<!-- Generado por: python plots/plot_ws.py -->

![Working Set](results/working_set.png)

### 3.5 Bonus: Traversal Conjecture

<!-- Generado por: python plots/plot_traversal.py -->

![Traversal Conjecture](results/traversal.png)

## 4. Análisis

> _Discutir los resultados. Preguntas guía:_

1. **Escenarios base**: ¿Cómo se comporta AVL vs Splay bajo inserciones
   aleatorias vs ordenadas? ¿Coincide con la teoría? ¿Dónde se nota la
   naturaleza amortizada del Splay Tree?
2. **Sequential Access**: ¿Se cumple que Splay es O(m * log(N/m)) mientras
   que AVL es O(m * log N)? Comparar pendientes empíricas con cotas teóricas.
3. **Working Set**: ¿El tiempo de Splay escala como O(log W)? ¿El de AVL se
   mantiene constante en O(log N)? ¿Se aprecia la mejora al reducir W?
4. **Bonus**: ¿El costo total de la Traversal Conjecture es O(n)? ¿Qué se
   observa en el gráfico de tiempos por búsqueda?

## 5. Conclusión

> _Recapitulación del trabajo y conclusión final sobre el desempeño de ambas
> estructuras. ¿Cuándo conviene usar cada una?_

## 6. Bonus: Traversal Conjecture

> _Resultados del experimento, gráfico, análisis y veredicto empírico.
> ¿La evidencia apoya la conjetura?_
```

---

### 2.13 `next-steps.md`

```markdown
# Next steps

Lista de lo que falta para completar la entrega de la Tarea 2 (CC4102) según
`mision.md`. El código C++ y los scripts Python están escritos y compilados,
pero **no se ha ejecutado la experimentación completa**.

## 1. Compilación y verificación

- [ ] `make clean && make` en el directorio raíz
- [ ] Verificar que no haya warnings con `-Wall -Wextra`
- [ ] Probar con ejecución rápida:
  ```bash
  ./bin/trees run --c 1 --out-dir /tmp/test_results
  ```
  Debe crear archivos CSV en `/tmp/test_results/`.

## 2. Experimentación base

- [ ] Ejecutar:
  ```bash
  ./bin/trees run --c 5 --lambda 0.01 --seed 42 --out-dir results/
  ```
- [ ] Verificar `results/base_results.csv` (20 filas: 4 configs × 5 N)
- [ ] **Estimación**: los escenarios base son pequeños (N ≤ 16384), tardan
  segundos.

## 3. Experimentos de teoremas (Sequential Access + Working Set)

- [ ] Los experimentos de teoremas se ejecutan automáticamente como parte de
      `run`. Con `N = 2^25` requiere:
    - **~800 MB de RAM** por árbol (solo se construye 1 AVL + 1 Splay a la vez
      ≈ 1.6 GB).
    - Tiempo estimado: minutos a horas dependiendo del hardware (construcción
      de 33M nodos, luego millones de búsquedas).
- [ ] Verificar `results/seq_results.csv` (10 filas)
- [ ] Verificar `results/ws_results.csv` (6 filas)

**Advertencia**: Si el equipo no tiene suficiente RAM (> 4 GB disponibles),
el proceso puede caer en swapping o ser muy lento. En ese caso, reducir
`N` a `2^24` editando `experiment.cpp`.

## 4. Bonus: Traversal Conjecture

- [ ] Ejecutar con `--bonus`:
  ```bash
  ./bin/trees run --c 5 --seed 42 --out-dir results/ --bonus
  ```
- [ ] Verificar `results/traversal_results.csv` (~33 millones de filas).
  **Advertencia**: el archivo puede ser grande (~500 MB).
- [ ] Opcional: samplear cada 1000 pasos para gráfico más manejable.

## 5. Generación de gráficos

- [ ] Crear venv e instalar dependencias:
  ```bash
  python3 -m venv .venv
  source .venv/bin/activate
  pip install -r plots/requirements.txt
  ```
- [ ] Gráficos base:
  ```bash
  python plots/plot_base.py
  ```
- [ ] Sequential Access:
  ```bash
  python plots/plot_seq.py
  ```
- [ ] Working Set:
  ```bash
  python plots/plot_ws.py
  ```
- [ ] Traversal (si hay bonus):
  ```bash
  python plots/plot_traversal.py
  ```

## 6. Completar REPORT.md

- [ ] **Sección 3.1**: llenar tabla con datos del hardware (`lscpu`, etc.)
- [ ] **Secciones 3.2-3.5**: incluir gráficos generados y comentar
- [ ] **Sección 4**: análisis de resultados, comparación con cotas teóricas
- [ ] **Sección 5**: conclusión
- [ ] **Sección 6** (bonus): incluir gráfico y análisis de Traversal Conjecture

## 7. Entrega

- [ ] `make clean && make` desde cero
- [ ] Verificar que `REPORT.md` se renderiza correctamente
- [ ] README.md completo y claro
- [ ] Empaquetar (zip/tar) o subir según instructivo del curso

## Riesgos

- **RAM**: 2 árboles de 2^25 nodos ~ 1.6 GB. Con estructuras auxiliares
  puede llegar a 3-4 GB. Monitorear con `htop` o `free -h`.
- **Tiempo**: La construcción de 2^25 nodos y millones de búsquedas puede
  tomar horas en hardware modesto.
- **Disco**: `traversal_results.csv` con 33M líneas puede ocupar ~500 MB.
- **Reproducibilidad**: Mantener la semilla fija (42) entre corridas.
```

---

## 3. Orden de implementación

1. `src/node.hpp`
2. `src/splay.hpp` + `src/splay.cpp` (rotaciones primero, luego splay, luego
   insert/search — como recomienda la misión)
3. `src/avl.hpp` + `src/avl.cpp` (reusa rotateZig, rotateZag de splay)
4. `src/dataset.hpp` + `src/dataset.cpp`
5. `src/experiment.hpp` + `src/experiment.cpp`
6. `src/main.cpp` + `Makefile`
7. `plots/` (requirements.txt + 4 scripts)
8. `README.md` + `REPORT.md` + `next-steps.md`

---

## 4. Notas adicionales

- **Memoria**: Cada `Node` tiene 4 bytes (key) + 2 punteros (16 bytes en 64-bit)
  + 1 int (4 bytes) = 24 bytes. Para 2^25 ≈ 33.5M nodos = ~805 MB por árbol.
  Con dos árboles simultáneos ≈ 1.6 GB. Más datasets (33M uint32 = 128 MB).
  Total ~ 2-3 GB durante experimentos de teoremas.
- **Rotaciones compartidas**: Las funciones `rotateZig` y `rotateZag` son
  idénticas entre AVL y Splay. En vez de duplicar, se pueden definir como
  funciones libres en `node.hpp` o tener versiones separadas en cada clase.
  La opción recomendada (más limpia): cada clase tiene sus propias versiones,
  pero el código es idéntico.
- **Splay recursivo vs iterativo**: La implementación recursiva es más
  compacta y se alinea con la notación de la misión. Para N = 2^25 la
  recursión puede ser profunda (hasta 2^25 en el peor caso), lo que puede
  causar stack overflow. **En splay.cpp, usar implementación iterativa** (o
  recursiva pero con una pila explícita). Alternativa: la implementación
  recursiva estándar de splay con el patrón de "splay on the way down" (top-down
  splay) es iterativa y evita problemas de stack.
- **Splay top-down**: La implementación más segura para 2^25 nodos es la
  top-down splay (iterativa, sin recursión), descrita en el paper original de
  Sleator & Tarjan (1985). Se recomienda esta para evitar stack overflow.
