#include "bulk_loader.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

// Comparador por suma de extremos en X. Equivalente monotonico al centro
// (x1 + x2) / 2 pero sin division: ordena identicamente y evita errores
// numericos triviales.
bool cmpCenterX(const Entry& a, const Entry& b) {
    return (a.mbr.x1 + a.mbr.x2) < (b.mbr.x1 + b.mbr.x2);
}

// Comparador por suma de extremos en Y.
bool cmpCenterY(const Entry& a, const Entry& b) {
    return (a.mbr.y1 + a.mbr.y2) < (b.mbr.y1 + b.mbr.y2);
}

// Convierte una lista de puntos en sus correspondientes entradas hoja.
// Cada entrada queda con MBR puntual (x1==x2, y1==y2) y child_idx == -1.
std::vector<Entry> pointsToEntries(const std::vector<Point>& points) {
    std::vector<Entry> entries;
    entries.reserve(points.size());
    for (const auto& p : points) {
        entries.push_back(Entry{pointMBR(p), -1});
    }
    return entries;
}

// Llena un Node con un rango de entradas y retorna el MBR que las envuelve.
//   src:   vector fuente de entradas.
//   start: indice inicial (inclusive) en src.
//   count: cantidad de entradas a copiar (1 <= count <= B).
//   dst:   nodo destino (sus entradas se sobreescriben).
//   ret:   MBR que envuelve a las entradas insertadas.
MBR packGroup(const std::vector<Entry>& src, std::size_t start, std::size_t count, Node& dst) {
    dst.k = static_cast<std::int32_t>(count);
    for (std::size_t i = 0; i < count; ++i) {
        dst.entries[i] = src[start + i];
    }
    return computeRangeMBR(dst.entries, count);
}

// Escribe la raiz en la posicion 0 del arbol con todas las entradas dadas.
// Asume que entries.size() <= B (caso base de la recursion).
void writeRoot(std::vector<Node>& tree, const std::vector<Entry>& entries) {
    Node& root = tree[0];
    root.k = static_cast<std::int32_t>(entries.size());
    for (std::size_t i = 0; i < entries.size(); ++i) {
        root.entries[i] = entries[i];
    }
}

// Recursion Nearest-X: ordena por X, agrupa de a B, empuja los nodos hijos al
// arbol y recurre con las entradas padre. Cuando todo cabe en un nodo, la
// raiz se escribe en la posicion 0.
void nearestXRecursive(std::vector<Node>& tree, std::vector<Entry>& entries) {
    if (entries.size() <= static_cast<std::size_t>(B)) {
        writeRoot(tree, entries);
        return;
    }
    std::sort(entries.begin(), entries.end(), cmpCenterX);

    std::vector<Entry> parents;
    parents.reserve((entries.size() + B - 1) / B);

    for (std::size_t i = 0; i < entries.size(); i += B) {
        const std::size_t count = std::min(static_cast<std::size_t>(B), entries.size() - i);
        Node node{};
        const MBR mbr = packGroup(entries, i, count, node);
        const auto idx = static_cast<std::int32_t>(tree.size());
        tree.push_back(node);
        parents.push_back(Entry{mbr, idx});
    }
    nearestXRecursive(tree, parents);
}

// Recursion STR: en cada nivel hace slicing 2D antes de empacar. Ordena por
// X, parte en S = ceil(sqrt(n/B)) slices verticales, ordena cada slice por Y
// y lo parte en grupos de tamanio B. Total de grupos ~= n/B.
void strRecursive(std::vector<Node>& tree, std::vector<Entry>& entries) {
    if (entries.size() <= static_cast<std::size_t>(B)) {
        writeRoot(tree, entries);
        return;
    }
    const std::size_t n = entries.size();
    const std::size_t total_groups = (n + B - 1) / B;
    std::size_t S = static_cast<std::size_t>(
        std::ceil(std::sqrt(static_cast<double>(total_groups))));
    if (S < 1) S = 1;

    std::sort(entries.begin(), entries.end(), cmpCenterX);
    const std::size_t slice_size = (n + S - 1) / S;

    std::vector<Entry> parents;
    parents.reserve(total_groups + S);

    for (std::size_t s = 0; s < S; ++s) {
        const std::size_t slice_start = s * slice_size;
        if (slice_start >= n) break;
        const std::size_t slice_end = std::min(slice_start + slice_size, n);

        // Ordenar el slice por Y.
        std::sort(entries.begin() + static_cast<std::ptrdiff_t>(slice_start),
                  entries.begin() + static_cast<std::ptrdiff_t>(slice_end),
                  cmpCenterY);

        // Partir el slice en grupos de tamanio B.
        for (std::size_t i = slice_start; i < slice_end; i += B) {
            const std::size_t count = std::min(static_cast<std::size_t>(B),
                                               slice_end - i);
            Node node{};
            const MBR mbr = packGroup(entries, i, count, node);
            const auto idx = static_cast<std::int32_t>(tree.size());
            tree.push_back(node);
            parents.push_back(Entry{mbr, idx});
        }
    }
    strRecursive(tree, parents);
}

// Estimacion (con holgura) del numero de nodos del arbol para reservar
// capacidad de antemano y evitar reallocs caras.
//   n: cantidad inicial de entradas hoja.
//   ret: cota superior aproximada del numero de nodos del arbol.
std::size_t estimateTreeSize(std::size_t n) {
    if (n <= static_cast<std::size_t>(B)) return 1;
    // Suma de la serie geometrica 1 + 1/B + 1/B^2 + ... acotada por n/(B-1).
    return n / (static_cast<std::size_t>(B) - 1) + static_cast<std::size_t>(B);
}

} // namespace

std::vector<Node> buildNearestX(const std::vector<Point>& points) {
    if (points.empty()) {
        throw std::invalid_argument("buildNearestX: lista de puntos vacia");
    }
    std::vector<Node> tree;
    tree.reserve(estimateTreeSize(points.size()));
    tree.push_back(Node{}); // posicion 0 reservada para la raiz
    auto entries = pointsToEntries(points);
    nearestXRecursive(tree, entries);
    return tree;
}

std::vector<Node> buildSTR(const std::vector<Point>& points) {
    if (points.empty()) {
        throw std::invalid_argument("buildSTR: lista de puntos vacia");
    }
    std::vector<Node> tree;
    tree.reserve(estimateTreeSize(points.size()));
    tree.push_back(Node{}); // posicion 0 reservada para la raiz
    auto entries = pointsToEntries(points);
    strRecursive(tree, entries);
    return tree;
}
