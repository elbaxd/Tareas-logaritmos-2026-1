# Instrucción

Tu misión es implementar en lenguaje C++ dos métodos de bulk-loading (carga masiva de datos) para crear R-Trees que se puedan consultar en memoria externa. Los R-trees guardarán puntos en dos dimensiones, y las consultas serán del tipo "retornar todos los puntos contenidos en un rectangulo R"

Para comparar el rendimiento de los métodos de bulk-loading se utilizaran dos dataset, ambos diponibles en: https://github.com/claugaete/tarea1-cc4102-2026-1

1. Un dataset con puntos aleatorios uniformemente distribuidos en el rango [0, 1] x [0, 1]
2. Un dataset real con las ubicaciones de edificios a lo largo de Europa. Las coordenadas de estos puntos fueron normalizadas al mismo rango [0, 1] x [0, 1]

Para ambos datasets, los puntos estan guardados de forma desordenada en un archivo binario, como pares de 2 floats (4 bytes cada uno), indicando las coordenadas x e y de cada punto.

## Definición de R-trees

El R-tree es una estructural similar al B-tree, pero que permite guardar puntos en multiples dimensiones (en el caso de este problema, solo 2 dimensiones).

Cada nodo está representado por un par clave-valor, la clave es el rectángulo mas pequeño que contiene a todos los hijos del nood (llamado Minimum Bounding Rectangle: MBR). El valor es un puntero a la dirección donde estan guardados todos los hijos; estos hijos a su vez tienen su propia clave (su MBR) y su propio valor (puntero a sus hijos).

En el nivel mas bajo del arbol, la clave de cada hijo es un punto (dado que no hay mas hijos).

Para efectos de esta implementación, cada nodo del R-tree tendrá los siguiente campos:

 - k: int de 4 bytes que guarda la cantidad de hijos actualmente contenidos en el nodo. k puede fluctuar entre 1 y b.
 - hijos: un arreglo de b pares clave-valor, donde:

   - cada clave está compuesta de 4 floats (total: 16 bytes), representando (x_1, x_2, y_1, y_2) (los limites inferior y superior del MBR en cada coordenada). Si el nodo es una hoja, entonces se tiene x_1=x_2 e y_1=y_2 (el MBR es un punto).
   - Cada valor es un int de 4 bytes, que representa la posicion del hijo en el archivo (0 si es el primer nodo del archivo, 1 si es el segundo, etc). Si el nodo es una hoja: el valor es -1 (indicando que no existe el hijo).

- pad: 12 bytes que quedan libres, para que el tamaño del nodo coincida con el tamaño de un bloque.

Para esta implementación, se utilizará b=204, así cada nodo tendrá un tamaño estandar de 4096 bytes (el tamaño típico de un bloque en disco)

### Utilización de memoria externa

Los R-trees se guardaran de manera serializada en un archivo binario: como se vio anteriormente, cada bloque del disco contiene exactamente un nodo, y cada nodo padre guarda las posiciones en disco de sus nodos hijos (para poder acceder a ellos cuando necesite).

Sin embargo, tener el arbol completamente en disco implica que cada inserción requiere multiples lecturas, realizar las millones de inserciones que se requieren en la implementación toma un tiempo excesivo.

Por ende, para la creación del R-tree se simulará la serialización de la siguiente forma:

 - El arbol sera representado como un arreglo (o vector) de nodos en RAM; el primer elemento del areglo siempre sera el nodo raíz.
 - Cada nodo interno guardará el indice en el cual esta ubicado cada uno de sus hijos.

Una vez se haya construido el arbol, este se guardará en disco escribiendo cada uno de los nodos de forma secuencial a un archivo binario. Las consultas al arbol se deben hacer leyendo desde el archivo binario, que en terminos de codigo es muy similar a leer desde el arreglo en RAM, solo que en vez de acceder al elemento i de un arreglo, tenemos que leer desde la posición i*(tamaño de un nodo en bytes) del archivo binario.

## Implementación

Se espera que implementes mecanismos para construir un R-tree mediante bulk-loading y para realizar consultas de todos los puntos que caen dentro de un rectangulo R={x_min, x_max, y_min, y_max}. NO SE DEBE IMPLEMENTAR LA INSERCIÓN NI BORRADO DE ELEMENTOS INDIVIDUALES. Recuerda que para el bulk-loading se guarda el arbol en un arreglo de nodos en RAM, mientras que para la busqueda, los nodos se deben leer desde un archivo en disco.

### Bulk-loading

Implementa los siguientes métodos de bulk-loading:

#### Nearest-X

Dado un conjunto de n pares llave-valor, donde las llaves son rectangulos (MBRs) y los valores son los indices correspondientes al nodo hijo, hacemos lo siguiente:

 1. Ordenar los rectangulos según la coordenada X del centro del rectangulo.
 2. Juntar en n/b grupos de rectangulos consecutivos, cada uno de estos grupos formara un nodo con b elementos (el ultimo nodo puede tener menos).
 3. Guardar los n/b nodos creados en el vector de nodos, DEJANDO EL PRIMER ESPACIO VACÍO (la raíz va ahí).
 4. Para cada uno de los n/n nodos creados, calcular su MBR y generar n/b pares llave-valor, donde nuevamente las llaves son rectangulos y los valores son los indices asociados a cada nodo (la posición en la que se guardaron en el vector de nodos)
 5. Si los n/b pares llave-valor entran en un nodo (es decir, n/b <= b), entonces creamos el nodo raiz con todos los pares llave-valor y lo guardamos en la primera posicion del vector de nodos. Si no, entonces aplicamos Nearest-X recursivamente a los n/n pares, generando un nivel mas en el R-tree

El algoritmo inicia con n=N pares llave-valor, donde cada llave es un punto (rectangulo de area 0, con x_1=x_2 e y_1=y_2) y cada indice es -1 (es decir, no se tiene un nodo hijo en el vector de nodos).

#### Sort-Tile-Recursive (STR)

Dado un conjunto de n pares llave-valor, donde las llaves son rectangulos (MBRs) y los valores son los indices correspondientes al nodo hijo, hacemos lo siguiente:

 1. Ordenar los rectangulos según la coordenada X del centro del rectangulo.
 2. Juntar en S = sqrt(n/b) grupos de rectangulos consecutivos
 3. Para cada grupo:
   3.1. Ordenar los rectangulos segun la coordenada Y del centro del rectangulo
   3.2. Dividir nuevamente en S = sqrt(n/b) grupos, en total, vamos a tener S^2 = n/b grupos, cada uno de estos grupos formara un nodo con b elementos
   4. Repetir los pasos 3 a 5 de Nearest-X (haciendo la recursión con STR si no entran todos los pares llave-valor en un unico nodo raíz)

Al igual que Nearest-X, el algoritmo inicia con n=N pares llave-valor, donde cada llave es un punto (rectangulo de area 0, x_1=x_2 e y_1=y_2) y cada indice es -1 (es decir, no se tiene un nodo hijo en el vector de nodos).

### Consultas

Para realizar una consulta del tipo "retornar los puntos contenidos en un rectangulo R", se inicia leyendo desde el nodo raiz (posicion 0 en el archivo binario). Para cada hijo, si su MBR intersecta con R, se lee el nodo hijo y se realiza la consulta recursivamente. Al llegar a una hoja (caso base), se retornarn todos los puntos del nodo que estén dentro de R.

## Experimentación

### Construcción

Para cada N en el conjunto {2^15, 2^16, ..., 2^24}, realizar lo siguiente:

 - Crear 4 R-trees mediante bulk-loading:
   - Dataset aleatorio (los primeros N elementos), usando Nearest-X.
   - Dataset aleatorio (los primeros N elementos), usando STR.
   - Dataset de Europa (los primeros N elementos), usando Nearest-X.
   - Dataset de Europa (los primeros N elementos), usando STR.

Debes reportar y graficar el tiempo de creación de cada arbol en función de N.

### Consultas

Para las consultas, usaremos los 4 R-trees con N=2^24 elementos que se construyeron en la parte anterior. Estos arboles deben ser escritos a disco (escribiendo cada nodo de manera secuencial).

CONSULTAS REALIZADAS A ARBOLES GUARDADOS EN RAM NO SE CONSIDERAN CORRECTAS.

Para cada s = {0.0025, 0.005, 0.01, 0.025, 0.05}

 - Generar 100 cuadrados aleatorios, con lados de largo s y en el rango [0, 1] x [0, 1]
 - Para cada arbol, consultar por los puntos contenidos en cada uno de los 100 cuadrados generados.

Para cada arbol, debes reportar y graficar:
 - La cantidad proomedio de lecturas a disco en cada consulta (como varía en funcion de s).
 - La cantidad promedio de puntos encontrados (como varía en funcion de s), con barras de error que muestren la desviacion estandar (la desviación será baja si las 100 consultas retornaron un numero similar de puntos, y sera alta si algunas consultas retornaron muchos puntos y otras muy posiciones)

 ### Bonus

 Construye un R-tree usando el archivo europa_bonus.bin disponible en el repositorio antes mencionado. Este dataset contiene los mismos datos de Europa, pero en su version no normalizada, por lo cual las coordenadas X e Y representan longitud y latitud respectivamente. El rango de este dataset es [-11, 35] x [35, 72].

 Debes realizar una consulta alrededor de una ubicacion que tu decidas, y generar un gráfico con los puntos obtenidos (usando un scatterfield). Debes indicar que ubicación escogiste.

 ## Requisitos

 - Debes generar el código en C++.
 - Debes generar el código para generar los gráficos en Python.
 - Un README con instrucciones para ejecutar el codigo, debe ser lo suficientemente explicativo para que cualquier persona solo leyendo este archivo pueda ejecutar la totalidad del código, incluyendo librerías.
 - Cada estructura de datos y funcion debe tener una descripción de lo que hace y una descripción de sus parametros de entrda y salida.
 - El uso del disco debe ser correcto:
   - Al crear el arbol R-tree se deben guardar los nodos en un arreglo secuencial, simulando el disco.
   - Una vez completo el arbol, se debe guardar en un archivo binario antes de realizar consultas.
   - Se debe poder realizar la interpretacion del archivo binario en memoria principal para realizar las busquedas.
   - Durante las consultas, se deben realizar lecturas por bloque (un bloque es equivalente a un nodo para efectos de esta implementacion)
- Debes generar un reporte claro y conciso en markdown, bajo REPORT.md, este debe contener:
  - Introducción: presentar el tema ene studio, resumir lo que se dira en el reporte y presentar una hipotesis
  - Desarrollo: presentación de algoritmos, estrucutras de datos, como funcionan y por que. Recordar que los metodos a implementar ya son conocidos por quien leera este reporte, lo que importa aca es tu propia implementación (que desiciones tomaste que no son parte de la instrucción).
  - Resultados: Especificación de los datos que se utilizaro pra los experimentos, la cantidad de veces que se realizaron los tests, con que inputs, que tamaño, etc. Se debe mencionar el sistema operativo y los tamaños de sus caches y RAM con los que se ejecutaron los experimentos. Se deben mostrar graficos/tablas con la infromación solicitada en la seccion Experimentación, y mencionar solo lo que se puede observar de estos.
  - Analisis: comentar y concluir los resultados. se hacen las inferencias de sus resultados, aplicando conocimientos de Diseño y Análisis de Algoritmos (curso CC4102, DCC, UCHILE).
  - Conclusión: Recapitulacion de lo que se hizo, se concluye lo que se puede decir con respecto a sus resultados, tambien ven si su hipotesis se cumplio o no y analizan la razon. Por ultimo, menciona que se podria mejorar en el desarrollo en una versión futura, que falta en el documento, que no está resuelto y como se podrían extender.

## Datos útiles

  - Si haces busquedas usando cuadrados de largo 0.01 en el dataset aleatorio, deberías obtener un 0.01% de los datos en cada busqueda. Si estas obteniendo muchos menos datos qeu eso, significa que tu busqueda no esta funcionando adecuadadamente (o construiste mal el arbol).
  - Para contabilizar las lecturas durante las consultas, te recomiendo crear una funcion readNode, que lea un nodo del disco (con un cierto offset) y automaticamente sume 1 a un contador de lecturas.
  - Si estas usando C++ te puede ser util el siguiente repositorio: https://github.com/pabloskewes/material-complementario-logs-2025-1/blob/main/serializacion_bst/README.md . Ese proyecto consiste en serializar un arobl de busqueda binaria, por lo que comparte similitudes con lo que te pido (Ej: al leer y escribir estrucutras en disco). Tambien te puede servir como referencia en cuanto a calidad del README y firmas.
