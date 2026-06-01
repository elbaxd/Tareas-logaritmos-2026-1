#pragma once

#include "point.hpp"

#include <cstddef>
#include <cstdint>

// Tamanio del bloque en disco (igual al tamanio de un nodo).
constexpr int NODE_SIZE = 4096;

// Cantidad maxima de hijos por nodo. Calculado para que sizeof(Node) == 4096:
//   4 (k) + B * (16 + 4) (entries) + 12 (pad) = 4096  =>  B = 204.
constexpr int B = 204;

// Padding al final del nodo para alinear al tamanio del bloque.
constexpr int PAD_BYTES = 12;

// Par clave-valor de un nodo (16 + 4 = 20 bytes).
//   mbr:       MBR del hijo (o el punto, si la entrada es de hoja).
//   child_idx: indice del nodo hijo en el archivo (-1 si la entrada es hoja).
struct Entry {
    MBR mbr;
    int32_t child_idx;
};

// Nodo del R-tree. Tamanio total: 4096 bytes (un bloque de disco).
//   k:       cantidad actual de entradas validas (1 <= k <= B).
//   entries: arreglo de B entradas, las primeras k son validas.
//   pad:     relleno hasta completar 4096 bytes.
struct Node {
    int32_t k;
    Entry entries[B];
    char pad[PAD_BYTES];
};

// // Se espera que el Entry (par llave-valor) tenga 20 bytes, y que el tamaño del nodo 4096 bytes
// static_assert(sizeof(Entry) == 20, "Entry debe medir 20 bytes");
// static_assert(sizeof(Node) == NODE_SIZE, "Node debe medir 4096 bytes");

// True si el nodo es una hoja (sus hijos son puntos).
// Convencion: en una hoja, todas las entradas tienen child_idx == -1; basta
// con revisar la primera por ser todas del mismo tipo dentro de un nodo.
//  n: nodo a consultar.
//  ret: true si n es hoja.
bool isLeaf(const Node& n);

// MBR que envuelve a las primeras k entradas del nodo.
//  n: nodo cuyas entradas se envuelven.
//  ret: MBR de las k entradas de n.
MBR computeNodeMBR(const Node& n);

// MBR que envuelve a un rango contiguo de entradas.
//  entries: puntero al inicio del rango de entradas.
//  count: cantidad de entradas en el rango.
//  ret: MBR que envuelve las count entradas.
MBR computeRangeMBR(const Entry* entries, std::size_t count);
