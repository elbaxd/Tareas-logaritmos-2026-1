#include "query.hpp"

#include "io.hpp"

#include <cstdint>
#include <stdexcept>

namespace {

// Recorrido recursivo del arbol leyendo cada nodo desde disco
// No usa el vector en RAM utilizado durante la construccion, sino que lee el  archivo binario 
// serializado, por lo que cada acceso cuenta para g_disk_reads.
void queryRecursive(std::ifstream& in,
                    std::int32_t idx,
                    const MBR& query,
                    std::vector<Point>& out) {
    Node node;
    readNode(in, idx, node);

    if (isLeaf(node)) {
        for (std::int32_t i = 0; i < node.k; ++i) {
            // En hojas, x1 == x2 e y1 == y2 son las coordenadas del punto.
            const float x = node.entries[i].mbr.x1;
            const float y = node.entries[i].mbr.y1;
            if (contains(query, x, y)) {
                out.push_back(Point{x, y});
            }
        }
        return;
    }
    for (std::int32_t i = 0; i < node.k; ++i) {
        if (intersects(node.entries[i].mbr, query)) {
            queryRecursive(in, node.entries[i].child_idx, query, out);
        }
    }
}

} // namespace

std::vector<Point> queryRange(const std::string& tree_path, const MBR& query) {
    std::ifstream in(tree_path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("No se pudo abrir el arbol: " + tree_path);
    }
    return queryRange(in, query);
}

std::vector<Point> queryRange(std::ifstream& in, const MBR& query) {
    std::vector<Point> result;
    queryRecursive(in, 0, query, result);
    return result;
}
