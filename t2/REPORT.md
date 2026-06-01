# Reporte - Arboles AVL y Splay Tree

> Tarea 2, curso CC4102 (Diseno y Analisis de Algoritmos), DCC, U. de Chile, 2026-1.

## 1. Introduccion

> _Presentacion del tema en estudio y resumen de lo que se abordara en el informe.
> Mencionar brevemente AVL vs Splay Tree y las diferencias fundamentales (balance
> estricto vs amortizado)._

## 2. Desarrollo

### 2.1 Decisiones de implementacion

#### Nodo BST compartido

Ambas estructuras comparten un mismo `struct Node` definido en `src/node.hpp`:

```cpp
struct Node {
    uint32_t key;
    Node* left;
    Node* right;
    int height;
};
```

- `key`: clave del nodo, entero sin signo de 32 bits en rango `[0, 2³² - 1]`
- `height`: altura del nodo. Usada exclusivamente por el AVL para calcular el factor de equilibrio; el Splay Tree la ignora completamente (se inicializa en 0 pero nunca se actualiza ni consulta en operaciones de Splay)
- Se incluyen funciones helper inline: `getHeight(Node*)` (retorna -1 para `nullptr`), `updateHeight(Node*)` (recalcula altura desde hijos), `bf(Node*)` (factor de equilibrio = altura izquierda - altura derecha)

#### Rotaciones primitivas

Las rotaciones `Zig` (derecha) y `Zag` (izquierda) estan implementadas de forma identica en ambas clases como metodos estaticos:

- **`rotateZig(y)`**: `y(x(A, B), C) → x(A, y(B, C))` — rotacion simple a la derecha
- **`rotateZag(y)`**: `y(A, x(B, C)) → x(y(A, B), C)` — rotacion simple a la izquierda

En el AVL, ademas de reasignar punteros, se llama a `updateHeight(y)` y `updateHeight(x)` para mantener las alturas correctas tras la rotacion. En el Splay Tree no se actualizan alturas.

**Rotaciones dobles del AVL:**
- **`rotateLR(z)`** (`Zig-Zag`): aplica `Zag` sobre `z->left` y luego `Zig` sobre `z`. Caso Left-Right.
- **`rotateRL(z)`** (`Zag-Zig`): aplica `Zig` sobre `z->right` y luego `Zag` sobre `z`. Caso Right-Left.

El AVL no utiliza `Zig-Zig` ni `Zag-Zag` (son exclusivas del Splay Tree).

#### Splay Tree

Se implemento **top-down splay iterativo** (algoritmo de Sleator y Tarjan, 1985) para evitar stack overflow con arboles de 2²⁵ nodos. El algoritmo divide el arbol en tres partes durante el descenso:

1. **Arbol izquierdo** (`L`): nodos menores que la clave buscada
2. **Arbol derecho** (`R`): nodos mayores que la clave buscada
3. **Arbol central** (`t`): nodos aun no examinados

Se usan dos punteros auxiliares (`leftMax` y `rightMin`) que rastrean las posiciones de insercion en L y R. En cada paso del descenso:

- Si `key < t->key`: se evalua si corresponde `Zig-Zig` (rotar derecha sobre `t`), luego se enlaza `t` al arbol derecho y se avanza a `t->left`
- Si `key > t->key`: simetrico con `Zag-Zag` y enlace al arbol izquierdo
- Si `key == t->key`: se termina el descenso

Al finalizar, se reensambla: `t->left` recibe el arbol derecho del header, `t->right` recibe el arbol izquierdo del header, y `t` queda como nueva raiz.

**Insercion**: Insercion iterativa estandar de ABB (sin splay durante el descenso) seguida de `splay(root, key)` para llevar el nuevo nodo a la raiz.

**Busqueda**: `splay(root, key)` directamente, retornando `true` si la nueva raiz coincide con la clave buscada. Si la clave no existe, el ultimo nodo visitado queda en la raiz (comportamiento especificado en la mision).

**Limpieza** (`clear`): Iterativa usando un `std::vector<Node*>` como stack explicito. Recorre el arbol en preorden inverso (push root, pop, push hijos, delete nodo). Evita recursion para no desbordar el stack con arboles de 33.5M nodos donde algun subarbol podria ser profundo.

#### AVL

**Insercion**: Recursiva con balanceo en el camino de vuelta. En cada nivel:
1. Inserta recursivamente en el subarbol correspondiente
2. Actualiza altura del nodo actual (`updateHeight`)
3. Verifica el factor de equilibrio y aplica rotacion si `|BF| > 1`

**Balanceo** (`balance`): Evalua los 4 casos segun el factor de equilibrio:
- `BF > 1` (desbalance izquierdo):
  - `BF(izquierdo) >= 0` → LL → `rotateZig`
  - `BF(izquierdo) < 0` → LR → `rotateLR`
- `BF < -1` (desbalance derecho):
  - `BF(derecho) <= 0` → RR → `rotateZag`
  - `BF(derecho) > 0` → RL → `rotateRL`

**Busqueda**: Recursiva estandar de ABB, sin modificar la estructura.

**Limpieza** (`clear`): Identica a la del Splay Tree: iterativa con `std::vector<Node*>` como stack. Aunque el AVL siempre esta balanceado (altura ~37 para N=2²⁵), se uniformo con la misma implementacion por robustez.

#### Monitoreo de progreso

Cada experimento emite progreso detallado con timestamps para identificar cuellos de botella:
- `[base]`: generacion de dataset, inserciones AVL/Splay (`i`/`I`), generacion de claves (`k`), busquedas (`.` para AVL, `,` para Splay), escritura CSV (`w`)
- `[seq]`: generacion y ordenamiento de dataset, construccion de cada arbol, cada valor de `m` con tiempos AVL/Splay
- `[ws]`: igual que `[seq]` mas cada valor de `W` con setup, generacion de claves y tiempos
- `[traversal]`: construccion de T1/T2, recorrido preorden, busquedas con barra de progreso

Los archivos CSV usan `flush()` tras cada fila para que los datos parciales sobrevivan si el proceso es interrumpido.

### 2.2 Constantes

- `C = 5` (constante para M = 10*C*N busquedas)
- `lambda = 0.01`
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
