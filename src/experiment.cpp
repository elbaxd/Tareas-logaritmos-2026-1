#include "bulk_loader.hpp"
#include "io.hpp"
#include "point.hpp"
#include "query.hpp"
#include "rtree_node.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

// Mide milisegundos entre dos tiempos.
//  start, end: timestamps del reloj.
//  ret: diferencia en milisegundos.
double elapsedMs(Clock::time_point start, Clock::time_point end) {
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// Empareja un metodo de bulk-loading con su funcion de construccion.
//  name: nombre del metodo.
//  build: puntero a la funcion que construye el R-tree.
struct Method {
    const char* name;
    std::vector<Node> (*build)(const std::vector<Point>&);
};

const Method METHODS[] = {
    {"nearest-x", buildNearestX},
    {"str",       buildSTR},
};

// Referencia a un dataset, usada para iterar sobre los conjuntos
// "random" y "europa" sin copiar los vectores grandes.
//  name: nombre del dataset.
//  points: puntero al vector de puntos
struct DatasetRef {
    const char* name;
    const std::vector<Point>* points;
};

// Genera (count) cuadrados aleatorios de lado s en [0,1] x [0,1].
//  s: largo del lado del cuadrado.
//  count: cantidad de cuadrados a generar.
//  rng: generador random
//  ret: vector de MBRs cuadrados
std::vector<MBR> generateQueries(double s, std::size_t count, std::mt19937& rng) {
    const double upper = std::max(0.0, 1.0 - s);
    std::uniform_real_distribution<double> dist(0.0, upper);
    std::vector<MBR> queries;
    queries.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        const double x1 = dist(rng);
        const double y1 = dist(rng);
        queries.push_back(MBR{
            static_cast<float>(x1),
            static_cast<float>(x1 + s),
            static_cast<float>(y1),
            static_cast<float>(y1 + s)
        });
    }
    return queries;
}

// Calcula media y desviacion estandar (poblacional) de un vector.
//  v: vector de valores.
//  ret: el par (media, desviacion estandar).
std::pair<double, double> meanStd(const std::vector<double>& v) {
    if (v.empty()) return {0.0, 0.0};
    double sum = 0;
    for (double x : v) sum += x;
    const double mean = sum / static_cast<double>(v.size());
    double sq = 0;
    for (double x : v) sq += (x - mean) * (x - mean);
    const double stdv = std::sqrt(sq / static_cast<double>(v.size()));
    return {mean, stdv};
}

} // namespace

// Subcomando 'experiment': itera N en {2^15, ..., 2^24}, mide tiempos de construccion para los 4 (dataset, metodo) 
// y guarda los arboles finales (N=2^24) a disco. 
// 
// Luego corre 100 consultas para cada s en {0.0025, 0.005, 0.01, 0.025, 0.05} sobre cada uno de los arboles
// guardados, midiendo lecturas y puntos encontrados.
// 
//  random_path: archivo binario del dataset aleatorio.
//  europa_path: archivo binario del dataset Europa.
//  out_dir: directorio donde se escribiran resultados y arboles.
//  ret: 0 en exito.
int runExperiment(const std::string& random_path,
                  const std::string& europa_path,
                  const std::string& out_dir) {
    namespace fs = std::filesystem;
    fs::create_directories(out_dir);

    std::cerr << "Leyendo dataset aleatorio desde " << random_path << "\n";
    const auto random_points = readPointsBin(random_path, 0);
    std::cerr << "  -> " << random_points.size() << " puntos\n";

    std::cerr << "Leyendo dataset Europa desde " << europa_path << "\n";
    const auto europa_points = readPointsBin(europa_path, 0);
    std::cerr << "  -> " << europa_points.size() << " puntos\n";

    const DatasetRef datasets[] = {
        {"random", &random_points},
        {"europa", &europa_points}
    };

    // Tiempos de construccion
    std::ofstream build_csv(out_dir + "/build_times.csv");
    build_csv << "dataset,method,N,time_ms\n";

    struct FinalTree { std::string label; std::string path; };
    std::vector<FinalTree> finals;

    for (int exp = 15; exp <= 24; ++exp) {
        const long long N = 1LL << exp;
        for (const auto& d : datasets) {
            if (N > static_cast<long long>(d.points->size())) {
                std::cerr << d.name << ": N=" << N
                          << " excede el tamano del dataset, se omite\n";
                continue;
            }
            for (const auto& m : METHODS) {
                std::vector<Point> sub(d.points->begin(),
                                       d.points->begin() + N);
                const auto t0 = Clock::now();
                auto tree = m.build(sub);
                const auto t1 = Clock::now();
                const double ms = elapsedMs(t0, t1);
                build_csv << d.name << "," << m.name << ","
                          << N << "," << ms << "\n";
                std::cerr << d.name << "/" << m.name
                          << " N=" << N
                          << " -> " << ms << " ms ("
                          << tree.size() << " nodos)\n";
                if (exp == 24) {
                    const std::string label =
                        std::string(d.name) + "_" + m.name;
                    const std::string tree_path =
                        out_dir + "/tree_" + label + ".bin";
                    writeTreeBin(tree_path, tree);
                    finals.push_back({label, tree_path});
                }
            }
        }
    }
    build_csv.close();

    // Consultas en arboles N=2^24
    std::ofstream query_csv(out_dir + "/query_stats.csv");
    query_csv << "tree,s,reads_avg,reads_std,points_avg,points_std\n";

    const double S_VALUES[] = {0.0025, 0.005, 0.01, 0.025, 0.05};
    std::mt19937 rng(42);

    for (const auto& f : finals) {
        std::ifstream in(f.path, std::ios::binary);
        if (!in) {
            std::cerr << "No se pudo abrir " << f.path << " para consultas\n";
            continue;
        }
        for (double s : S_VALUES) {
            const auto queries = generateQueries(s, 100, rng);
            std::vector<double> reads_v;
            std::vector<double> points_v;
            reads_v.reserve(queries.size());
            points_v.reserve(queries.size());
            for (const auto& q : queries) {
                resetReadCounter();
                const auto pts = queryRange(in, q);
                reads_v.push_back(static_cast<double>(g_disk_reads));
                points_v.push_back(static_cast<double>(pts.size()));
            }
            const auto [r_mean, r_std] = meanStd(reads_v);
            const auto [p_mean, p_std] = meanStd(points_v);
            query_csv << f.label << "," << s << ","
                      << r_mean << "," << r_std << ","
                      << p_mean << "," << p_std << "\n";
            std::cerr << f.label << " s=" << s
                      << " reads=" << r_mean << "+-" << r_std
                      << " points=" << p_mean << "+-" << p_std << "\n";
        }
    }
    query_csv.close();

    std::cerr << "Experimentacion completa. Resultados en " << out_dir << "\n";
    return 0;
}

// Subcomando 'bonus': construye un R-tree (con STR) sobre el dataset europa_bonus.bin (sin normalizar) y ejecuta una
// consulta sobre el rectangulo entregado. 
// Escribe los puntos encontrados a un CSV (x, y).
// 
//  path: archivo binario del dataset bonus (coordenadas reales).
//  query: rectangulo de consulta en coordenadas reales.
//  out_path: ruta del archivo CSV de salida.
//  ret: 0 en exito.
int runBonus(const std::string& path,
             const MBR& query,
             const std::string& out_path) {
    std::cerr << "Leyendo dataset bonus desde " << path << "\n";
    const auto points = readPointsBin(path, 0);
    std::cerr << "  -> " << points.size() << " puntos\n";

    std::cerr << "Construyendo R-tree (STR)...\n";
    const auto t0 = Clock::now();
    auto tree = buildSTR(points);
    const auto t1 = Clock::now();
    std::cerr << "  -> " << elapsedMs(t0, t1)
              << " ms (" << tree.size() << " nodos)\n";

    const std::string tree_path = out_path + ".tree";
    writeTreeBin(tree_path, tree);

    resetReadCounter();
    const auto results = queryRange(tree_path, query);

    std::ofstream csv(out_path);
    csv << "x,y\n";
    for (const auto& p : results) {
        csv << p.x << "," << p.y << "\n";
    }
    std::cerr << "Encontrados " << results.size()
              << " puntos con " << g_disk_reads
              << " lecturas. CSV: " << out_path << "\n";
    return 0;
}
