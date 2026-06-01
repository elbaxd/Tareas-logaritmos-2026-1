# Next steps

Lista de lo que falta para completar la entrega de la Tarea 2 (CC4102) segun
`mision.md`. El codigo C++ y los scripts Python estan escritos y compilados.
**Se verifico el correcto funcionamiento end-to-end con C=1 (~35 min).**
Falta ejecutar el experimento completo con C=5.

## 1. Compilacion y verificacion

- [x] `make clean && make` en el directorio raiz
- [x] Verificar que no haya warnings con `-Wall -Wextra`
- [x] Smoke test end-to-end con C=1 (~35 min, verifica que todo funcione)

## 2. Experimentacion base

- [ ] Ejecutar experimento completo:
  ```bash
  ./bin/trees run --c 5 --lambda 0.01 --seed 42 --out-dir results/
  ```
- [ ] Verificar `results/base_results.csv` (20 filas: 4 configs x 5 N)
- [ ] Verificar `results/base_per_search.csv`

## 3. Experimentos de teoremas (Sequential Access + Working Set)

- [ ] Ejecutar con C=5:
  - `results/seq_results.csv` (10 filas)
  - `results/ws_results.csv` (6 filas)
  - Sequential Access: ~2 min (no escala con C)
  - Working Set: M = 50*N = ~1.68 mil millones de busquedas por W
  - Estimado: ~2.5-3h totales con C=5
  - RAM requerida: ~3 GB pico

## 4. Bonus: Traversal Conjecture

- [ ] Ejecutar con `--bonus`:
  ```bash
  ./bin/trees run --c 5 --seed 42 --out-dir results/ --bonus
  ```
- [ ] Verificar `results/traversal_results.csv` (~33 millones de filas).
  **Advertencia**: el archivo puede ser grande (~500 MB).
- [ ] Opcional: samplear cada 1000 pasos para grafico mas manejable.

## 5. Generacion de graficos

- [ ] Crear venv e instalar dependencias
- [ ] Graficos base: `results/base_scenarios.png`, `results/base_per_search.png`
- [ ] Sequential Access: `results/sequential_access.png`
- [ ] Working Set: `results/working_set.png`
- [ ] Traversal: solo si se ejecuta con `--bonus`

## 6. Completar REPORT.md

- [x] Seccion 2: Decisiones de implementacion detalladas
- [x] Constantes: C=5, lambda=0.01, seed=42
- [ ] Seccion 3.1: llenar tabla con datos del hardware (`lscpu`, `free -h`)
- [ ] Secciones 3.2-3.5: incluir graficos generados con C=5
- [ ] Seccion 4: analisis de resultados, comparacion con cotas teoricas
- [ ] Seccion 5: conclusion
- [ ] Seccion 6 (bonus): solo si se ejecuta Traversal Conjecture

## 7. Entrega

- [ ] Borrar `results/` (contiene datos de verificacion con C=1)
- [ ] Ejecutar experimento completo con C=5
- [ ] Ejecutar `--bonus` (Traversal Conjecture)
- [ ] Generar todos los graficos
- [ ] `make clean && make` desde cero
- [ ] Verificar que `REPORT.md` se renderiza correctamente
- [ ] `README.md` completo y claro
- [ ] Empaquetar (zip/tar) o subir segun instructivo del curso

## Riesgos

- **RAM**: 2 arboles de 2^25 nodos ~ 1.6 GB. Con estructuras auxiliares
  puede llegar a 3-4 GB. Monitorear con `htop` o `free -h`.
- **Tiempo**: La construccion de 2^25 nodos y millones de busquedas puede
  tomar horas en hardware modesto.
- **Disco**: `traversal_results.csv` con 33M lineas puede ocupar ~500 MB.
- **Reproducibilidad**: Mantener la semilla fija (42) entre corridas.
