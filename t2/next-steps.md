# Next steps

Lista de lo que falta para completar la entrega de la Tarea 2 (CC4102) segun
`mision.md`. El codigo C++ y los scripts Python estan escritos y compilados,
pero **no se ha ejecutado la experimentacion completa**.

## 1. Compilacion y verificacion

- [ ] `make clean && make` en el directorio raiz
- [ ] Verificar que no haya warnings con `-Wall -Wextra`
- [ ] Probar con ejecucion rapida:
  ```bash
  ./bin/trees run --c 1 --out-dir /tmp/test_results
  ```
  Debe crear archivos CSV en `/tmp/test_results/`.

## 2. Experimentacion base

- [ ] Ejecutar:
  ```bash
  ./bin/trees run --c 5 --lambda 0.01 --seed 42 --out-dir results/
  ```
- [ ] Verificar `results/base_results.csv` (20 filas: 4 configs x 5 N)
- [ ] **Estimacion**: los escenarios base son pequenos (N <= 16384), tardan
  segundos.

## 3. Experimentos de teoremas (Sequential Access + Working Set)

- [ ] Los experimentos de teoremas se ejecutan automaticamente como parte de
      `run`. Con `N = 2^25` requiere:
    - **~800 MB de RAM** por arbol (solo se construye 1 AVL + 1 Splay a la vez
      ~ 1.6 GB).
    - Tiempo estimado: minutos a horas dependiendo del hardware (construccion
      de 33M nodos, luego millones de busquedas).
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
- [ ] Opcional: samplear cada 1000 pasos para grafico mas manejable.

## 5. Generacion de graficos

- [ ] Crear venv e instalar dependencias:
  ```bash
  python3 -m venv .venv
  source .venv/bin/activate
  pip install -r plots/requirements.txt
  ```
- [ ] Graficos base:
  ```bash
  python3 plots/plot_base.py
  ```
- [ ] Sequential Access:
  ```bash
  python3 plots/plot_seq.py
  ```
- [ ] Working Set:
  ```bash
  python3 plots/plot_ws.py
  ```
- [ ] Traversal (si hay bonus):
  ```bash
  python3 plots/plot_traversal.py
  ```

## 6. Completar REPORT.md

- [ ] **Seccion 3.1**: llenar tabla con datos del hardware (`lscpu`, `free -h`)
- [ ] **Secciones 3.2-3.5**: incluir graficos generados y comentar
- [ ] **Seccion 4**: analisis de resultados, comparacion con cotas teoricas
- [ ] **Seccion 5**: conclusion
- [ ] **Seccion 6** (bonus): incluir grafico y analisis de Traversal Conjecture

## 7. Entrega

- [ ] `make clean && make` desde cero
- [ ] Verificar que `REPORT.md` se renderiza correctamente
- [ ] README.md completo y claro
- [ ] Empaquetar (zip/tar) o subir segun instructivo del curso

## Riesgos

- **RAM**: 2 arboles de 2^25 nodos ~ 1.6 GB. Con estructuras auxiliares
  puede llegar a 3-4 GB. Monitorear con `htop` o `free -h`.
- **Tiempo**: La construccion de 2^25 nodos y millones de busquedas puede
  tomar horas en hardware modesto.
- **Disco**: `traversal_results.csv` con 33M lineas puede ocupar ~500 MB.
- **Reproducibilidad**: Mantener la semilla fija (42) entre corridas.
