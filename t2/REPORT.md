# Reporte - Arboles AVL y Splay Tree

> Tarea 2, curso CC4102 (Diseno y Analisis de Algoritmos), DCC, U. de Chile, 2026-1.

## 1. Introduccion

> _Presentacion del tema en estudio y resumen de lo que se abordara en el informe.
> Mencionar brevemente AVL vs Splay Tree y las diferencias fundamentales (balance
> estricto vs amortizado)._

## 2. Desarrollo

### 2.1 Decisiones de implementacion

#### Nodo BST compartido

> _Describir el struct Node y por que se comparte entre AVL y Splay (campo height
> ignorado por Splay)._

#### Rotaciones primitivas

> _Explicar como se implementaron Zig, Zag y como se construyen las rotaciones
> dobles (Zig-Zig, Zig-Zag, Zag-Zig, Zag-Zag). Mencionar que AVL solo usa Zig,
> Zag, Zig-Zag (LR), Zag-Zig (RL) mientras que Splay usa las seis._

#### Splay Tree

> _Describir el algoritmo splay top-down iterativo y como se integra en insert y search._

#### AVL

> _Describir el balanceo por factor de equilibrio, los 4 casos (LL, RR, LR, RL)
> y como se actualizan alturas._

### 2.2 Constantes

- `C = _` (elegir entre 1 y 10)
- `lambda = _` (elegir entre 0.001 y 0.05)
- `semilla = 42`

## 3. Resultados

### 3.1 Entorno de ejecucion

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

![Escenarios base](results/base_scenarios.png)

![Busquedas individuales](results/base_per_search.png)

> _Incluir tabla con tiempos de insercion y busqueda para cada N y configuracion._

### 3.3 Sequential Access Theorem

![Sequential Access](results/sequential_access.png)

### 3.4 Working Set Theorem

![Working Set](results/working_set.png)

### 3.5 Bonus: Traversal Conjecture

![Traversal Conjecture](results/traversal.png)

## 4. Analisis

> _Discutir los resultados. Preguntas guia:_

1. **Escenarios base**: Como se comporta AVL vs Splay bajo inserciones
   aleatorias vs ordenadas? Coincide con la teoria? Donde se nota la
   naturaleza amortizada del Splay Tree?
2. **Sequential Access**: Se cumple que Splay es O(m * log(N/m)) mientras
   que AVL es O(m * log N)? Comparar pendientes empiricas con cotas teoricas.
3. **Working Set**: El tiempo de Splay escala como O(log W)? El de AVL se
   mantiene constante en O(log N)? Se aprecia la mejora al reducir W?
4. **Bonus**: El costo total de la Traversal Conjecture es O(n)? Que se
   observa en el grafico de tiempos por busqueda?

## 5. Conclusion

> _Recapitulacion del trabajo y conclusion final sobre el desempeno de ambas
> estructuras. Cuando conviene usar cada una?_

## 6. Bonus: Traversal Conjecture

> _Resultados del experimento, grafico, analisis y veredicto empirico.
> La evidencia apoya la conjetura?_
