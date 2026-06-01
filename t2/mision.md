# Instrucción

## Contexto
El árbol AVL fue presentado en 1962 como el primer árbol de búsqueda binaria autobalanceable, es decir, que está constantemente equilibrado. Esto lo consigue mediante una variable adicional denominada Balance Factor (en español, Factor de Equilibrio) la cual almacena toda la información necesaria para saber si los hijos de cada nodo están o no balanceados. Lo anterior permite conseguir un costo de O(log n) para búsqueda, inserción y eliminación tanto promedio como peor caso al mantener una altura máxima de aproximadamente 3/2 log_2(n) para cada nodo.

El Splay Tree es otra estructura de árbol de búsqueda binaria autobalanceable, la cual, sin la necesidad de almacenar información adicional para mantener el equilibrio, alcanza un costo amortizado de O(log n) para las mismas tres operaciones. Es más, para una secuencia de accesos a nodos con distintas probabilidades tiene un costo amortizado de O(K), donde K es la entropía de esas probabilidades (visto en detalle más adelante).

El objetivo de esta misión es comparar ambas estructuras en distintos escenarios de inserción y búsqueda, además de comprobar experimentalmente el desempeño mediante ciertos teoremas probados para los Splay Trees.

## Conceptos Preliminares

### Notación
Durante esta misión se ocupará la notación X=x(A, B) para denotar que X es un árbol con el elemento x en la raíz, subárbol izquierdo A y subárbol derecho B. Además, se usará el símbolo A → B para denotar una rotación de un árbol A a un árbol B.

Por ejemplo: y(x(A, B), C) → x(A, y(B, C))
Representa una rotación simple hacia la derecha, donde x es el hijo izquierdo de y, y luego y pasa a ser el hijo derecho de x.

### Operaciones Básicas
Para la implementación de las estructuras, se definen una serie de rotaciones primitivas, con las cuales se pueden realizar operaciones más complejas tales como splay:

#### Rotaciones Simples:
- Zig: y(x(A, B), C) → x(A, y(B, C))
- Zag: y(A, x(B, C)) → x(y(A, B), C)

Notar que Zig corresponde a la rotación simple a la derecha del ejemplo que se vio en el punto anterior.

#### Rotaciones Dobles
- Zig-Zig: z(y(x(A, B), C), D) → x(A, y(B, z(C, D)))
- Zig-Zag: z(y(A, x(B, C)), D) → x(y(A, B), z(C, D))
- Zag-Zig: z(A, y(x(B, C), D)) → x(z(A, B), y(C, D))
- Zag-Zag: z(A, y(B, x(C, D))) → x(y(z(A, B), C), D)

## Árboles AVL
El Balance Factor de un nodo se define como la diferencia entre las alturas del subárbol izquierdo y derecho, es decir:

BF(x(A, B)) = height(A) - height(B)

Un árbol AVL es árbol de búsqueda binaria donde el factor de equilibrio de cada uno de sus nodos no es mayor a 1 ni menor a -1, es decir, que su subárbol izquierdo tiene a lo más un nivel más de profundidad que su subárbol derecho (o al revés). De esta forma, se asegura que la altura de cada nodo es menor a 3/2 log_2(n) como se vio anteriormente. Dicho de otra forma, un árbol AVL es un ABB que mantiene el invariante:

Para todo x_i existe x(A, B) tal que |BF(x)| <= 1

Se dice que un nodo x está "desbalanceado" si BF(x) > 1

Esta propiedad de los AVL se consigue mediante una rotación luego de cada operación que altere la estructura del árbol (como inserciones o borrados) en caso de ser necesaria.

Nota Importante: Una sola operación no puede aumentar la altura de un nodo en más de una unidad, por lo que simplemente se asume que un nodo x está desbalanceado si BF(x)=2.


### Búsqueda en un AVL
La búsqueda en un árbol AVL se hace de la misma manera que en un ABB clásico. Considerando el caso cuando se quiere buscar el elemento x en el árbol r(A, B):

- Si r = x, Se termina la búsqueda exitosamente.
- Si x < r, se busca x en A recursivamente.
- Si x > r, se busca x en B recursivamente.

Si eventualmente se llega a un árbol vacío, significa que el elemento x no existe.

### Inserción en un AVL
La inserción en un árbol AVL es similar a la inserción en un ABB clásico, con la diferencia que se debe verificar el factor de balance en cada llamada recursiva y realizar una rotación en caso de ser necesaria. En efecto, si se quiere insertar el elemento x en el árbol r(A, B), se describe el proceso de inserción a continuación:

1. Primero, insertar x dentro del árbol.
  - Si r es vacío, insertar x en r.
  - Si x < r, insertar en A recursivamente.
  - Si x > r, insertar en B recursivamente.

2. Luego, para cada nodo en el camino de vuelta a la raíz del árbol, se debe:
  - Actualizar su altura.
  - Recalcular su factor de equilibrio.
  - Verificar que se siga cumpliendo el invariante: Si no se cumple, se debe balancear con una rotación.

### Rotaciones en un AVL
Como se vio anteriormente, al encontrar un nodo desbalanceado (un nodo z con |BF(z)|=2) en el camino de vuelta, se debe realizar una rotación para restaurar el invariante.

En efecto, considerando lo siguiente:
 - z: nodo desbalanceado
 - y: su hijo de mayor altura
 - x: el nieto que produjo el desbalance

Se pueden encontrar 4 casos posibles de desbalance:

#### Left Left (LL)
El desbalance está en el subárbol izquierdo del hijo izquierdo, es decir:
z(y(x(A, B), C), D)
Se identifica si BF(z)=2 y BF(y)>=0

Solución: Aplicar un Zig sobre z.

#### Right Right (RR)
El desbalance está en el subárbol derecho del hijo derecho, es decir:
z(A, y(B, x(C, D)))
Se identifica si BF(z)=-2 y BF(y)<=0

Solución: Aplicar un Zag sobre z.

#### Left Right (LR)
El desbalance está en el subárbol derecho del hijo izquierdo, es decir:
z(y(A, x(B, C)), D)
Se identifica si BF(z)=2 y BF(y)<0

Solución: Aplicar un Zig-Zag sobre z.

#### Right Left (RL)
El desbalance está en el subárbol izquierdo del hijo derecho, es decir:
z(A, y(x(B, C), D))
Se identifica si BF(z)=-2 y BF(y)>0

Solución: Aplicar una rotación doble Zag-Zig sobre z.

#### Nota
Como se puede observar, el AVL nunca utiliza las rotaciones Zig-Zig ni Zag-Zag definidas en la sección anterior, a diferencia del Splay Tree.

## Splay Trees
El Splay Tree es un tipo de árbol de búsqueda binaria que tiene un método distinto de realizar las operaciones, el cual garantiza un costo amortizado de O(K), donde K es la entropía de esas probabilidades, la cual es definida como sigue:

Dada P: U → [0, 1], se tiene:
K(P) = Sumatoria de los u pertenecientes a U de: P(u) * log_2(1/P(u))

Lo que se traduce a O(log n) para claves con distribución uniforme, pues:

P(u) = 1/u, para todo u perteneciente a U

Esto se consigue gracias a que en esta estructura, todas las operaciones (incluso la lectura) modifican la estructura mediante rotaciones, manteniendo el árbol constantemente balanceado al igual que el AVL, pero además mejorando la eficiencia para distribuciones sesgadas de claves gracias a las modificaciones (y sin la necesidad de almacenar información adicional).

La idea principal del Splay Tree es que el nodo más profundo accedido (normalmente el objetivo de la operación) debe quedar en la raíz del árbol. Para ello, una vez accedido un nodo x mediante cualquier operación, éste se lleva a la raíz utilizando una operación llamada splay(x).

### Splay
Como se mencionó anteriormente, luego de acceder a un nodo x, se debe llamar a splay(x) para subirlo hasta la raíz. Esta operación consiste de una serie de rotaciones consecutivas (como las descritas en la sección 3.2: Operaciones Básicas), la cual finalizará una vez que x esté en la raíz del árbol.

Es importante mencionar que las rotaciones simples (zig y zag) solo se realizarán si el nodo y la raíz del árbol, por lo que la secuencia de rotaciones llamadas por splay consistirá de 0 o más rotaciones dobles consecutivas seguidas de una posible rotación simple.

### Búsqueda en un Splay Tree
La búsqueda en un Splay Tree se hace de la misma manera que en un ABB clásico, o, lo que es lo mismo, de la misma forma que un AVL (Ver sección 4.1: Búsqueda en un AVL). Luego, se hace splay(x).

Si x no se encuentra, se hace splay(x'), donde x' es el último nodo visitado, o mejor dicho, el nodo que sería el padre de x en caso de estar este último en el árbol.

### Inserción en un Splay Tree
La inserción en un Splay Tree se hace de la misma manera que en un ABB clásico. En efecto, la inserción del nodo x en el árbol r(A, B):

- Si r = none, se crea un nodo x(none, none) y se inserta en lugar de r.
- Si x <= r, se inserta en A recursivamente.
- Si x > r, se inserta en B recursivamente.

Luego de finalizada la inserción del elemento, se hace splay(x).

### Sequential Access Theorem
El Static Finger Theorem establece que dado un elemento fijo f del universo, llamado "dedo" (finger), el costo total de una secuencia de m accesos x_1, x_2, ..., x_m en un Splay Tree con n elementos es:

O(n log(n) + m + Sumatoria de i=1..m de log(|x_i - f|) + 1)

Es decir, el costo de acceder a un elemento x_i es proporcional al logaritmo de su distancia al dedo de f: cuanto más cercano a f es el elemento accedido, menor es su costo.

Una consecuencia directa de este resultado es el Sequential Access Theorem: si se buscan m claves en orden estrictamente creciente en un Splay Tree con n elementos, entonces el costo total de la secuencia es:

O(n + m * log(n/m))

### Working Set Theorem
El Working Set Theorem establece que el costo de acceder al elemento x en el instante t es el O(log(t(x) + 1)), donde t(x) denota el número de elementos distintos accedidos entre la última vez que se accedió a x y el instante actual. A este valor se le denomina el tamaño del working set de x en el instante t.

Intuitivamente, los elementos accedidos recientemente ascienden hacia la raíz y son baratos de volver a acceder. Más formalmente, si una aplicación trabaja repetidamente sobre un subconjunto activo de W elementos (el working set), el costo amortizado por acceso en un Splay Tree de n nodos será O(log W) en lugar de O(log n).

## Implementación

### Objetivos
Implementa ambas estructuras de datos junto a sus operaciones fundamentales. Es decir, para cada estructura, debes implementar al menos los siguientes métodos:

- insert(x): Inserta un nuevo elemento con clave x en el árbol.
- search(x): Busca el elemento x en el árbol. Puede retornar un booleano o un puntero al nodo, pero lo fundamental es que ejecute toda la lógica de recorrido y reestructuración correspondiente.

Adicionalmente, se exige contar con funciones de rotación que representen las primitivas descritas en la sección 3.2 (Operaciones Básicas), con las cuales se deberán construir las operaciones más complejas como splay(x) y el balanceo del árbol AVL.

## Experimentación

### Construcción

#### Creación de datasets
Para cada N perteneciente a {2^10, 2^11, ..., 2^14} debes construir un dataset de números aleatorios uniformemente distribuidos en el rango [0, 4294967295] = [0, 2^32 - 1]. Estos pueden ser guardados en un arreglo, en un archivo de texto plano o en la forma que se estime conveniente.

#### Generación de función aleatoria
Debes definir una función de probabilidad sesgada basada en la distribución exponencial:

P(i) = e^(-lambda * i) * (1 - e^(-lambda))/(1 - e^(-lambda * N))

Donde, tendrás que fijar el parámetro lambda para crear tu propia función sesgada: Valores cercanos a 0 generan distribuciones casi uniformes y cercanos a 1 generan distribuciones demasiado sesgadas.

Se recomienda usar lambda en [0.001, 0.05] para obtener resultados interesantes.

### Escenarios Base
Para comenzar, debes fijar un c perteneciente a {1, 2, ..., 10} a elección (este valor debe estar   en el código y el reporte).

Luego, para cada N perteneciente a {2^10, 2^11, ..., 2^14} debes realizar N inserciones en ambas estructuras, seguidas de  M = 10 * c * N búsquedas para 4 configuraciones experimentales definidas más adelante.

Los elementos escogidos tanto para la inserción, como para la búsqueda deben ser obtenidos del dataset asociado al N actual.

Además, tendrás que reportar los tiempos totales tanto de inserción como de búsqueda para todos los casos en una tabla y realizar un gráfico de línea para las 4 configuraciones que permita comparar los tiempos de ejecución de ambas estructuras para cada valor de N, el cual muestre el tiempo para cada búsqueda de modo de exponer visualmente la naturaleza amortizada del Splay Tree.

Las configuraciones son descritas a continuación

#### A: Inserción aleatoria, búsqueda uniforme
Realiza N inserciones eligiendo elementos aleatorios con probabilidad uniforme. Luego, realiza M búsquedas sobre elementos aleatorios escogidos con probabilidad uniforme.

#### B: Inserción aleatoria, búsqueda sesgada
Realiza N inserciones eligiendo elementos aleatorios con probabilidad uniforme. Luego, realiza M búsquedas sobre elementos aleatorios escogidos con la función P(i) definida anteriormente.

#### C: Inserción ordenada, búsqueda uniforme
Antes de insertar, ordena los elementos de menor a mayor y realiza N inserciones secuenciales. Luego, realiza M búsquedas sobre elementos aleatorios con probabilidad uniforme.

#### D: Inserción ordenada, búsqueda sesgada
Antes de insertar, ordena los elementos de menor a mayor y realiza N inserciones secuenciales. Luego, realiza M búsquedas sobre elementos aleatorios usando la función P(i) definida anteriormente.

Entrega una breve conclusión sobre los resultados obtenidos, comparándolos con las cotas conocidas para ambas estructuras.

### Teoremas y Naturaleza de los Splay Trees
Además de los escenarios base, deberás comprobar el comportamiento de ambas estructuras frente a los dos teoremas enunciados anteriormente para un nuevo dataset N' con N=2^25 elementos aleatorios en el rango  [0, 4294967295] = [0, 2^32 − 1] generado por ti.

Luego de eso, deberás crear tanto un AVL tree como un Splay Tree para ambos experimentos, cuidando que los elementos hayan sido insertados en el mismo orden para cada uno de los 4 experimentos de esta sección.

Finalmente, deberás mostrar el desempeño de ambas estructuras frente a los dos escenarios experimentales descritos a continuación en un gráfico de línea.

#### Experimentos

##### A: Sequential Access Theorem
Para los valores de m en {N/100, 2N/100, ..., 9N/100, 10N/100}, ejecuta una secuencia de m búsquedas de claves estrictamente crecientes no necesariamente equiespaciadas y grafica el tiempo de ejecución para cada valor de m en ambas estructuras.

Puedes controlar el crecimiento con algún parámetro o función a elección, pero se recomienda recorrer las claves aumentando su valor en un número fijo.

##### B: Working Set Theorem
Para W en {10, 10^2, 10^3, ..., 10^6} genera un subconjunto W' (que es subconjunto de N') aleatorio de tamaño W (el working set) y realiza una secuencia de M = 10 * c * N búsquedas de elementos escogidos uniformemente al azar exclusivamente dentro de dicho subconjunto en ambos árboles y grafica el tiempo de ejecución para cada valor de W.

#### Análisis de Resultados
Para las secuencias de búsquedas del Sequential Access Theorem, comprueba que en el árbol AVL toman un tiempo de O(m * log n), mientras que el Splay Tree es capaz de aprovechar esta secuencia y resolverlas en O(m * log(n/m)),

Para las secuencias de búsquedas del Working Set Theorem, comprueba que el costo escala como O(log W) en el Splay Tree, mientras que en el árbol AVL permanece como O(log n).

### Bonus: Traversal Conjecture
Así como existen muchos teoremas probados acerca del desempeño de los Splay Trees bajo ciertas condiciones, también quedan bastantes conjeturas sin probar (inclusive algunas se remontan a la presentación original de los Splay Trees en 1985). En particular, en esta sección se tratará la Traversal Conjecture.

#### Enunciado
La Traversal Conjecture postula que dados T_1, T_2 dos Splay Trees con los mismos elementos y dada S la secuencia obtenida producto de visitar todos los nodos de T_2 en preorden, el costo total de buscar secuencialmente los elementos de S en T_1 es O(n).

#### Experimento
Construye dos Splay Trees T_1 y T_2 de tamaño N = 2^25 cada uno, insertando los mismos elementos pero con distintas permutaciones (puedes reutilizar el dataset N'). Luego, ejecuta un recorrido completo en algún orden a elección (preorden, inorden o postorden) sobre T_1, para luego buscar esa misma secuencia en T_2.

Entrega el costo total de este proceso, muestra los tiempos de cada búsqueda en un gráfico de línea y emite una conclusión evaluando empíricamente la conjetura con respecto a los resultados obtenidos.

## Entregables

### Código
Debes entregar el código fuente en ./src escrito en C++ 17 compilable mediante ./Makefile. Una carpeta ./plots con código fuente en Python 3 para generar los gráficos junto a ./requirements.txt para instalar el entorno pip del código Python.

La entrega de código debe contener:

- README: Archivo con las instrucciones claras para compilar y ejecutar el código. Debe ser lo suficientemente explicativo para que cualquier persona pueda ejecutar la totalidad de sus experimentos 
- Firmas: Cada estructura de datos y función relevante debe tener una descripción de su propósito, parámetros de entrada y salida.
- Implementación AVL: Inserción, búsqueda y rotaciones correctas. Preserva el invariante descrito.
- Implementación Splay Tree: Inserción, búsqueda, rotaciones y el método splay son correctas.
- Experimentos: Creación correcta de los 4 escenarios base y los 2 generadores de secuencias para los teoremas.
- Obtención de resultados: La forma en la que se miden los tiempos es rigurosa y suficiente para emitir conclusiones.
- Main: Un archivo principal o parte del código (una función main) que permita ejecutar toda la batería de tests sin modificar el código.
- Bonus: Implementación, testeo y conclusión correspondiente al experimento.

### Reporte
Debes escribir un reporte en formato markdown, este debe ser claro, analítico y conciso. Debe contener:

- Introducción: Presentación del tema en estudio y resumen de lo que se abordará en el informe.
- Desarrollo: Breve explicación de las estructuras y cómo funcionan. Dado que el equipo revisor ya conoce la teoría, enfócate en explicar tus decisiones de implementación. En esta sección debes incluir las constantes y funciones determinadas para su implementación.
- Resultados: Especificación detallada del setup experimental (hardware, SO, caché, RAM de la computadora donde se ejecutaron los experimentos). Debes mostrar los gráficos y tablas solicitadas, con sus ejes correctamente etiquetados.
- Análisis: Discusión de los resultados obtenidos. En esta sección responde las preguntas de análisis que se fueron realizando a lo largo de la misión.
- Conclusión: Recapitulación del trabajo. Conclusión final sobre el desempeño de ambas estructuras.

## Recomendaciones
- Programa los métodos de rotación primero en los Splay Trees de modo que puedan ser replicados para los árboles AVL y así ahorrar trabajo.
- En los gráficos de línea incluye las funciones de las cotas teóricas multiplicadas por alguna constante a determinar que permita comparar más simplemente los resultados obtenidos.
- Mucho cuidado con la notación de los árboles AVL: Recuerda que BF > 0 significa un desbalance hacia la izquierda, mientras que BF < 0 significa un desbalance hacia la derecha. Se menciona esto pues podría ser un poco contrario a la intuición.
- Notar que los experimentos de la sección 7.3 (Experimentos: Sequential Access Theorem y Working Set Theorem) requieren la construcción de 4 árboles de tamaño aproximadamente 2^25 nodos, lo que dependiendo de su implementación, podría traducirse en aproximadamente
4 GB de RAM.
- Se adjunta la publicación original en el siguiente enlace: https://www.cs.cmu.edu/~sleator/papers/self-adjusting.pdf
