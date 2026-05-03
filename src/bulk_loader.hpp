#pragma once

#include "point.hpp"
#include "rtree_node.hpp"

#include <vector>

// Construye un R-tree mediante bulk-loading Nearest-X.
// El arbol se devuelve como vector de Nodes en RAM, con la raiz en indice 0.
//   points: puntos de entrada (no se modifican).
//   ret:    vector de nodos del arbol (indice 0 = raiz).
std::vector<Node> buildNearestX(const std::vector<Point>& points);

// Construye un R-tree mediante bulk-loading Sort-Tile-Recursive (STR).
// Mismo formato de retorno que buildNearestX.
//   points: puntos de entrada (no se modifican).
//   ret:    vector de nodos del arbol (indice 0 = raiz).
std::vector<Node> buildSTR(const std::vector<Point>& points);
