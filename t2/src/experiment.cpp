#include "experiment.hpp"
#include "dataset.hpp"
#include "avl.hpp"
#include "splay.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

using Clock = std::chrono::steady_clock;
using HiResClock = std::chrono::high_resolution_clock;

static double elapsedMs(Clock::time_point start, Clock::time_point end) {
    return std::chrono::duration<double, std::milli>(end - start).count();
}

static double elapsedNs(HiResClock::time_point start, HiResClock::time_point end) {
    return std::chrono::duration<double, std::nano>(end - start).count();
}

static std::ofstream openCsv(const std::string& outDir, const std::string& name) {
    std::string path = outDir + "/" + name;
    std::ofstream f(path);
    if (!f) std::cerr << "Warning: cannot open " << path << "\n";
    return f;
}

void runBaseScenarios(const std::string& outDir, int C, double lambda, uint64_t seed) {
    auto fRes = openCsv(outDir, "base_results.csv");
    fRes << "config,N,avl_insert_ms,avl_search_ms,splay_insert_ms,splay_search_ms\n";

    auto fPerSearch = openCsv(outDir, "base_per_search.csv");
    fPerSearch << "config,N,step,avl_time_ns,splay_time_ns\n";

    const uint32_t baseN[] = {1024, 2048, 4096, 8192, 16384};

    for (uint32_t N : baseN) {
        std::cout << "  Base N=" << N << std::endl;

        auto dataset = generateUniform(N, seed);
        auto sortedDataset(dataset);
        std::sort(sortedDataset.begin(), sortedDataset.end());

        std::vector<double> pmf;
        if (C != 0)
            pmf = exponentialPMF(N, lambda);

        for (char config : {'A', 'B', 'C', 'D'}) {
            std::mt19937 rng(seed + static_cast<uint64_t>(config) * 37 + N * 13);

            const auto& dataForInsert = (config == 'C' || config == 'D') ? sortedDataset : dataset;

            AVL avl;
            auto t0 = Clock::now();
            for (auto key : dataForInsert) avl.insert(key);
            auto t1 = Clock::now();
            double avlInsertMs = elapsedMs(t0, t1);

            SplayTree splay;
            t0 = Clock::now();
            for (auto key : dataForInsert) splay.insert(key);
            t1 = Clock::now();
            double splayInsertMs = elapsedMs(t0, t1);

            uint64_t M = static_cast<uint64_t>(10) * C * N;
            std::vector<uint32_t> searchKeys(N);

            if (config == 'A' || config == 'C') {
                std::uniform_int_distribution<size_t> pickIdx(0, N - 1);
                for (uint32_t i = 0; i < N; ++i)
                    searchKeys[i] = dataset[pickIdx(rng)];
            } else {
                std::discrete_distribution<int> dist(pmf.begin(), pmf.end());
                for (uint32_t i = 0; i < N; ++i)
                    searchKeys[i] = sortedDataset[dist(rng)];
            }

            std::vector<double> avlPerSearchNs(M);
            std::vector<double> splayPerSearchNs(M);

            t0 = Clock::now();
            for (uint64_t i = 0; i < M; ++i) {
                uint32_t key = searchKeys[i % N];
                auto tb = HiResClock::now();
                avl.search(key);
                auto ta = HiResClock::now();
                avlPerSearchNs[i] = elapsedNs(tb, ta);
            }
            t1 = Clock::now();
            double avlSearchMs = elapsedMs(t0, t1);

            t0 = Clock::now();
            for (uint64_t i = 0; i < M; ++i) {
                uint32_t key = searchKeys[i % N];
                auto tb = HiResClock::now();
                splay.search(key);
                auto ta = HiResClock::now();
                splayPerSearchNs[i] = elapsedNs(tb, ta);
            }
            t1 = Clock::now();
            double splaySearchMs = elapsedMs(t0, t1);

            fRes << config << "," << N << ","
                 << avlInsertMs << "," << avlSearchMs << ","
                 << splayInsertMs << "," << splaySearchMs << "\n";

            for (uint64_t i = 0; i < M; ++i) {
                fPerSearch << config << "," << N << "," << i << ","
                           << avlPerSearchNs[i] << "," << splayPerSearchNs[i] << "\n";
            }

            avl.clear();
            splay.clear();
        }
    }
}

void runSequentialAccess(const std::string& outDir, uint64_t seed) {
    std::cout << "  Sequential Access Theorem (N=2^25)" << std::endl;

    auto f = openCsv(outDir, "seq_results.csv");
    f << "m,avl_search_ms,splay_search_ms\n";

    const uint64_t N = 33554432ULL;
    auto dataset = generateUniform(N, seed);
    auto sortedDataset(dataset);
    std::sort(sortedDataset.begin(), sortedDataset.end());

    AVL avl;
    SplayTree splay;
    for (auto key : dataset) { avl.insert(key); splay.insert(key); }
    std::cout << "    Trees built." << std::endl;

    for (int k = 1; k <= 10; ++k) {
        uint64_t m = k * N / 100;
        std::cout << "    m=" << m << std::endl;

        auto t0 = Clock::now();
        for (uint64_t i = 0; i < m; ++i) avl.search(sortedDataset[i]);
        auto t1 = Clock::now();
        double avlMs = elapsedMs(t0, t1);

        t0 = Clock::now();
        for (uint64_t i = 0; i < m; ++i) splay.search(sortedDataset[i]);
        t1 = Clock::now();
        double splayMs = elapsedMs(t0, t1);

        f << m << "," << avlMs << "," << splayMs << "\n";
    }
}

void runWorkingSet(const std::string& outDir, int C, uint64_t seed) {
    std::cout << "  Working Set Theorem (N=2^25)" << std::endl;

    auto f = openCsv(outDir, "ws_results.csv");
    f << "W,avl_search_ms,splay_search_ms\n";

    const uint64_t N = 33554432ULL;
    auto dataset = generateUniform(N, seed);

    AVL avl;
    SplayTree splay;
    for (auto key : dataset) { avl.insert(key); splay.insert(key); }
    std::cout << "    Trees built." << std::endl;

    const uint64_t W_values[] = {10, 100, 1000, 10000, 100000, 1000000};
    uint64_t M = static_cast<uint64_t>(10) * C * N;

    for (uint64_t W : W_values) {
        std::cout << "    W=" << W << std::endl;

        std::mt19937 rng(seed + W);

        std::vector<size_t> indices(N);
        for (size_t i = 0; i < N; ++i) indices[i] = i;
        std::shuffle(indices.begin(), indices.end(), rng);

        std::vector<uint32_t> workingSet(W);
        for (uint64_t i = 0; i < W; ++i) workingSet[i] = dataset[indices[i]];

        std::uniform_int_distribution<uint64_t> pick(0, W - 1);
        std::vector<uint32_t> searchKeys(M);
        for (uint64_t i = 0; i < M; ++i) searchKeys[i] = workingSet[pick(rng)];

        auto t0 = Clock::now();
        for (auto key : searchKeys) avl.search(key);
        auto t1 = Clock::now();
        double avlMs = elapsedMs(t0, t1);

        t0 = Clock::now();
        for (auto key : searchKeys) splay.search(key);
        t1 = Clock::now();
        double splayMs = elapsedMs(t0, t1);

        f << W << "," << avlMs << "," << splayMs << "\n";
    }

    avl.clear();
    splay.clear();
}

static void getPreorder(Node* node, std::vector<uint32_t>& seq) {
    if (!node) return;
    seq.push_back(node->key);
    getPreorder(node->left, seq);
    getPreorder(node->right, seq);
}

void runTraversal(const std::string& outDir, uint64_t seed) {
    std::cout << "  Traversal Conjecture (N=2^25)" << std::endl;

    auto f = openCsv(outDir, "traversal_results.csv");
    f << "step,search_time_ms\n";

    const uint64_t N = 33554432ULL;
    auto dataset = generateUniform(N, seed);

    SplayTree T1;
    for (auto key : dataset) T1.insert(key);

    std::mt19937 rng(seed + 1);
    std::shuffle(dataset.begin(), dataset.end(), rng);

    SplayTree T2;
    for (auto key : dataset) T2.insert(key);
    std::cout << "    Both trees built." << std::endl;

    std::vector<uint32_t> seq;
    seq.reserve(N);
    getPreorder(T1.getRoot(), seq);

    double totalMs = 0.0;
    uint64_t step = 0;
    for (auto key : seq) {
        auto t0 = Clock::now();
        T2.search(key);
        auto t1 = Clock::now();
        double ms = elapsedMs(t0, t1);
        totalMs += ms;
        f << step << "," << ms << "\n";
        ++step;
    }

    std::cout << "    Total: " << totalMs << " ms" << std::endl;
    T1.clear();
    T2.clear();
}
