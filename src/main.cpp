#include "bulk_loader.hpp"
#include "io.hpp"
#include "point.hpp"
#include "query.hpp"
#include "rtree_node.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Funciones implementadas en experiment.cpp
int runExperiment(const std::string& random_path,
                  const std::string& europa_path,
                  const std::string& out_dir);
int runBonus(const std::string& path,
             const MBR& query,
             const std::string& out_path);

namespace {

// Imprime las instrucciones de uso
void usage() {
    std::cerr <<
        "Uso:\n"
        "  rtree build      --method {nearest-x|str} --in <points.bin> "
        "--N <int> --out <tree.bin>\n"
        "  rtree query      --tree <tree.bin> --rect x1 x2 y1 y2\n"
        "  rtree experiment --random <random.bin> --europa <europa.bin> "
        "--out-dir <dir>\n"
        "  rtree bonus      --in <bonus.bin> --rect x1 x2 y1 y2 "
        "--out <results.csv>\n";
}

// Busca el argumento --flag y retorna su valor (o nullptr si no esta)
//  argc, argv: argumentos del comando
//  flag: nombre del flag a buscar (considera --)
//  ret: valor del flag, o nullptr si no se encuentra
const char* getOpt(int argc, char** argv, const std::string& flag) {
    for (int i = 1; i < argc - 1; ++i) {
        if (flag == argv[i]) return argv[i + 1];
    }
    return nullptr;
}

// Extrae los 4 floats de --rect x1 x2 y1 y2, o retorna false si no esta.
//  argc, argv: argumentos del comando
//  mbr: destino del rectangulo parseado (se sobreescribe!
//  ret: true si se encontro  --rect con 4 valores numericos válidos
bool getRect(int argc, char** argv, MBR& mbr) {
    for (int i = 1; i < argc; ++i) {
        if (std::string("--rect") == argv[i] && i + 4 < argc) {
            mbr.x1 = std::stof(argv[i + 1]);
            mbr.x2 = std::stof(argv[i + 2]);
            mbr.y1 = std::stof(argv[i + 3]);
            mbr.y2 = std::stof(argv[i + 4]);
            return true;
        }
    }
    return false;
}

// Subcomando build: construye un R-tree desde un archivo de puntos y lo guarda a disco
// Lee los flags --method, --in, --N, --out de argv
// 
//  argc, argv: argumentos de linea de comandos.
//  ret: 0 en exito, 1 en error de uso
int cmdBuild(int argc, char** argv) {
    const char* method = getOpt(argc, argv, "--method");
    const char* in_path = getOpt(argc, argv, "--in");
    const char* n_str = getOpt(argc, argv, "--N");
    const char* out_path = getOpt(argc, argv, "--out");
    if (!method || !in_path || !out_path) { usage(); return 1; }
    const long long N = n_str ? std::stoll(n_str) : 0;

    const auto points = readPointsBin(in_path, N);
    const auto t0 = std::chrono::steady_clock::now();
    std::vector<Node> tree;
    if (std::string(method) == "nearest-x") {
        tree = buildNearestX(points);
    } else if (std::string(method) == "str") {
        tree = buildSTR(points);
    } else {
        std::cerr << "Metodo desconocido: " << method << "\n";
        return 1;
    }
    const auto t1 = std::chrono::steady_clock::now();
    const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    writeTreeBin(out_path, tree);
    std::cerr << "Construido " << method
              << " con " << points.size() << " puntos en "
              << ms << " ms (" << tree.size() << " nodos). "
              << "Archivo: " << out_path << "\n";
    return 0;
}

// Subcomando query: consulta puntos contenidos en un rectangulo sobre un R-tree serializado en disco
//  Lee los flags --tree, --rect de argv.
// 
//  argc, argv: argumentos del comando
//  ret: 0 en exito, 1 en error de uso
int cmdQuery(int argc, char** argv) {
    const char* tree_path = getOpt(argc, argv, "--tree");
    if (!tree_path) { usage(); return 1; }
    MBR rect{};
    if (!getRect(argc, argv, rect)) { usage(); return 1; }
    resetReadCounter();
    const auto results = queryRange(tree_path, rect);
    for (const auto& p : results) {
        std::cout << p.x << " " << p.y << "\n";
    }
    std::cerr << "Puntos encontrados: " << results.size()
              << " | Lecturas: " << g_disk_reads << "\n";
    return 0;
}

// Subcomando experiment: ejecuta el experimento completo de construccion y consultas sobre ambos datasets.
//  Lee los flags --random, --europa, --out-dir.
// 
//  argc, argv: argumentos del comanodo
//  ret: 0 en exito, 1 en error de uso.
int cmdExperiment(int argc, char** argv) {
    const char* rnd = getOpt(argc, argv, "--random");
    const char* eur = getOpt(argc, argv, "--europa");
    const char* out = getOpt(argc, argv, "--out-dir");
    if (!rnd || !eur || !out) { usage(); return 1; }
    return runExperiment(rnd, eur, out);
}

// Subcomando bonus: consulta el dataset bonus (coordenadas reales no normalizadas) y escribe los puntos encontrados a un CSV. 
// Lee los flags --in, --rect, --out de argv.
// 
//  argc, argv: argumentos delcomando
//  ret: 0 en exito, 1 en error de uso
int cmdBonus(int argc, char** argv) {
    const char* in = getOpt(argc, argv, "--in");
    const char* out = getOpt(argc, argv, "--out");
    if (!in || !out) { usage(); return 1; }
    MBR rect{};
    if (!getRect(argc, argv, rect)) { usage(); return 1; }
    return runBonus(in, rect, out);
}

} // namespace

// Punto de entrada del programa. Actua como despachador al comando que se invoque (build, query, experiment, bonus) segun argv[1]
// 
//  argc, argv: argumentos del comando
//  ret: 0 en exito, 1 en error de uso, 2 en error de ejecucion
int main(int argc, char** argv) {
    if (argc < 2) { usage(); return 1; }
    const std::string cmd = argv[1];
    try {
        if (cmd == "build")      return cmdBuild(argc, argv);
        if (cmd == "query")      return cmdQuery(argc, argv);
        if (cmd == "experiment") return cmdExperiment(argc, argv);
        if (cmd == "bonus")      return cmdBonus(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }
    usage();
    return 1;
}
