# Next steps

Lista de lo que falta para completar la entrega de la Tarea 1 (CC4102) segun
`mision.md`. Lo implementado hasta ahora (codigo C++, scripts Python,
Makefile, README y esqueleto de REPORT.md) NO ha sido compilado ni ejecutado.

## 1. Compilacion

- [ ] `cd crons/rt && make`
- [ ] Verificar que no haya warnings con `-Wall -Wextra`.
- [ ] Confirmar que los `static_assert` de `rtree_node.hpp` pasan
  (`sizeof(Entry) == 20`, `sizeof(Node) == 4096`). Si el compilador metiera
  padding distinto al esperado, no compila.

## 2. Datasets

- [ ] Descargar desde
  [claugaete/tarea1-cc4102-2026-1](https://github.com/claugaete/tarea1-cc4102-2026-1):
  - `random.bin`
  - `europa.bin`
  - `europa_bonus.bin`
- [ ] Dejarlos en `crons/rt/data/` (o en la ruta que prefieras y ajustar los
  comandos).

## 3. Verificacion rapida (sanity check)

Antes de la experimentacion completa, validar que la implementacion funciona:

- [ ] Construir un arbol pequeno:
  `./bin/rtree build --method nearest-x --in data/random.bin --N 32768 --out /tmp/tree_small.bin`
- [ ] Consultar:
  `./bin/rtree query --tree /tmp/tree_small.bin --rect 0.0 0.01 0.0 0.01`
- [ ] **Regla del 0.01%**: con `s = 0.01` sobre el dataset aleatorio se debe
  encontrar aproximadamente el `0.01%` de los puntos
  (~1678 para `N = 2^24`). Si encuentras muchos menos, la consulta o el
  arbol estan mal.

## 4. Experimentacion - construccion

- [ ] Ejecutar:
  ```bash
  ./bin/rtree experiment \
      --random data/random.bin \
      --europa data/europa.bin \
      --out-dir results/
  ```
- [ ] Verificar que se hayan creado:
  - `results/build_times.csv`
  - `results/tree_random_nearest-x.bin`
  - `results/tree_random_str.bin`
  - `results/tree_europa_nearest-x.bin`
  - `results/tree_europa_str.bin`
- [ ] **Estimacion**: para `N = 2^24 = 16,777,216` puntos, cada arbol pesa
  ~330 MB en disco. Total ~1.3 GB. Asegurar espacio.
- [ ] Tiempo total estimado: del orden de varios minutos por la suma de
  todas las construcciones (depende del hardware).

## 5. Experimentacion - consultas

El subcomando `experiment` ya corre las 100 consultas por cada
`s in {0.0025, 0.005, 0.01, 0.025, 0.05}` sobre los 4 arboles finales.

- [ ] Verificar que se haya creado `results/query_stats.csv`.
- [ ] Inspeccionar promedio de puntos: para `s = 0.01` en `random` debe ser
  cercano a 1678. Para `europa` el numero variara segun la zona del
  cuadrado.

## 6. Bonus

- [ ] Elegir una ubicacion europea (ej. Madrid, Paris, Berlin, Santiago de
  Compostela...) y definir un rectangulo en coordenadas reales
  (longitud, latitud).
- [ ] Ejecutar:
  ```bash
  ./bin/rtree bonus \
      --in data/europa_bonus.bin \
      --rect <x1> <x2> <y1> <y2> \
      --out results/bonus_<ubicacion>.csv
  ```
- [ ] Anotar la ubicacion elegida y el rectangulo en `REPORT.md` seccion 6.

Ejemplo (Madrid):

```bash
./bin/rtree bonus \
    --in data/europa_bonus.bin \
    --rect -3.9 -3.5 40.3 40.6 \
    --out results/bonus_madrid.csv
```

## 7. Generacion de graficos

- [ ] Crear venv e instalar dependencias:
  ```bash
  python3 -m venv .venv
  source .venv/bin/activate
  pip install -r plots/requirements.txt
  ```
- [ ] Tiempos de construccion:
  ```bash
  python plots/plot_construction.py \
      results/build_times.csv \
      results/build_times.png
  ```
- [ ] Lecturas y puntos por consulta (genera 2 PNGs en `results/`):
  ```bash
  python plots/plot_queries.py \
      results/query_stats.csv \
      results/
  ```
- [ ] Bonus (scatterfield):
  ```bash
  python plots/plot_bonus.py \
      results/bonus_madrid.csv \
      results/bonus_madrid.png \
      --label "Madrid" --center -3.7 40.4
  ```

## 8. Completar REPORT.md

`REPORT.md` esta en estado de esqueleto con todas las secciones requeridas
por el enunciado. Falta llenar:

- [ ] **Seccion 3.1 Entorno**: tabla con SO, CPU, RAM, tamanos de cache
  L1/L2/L3, modelo de disco. Comandos utiles:
  - `lsb_release -a` (SO)
  - `lscpu` (CPU + caches)
  - `free -h` (RAM)
  - `lsblk -o NAME,MODEL,SIZE,ROTA` (disco; `ROTA=0` indica SSD)
- [ ] **Seccion 3.2**: incluir `results/build_times.png` y comentar
  brevemente lo observable.
- [ ] **Seccion 3.3**: incluir `results/query_reads.png` y
  `results/query_points.png` con comentarios.
- [ ] **Seccion 3.4 Verificacion**: reportar el numero promedio de puntos
  para `s = 0.01` en `random` (debe ser ~1678 para N=2^24).
- [ ] **Seccion 4 Analisis**: discutir los puntos listados (Nearest-X vs STR
  en construccion, lecturas, comportamiento vs `s`, variabilidad). Concluir
  si la hipotesis del informe se sostiene.
- [ ] **Seccion 5 Conclusion**: recapitulacion + verificacion de hipotesis +
  trabajo futuro.
- [ ] **Seccion 6 Bonus**: reemplazar placeholder por la ubicacion elegida
  e incluir `results/bonus_<ubicacion>.png`.

## 9. Entrega

- [ ] Revisar que el codigo compile desde cero (`make clean && make`).
- [ ] Revisar que `REPORT.md` se renderiza bien (graficos visibles,
  secciones completas, sin placeholders).
- [ ] Revisar README.md: que cualquier persona pueda compilar y correr todo
  siguiendo solo ese archivo.
- [ ] Empaquetar (zip/tar) o subir al repositorio segun lo que pida el curso.

## Riesgos / cosas a vigilar

- **Memoria RAM**: durante `experiment`, en cada iteracion se mantiene en
  RAM:
  - Los dos datasets completos (~134 MB cada uno para N=2^24).
  - El subset actual (otra copia, hasta 134 MB).
  - El arbol en construccion (~330 MB para N=2^24).
  Total transitorio: del orden de 700-800 MB. En equipos con poca RAM, podria
  ser necesario partir el experimento en dos corridas (random y europa por
  separado) ajustando levemente `experiment.cpp`.
- **`-march=native`**: si quieres mas rendimiento, recompila con
  `make CXXFLAGS="-O3 -std=c++17 -DNDEBUG -march=native"`.
- **Reproducibilidad**: la semilla del generador de queries esta fija
  (`std::mt19937 rng(42)` en `experiment.cpp`). No cambiar entre corridas
  para que los numeros sean comparables.
- **Tiempo de E/S**: si el disco es HDD (no SSD), las consultas seran
  significativamente mas lentas; mencionar el tipo de disco en la seccion
  de entorno del REPORT.
