#include "io.hpp"

#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>

std::size_t g_disk_reads = 0;

// Documentado en io.hpp
void resetReadCounter() {
    g_disk_reads = 0;
}

// Documentado en io.hpp
std::vector<Point> readPointsBin(const std::string& path, long long N) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("No se pudo abrir el archivo de puntos: " + path);
    }
    const std::uintmax_t total_bytes = std::filesystem::file_size(path);
    const long long total_points = static_cast<long long>(total_bytes / sizeof(Point));
    const long long to_read = (N <= 0 || N > total_points) ? total_points : N;
    std::vector<Point> points(static_cast<std::size_t>(to_read));
    in.read(reinterpret_cast<char*>(points.data()),
            static_cast<std::streamsize>(to_read) * sizeof(Point));
    if (!in) {
        throw std::runtime_error("Error leyendo puntos desde: " + path);
    }
    return points;
}

// Documentado en io.hpp
void writeTreeBin(const std::string& path, const std::vector<Node>& tree) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) {
        throw std::runtime_error("No se pudo abrir el archivo de salida: " + path);
    }
    out.write(reinterpret_cast<const char*>(tree.data()),
              static_cast<std::streamsize>(tree.size()) * sizeof(Node));
    if (!out) {
        throw std::runtime_error("Error escribiendo el arbol a: " + path);
    }
}

// Documentado en io.hpp
void readNode(std::ifstream& in, std::int32_t idx, Node& out) {
    in.seekg(static_cast<std::streamoff>(idx) * NODE_SIZE);
    in.read(reinterpret_cast<char*>(&out), NODE_SIZE);
    ++g_disk_reads;
    if (!in) {
        throw std::runtime_error("Error leyendo nodo " + std::to_string(idx));
    }
}
