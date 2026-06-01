#pragma once

#include "point.hpp"
#include "rtree_node.hpp"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

// Contador global de lecturas a disco. Cada llamada a readNode() lo incrementa
extern std::size_t g_disk_reads;

// Pone el contador en 0, usado antes de medir una consulta.
void resetReadCounter();

// Lee N puntos desde un archivo binario de pares (float x, float y).
// Si N <= 0 o N supera la cantidad de puntos del archivo, lee todos.
//   path: ruta al archivo binario.
//   N:    cantidad de puntos a leer (0 o negativo => todos).
//   ret:  vector con los puntos leidos.
std::vector<Point> readPointsBin(const std::string& path, long long N);

// Escribe el arbol a disco como bloques secuenciales de NODE_SIZE bytes.
// Cada Node se escribe tal cual (memoria cruda).
//   path: archivo de salida.
//   tree: vector de nodos (indice 0 es la raiz).
void writeTreeBin(const std::string& path, const std::vector<Node>& tree);

// Lee el nodo en la posicion idx desde el archivo binario y lo deja en out.
// Incrementa g_disk_reads en 1.
//   in:  stream binario abierto sobre el arbol serializado.
//   idx: indice del nodo (offset = idx * NODE_SIZE).
//   out: nodo destino.
void readNode(std::ifstream& in, std::int32_t idx, Node& out);
