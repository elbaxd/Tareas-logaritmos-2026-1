#pragma once

#include "point.hpp"
#include "rtree_node.hpp"

#include <fstream>
#include <string>
#include <vector>

// Consulta por rango: retorna todos los puntos del R-tree contenidos en el
// rectangulo dado, leyendo los nodos directamente desde disco. Cada lectura
// incrementa g_disk_reads (definido en io.hpp).
//
//   tree_path: ruta al archivo binario del arbol.
//   query:     rectangulo de busqueda.
//   ret:       puntos contenidos en query.
std::vector<Point> queryRange(const std::string& tree_path, const MBR& query);

// Variante que reusa un ifstream ya abierto. Util cuando se hacen muchas
// consultas seguidas (evita reabrir el archivo cada vez).
//
//   in:    stream binario abierto sobre el arbol serializado.
//   query: rectangulo de busqueda.
//   ret:   puntos contenidos en query.
std::vector<Point> queryRange(std::ifstream& in, const MBR& query);
