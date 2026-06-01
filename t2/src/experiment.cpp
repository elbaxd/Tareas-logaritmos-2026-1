#include "experiment.hpp"
#include "dataset.hpp"
#include "avl.hpp"
#include "splay.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
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

static double since(Clock::time_point start) {
    return elapsedMs(start, Clock::now());
}

static std::ofstream openCsv(const std::string& outDir, const std::string& name) {
    std::string path = outDir + "/" + name;
    std::ofstream f(path);
    if (!f) std::cerr << "Warning: cannot open " << path << "\n";
    return f;
}

void runBaseScenarios(const std::string& outDir, int C, double lambda, uint64_t seed) {
    std::cout << "[base] Starting base scenarios (C=" << C << ", lambda=" << lambda
              << ", seed=" << seed << ")" << std::endl;

    auto fRes = openCsv(outDir, "base_results.csv");
    fRes << "config,N,avl_insert_ms,avl_search_ms,splay_insert_ms,splay_search_ms\n";

    auto fPerSearch = openCsv(outDir, "base_per_search.csv");
    fPerSearch << "config,N,step,avl_time_ns,splay_time_ns\n";

    const uint32_t baseN[] = {1024, 2048, 4096, 8192, 16384};

    for (uint32_t N : baseN) {
        auto tN = Clock::now();
        std::cout << "[base]   N=" << N << std::flush;

        auto dataset = generateUniform(N, seed);
        auto sortedDataset(dataset);
        std::sort(sortedDataset.begin(), sortedDataset.end());
        std::cout << " | dataset gen " << since(tN) << "ms" << std::flush;

        std::vector<double> pmf;
        if (C != 0)
            pmf = exponentialPMF(N, lambda);

        for (char config : {'A', 'B', 'C', 'D'}) {
            std::cout << " | " << config << ":" << std::flush;
            auto tCfg = Clock::now();

            std::mt19937 rng(seed + static_cast<uint64_t>(config) * 37 + N * 13);

            const auto& dataForInsert = (config == 'C' || config == 'D') ? sortedDataset : dataset;

            AVL avl;
            auto t0 = Clock::now();
            for (auto key : dataForInsert) avl.insert(key);
            auto t1 = Clock::now();
            double avlInsertMs = elapsedMs(t0, t1);
            std::cout << "i" << std::flush;

            SplayTree splay;
            t0 = Clock::now();
            for (auto key : dataForInsert) splay.insert(key);
            t1 = Clock::now();
            double splayInsertMs = elapsedMs(t0, t1);
            std::cout << "I" << std::flush;

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
            std::cout << "k" << std::flush;

            std::vector<double> avlPerSearchNs(M);
            std::vector<double> splayPerSearchNs(M);

            uint64_t pctStep = M / 10;
            if (pctStep == 0) pctStep = 1;

            t0 = Clock::now();
            for (uint64_t i = 0; i < M; ++i) {
                uint32_t key = searchKeys[i % N];
                auto tb = HiResClock::now();
                avl.search(key);
                auto ta = HiResClock::now();
                avlPerSearchNs[i] = elapsedNs(tb, ta);
                if ((i + 1) % pctStep == 0) std::cout << "." << std::flush;
            }
            t1 = Clock::now();
            double avlSearchMs = elapsedMs(t0, t1);
            std::cout << "s" << std::flush;

            t0 = Clock::now();
            for (uint64_t i = 0; i < M; ++i) {
                uint32_t key = searchKeys[i % N];
                auto tb = HiResClock::now();
                splay.search(key);
                auto ta = HiResClock::now();
                splayPerSearchNs[i] = elapsedNs(tb, ta);
                if ((i + 1) % pctStep == 0) std::cout << "," << std::flush;
            }
            t1 = Clock::now();
            double splaySearchMs = elapsedMs(t0, t1);
            std::cout << "S" << std::flush;

            fRes << config << "," << N << ","
                 << avlInsertMs << "," << avlSearchMs << ","
                 << splayInsertMs << "," << splaySearchMs << "\n";

            for (uint64_t i = 0; i < M; ++i) {
                fPerSearch << config << "," << N << "," << i << ","
                           << avlPerSearchNs[i] << "," << splayPerSearchNs[i] << "\n";
            }
            std::cout << "w" << std::flush;

            avl.clear();
            splay.clear();

            std::cout << " (" << since(tCfg) << "ms)" << std::flush;
        }
        std::cout << " | total " << since(tN) << "ms" << std::endl;
    }
    std::cout << "[base] Done." << std::endl;
}

void runSequentialAccess(const std::string& outDir, uint64_t seed) {
    std::cout << "[seq] Sequential Access Theorem (N=2^25)" << std::endl;

    auto f = openCsv(outDir, "seq_results.csv");
    f << "m,avl_search_ms,splay_search_ms\n";

    const uint64_t N = 33554432ULL;

    auto tStart = Clock::now();
    std::cout << "[seq]   Generating dataset..." << std::flush;
    auto dataset = generateUniform(N, seed);
    std::cout << " " << since(tStart) << "ms" << std::endl;

    auto tSort = Clock::now();
    std::cout << "[seq]   Sorting dataset..." << std::flush;
    auto sortedDataset(dataset);
    std::sort(sortedDataset.begin(), sortedDataset.end());
    std::cout << " " << since(tSort) << "ms" << std::endl;

    auto tBuild = Clock::now();
    std::cout << "[seq]   Building AVL..." << std::flush;
    AVL avl;
    for (auto key : dataset) avl.insert(key);
    std::cout << " " << since(tBuild) << "ms" << std::endl;

    auto tSplayBuild = Clock::now();
    std::cout << "[seq]   Building Splay..." << std::flush;
    SplayTree splay;
    for (auto key : dataset) splay.insert(key);
    std::cout << " " << since(tSplayBuild) << "ms" << std::endl;
    std::cout << "[seq]   Trees built. Total build: " << since(tStart) << "ms" << std::endl;

    for (int k = 1; k <= 10; ++k) {
        uint64_t m = k * N / 100;
        std::cout << "[seq]     m=" << m << " (" << k << "/10)" << std::flush;

        auto t0 = Clock::now();
        for (uint64_t i = 0; i < m; ++i) avl.search(sortedDataset[i]);
        auto t1 = Clock::now();
        double avlMs = elapsedMs(t0, t1);
        std::cout << " | AVL " << avlMs << "ms" << std::flush;

        t0 = Clock::now();
        for (uint64_t i = 0; i < m; ++i) splay.search(sortedDataset[i]);
        t1 = Clock::now();
        double splayMs = elapsedMs(t0, t1);
        std::cout << " | Splay " << splayMs << "ms" << std::endl;

        f << m << "," << avlMs << "," << splayMs << "\n";
        f.flush();
    }
    avl.clear();
    splay.clear();
    std::cout << "[seq] Done. Total: " << since(tStart) << "ms" << std::endl;
}

void runWorkingSet(const std::string& outDir, int C, uint64_t seed) {
    std::cout << "[ws] Working Set Theorem (N=2^25, C=" << C << ")" << std::endl;

    auto f = openCsv(outDir, "ws_results.csv");
    f << "W,avl_search_ms,splay_search_ms\n";

    const uint64_t N = 33554432ULL;

    auto tStart = Clock::now();
    std::cout << "[ws]   Generating dataset..." << std::flush;
    auto dataset = generateUniform(N, seed);
    std::cout << " " << since(tStart) << "ms" << std::endl;

    auto tBuild = Clock::now();
    std::cout << "[ws]   Building AVL..." << std::flush;
    AVL avl;
    for (auto key : dataset) avl.insert(key);
    std::cout << " " << since(tBuild) << "ms" << std::endl;

    auto tSplayBuild = Clock::now();
    std::cout << "[ws]   Building Splay..." << std::flush;
    SplayTree splay;
    for (auto key : dataset) splay.insert(key);
    std::cout << " " << since(tSplayBuild) << "ms" << std::endl;
    std::cout << "[ws]   Trees built. Total build: " << since(tStart) << "ms" << std::endl;

    const uint64_t W_values[] = {10, 100, 1000, 10000, 100000, 1000000};
    uint64_t M = static_cast<uint64_t>(10) * C * N;

    std::cout << "[ws]   M = " << M << " searches per W value." << std::endl;

    for (uint64_t W : W_values) {
        auto tW = Clock::now();
        std::cout << "[ws]     W=" << W << std::flush;

        std::mt19937 rng(seed + W);

        std::vector<size_t> indices(N);
        for (size_t i = 0; i < N; ++i) indices[i] = i;
        std::shuffle(indices.begin(), indices.end(), rng);

        std::vector<uint32_t> workingSet(W);
        for (uint64_t i = 0; i < W; ++i) workingSet[i] = dataset[indices[i]];
        std::cout << " | setup " << since(tW) << "ms" << std::flush;

        std::uniform_int_distribution<uint64_t> pick(0, W - 1);
        std::vector<uint32_t> searchKeys(M);
        for (uint64_t i = 0; i < M; ++i) searchKeys[i] = workingSet[pick(rng)];
        std::cout << " | keys gen " << since(tW) << "ms" << std::flush;

        auto t0 = Clock::now();
        for (auto key : searchKeys) avl.search(key);
        auto t1 = Clock::now();
        double avlMs = elapsedMs(t0, t1);
        std::cout << " | AVL " << avlMs << "ms" << std::flush;

        t0 = Clock::now();
        for (auto key : searchKeys) splay.search(key);
        t1 = Clock::now();
        double splayMs = elapsedMs(t0, t1);
        std::cout << " | Splay " << splayMs << "ms" << std::endl;

        f << W << "," << avlMs << "," << splayMs << "\n";
        f.flush();
    }

    avl.clear();
    splay.clear();
    std::cout << "[ws] Done. Total: " << since(tStart) << "ms" << std::endl;
}

static void getPreorder(Node* node, std::vector<uint32_t>& seq) {
    if (!node) return;
    seq.push_back(node->key);
    getPreorder(node->left, seq);
    getPreorder(node->right, seq);
}

void runTraversal(const std::string& outDir, uint64_t seed) {
    std::cout << "[traversal] Traversal Conjecture (N=2^25)" << std::endl;

    auto tStart = Clock::now();

    auto f = openCsv(outDir, "traversal_results.csv");
    f << "step,search_time_ms\n";

    const uint64_t N = 33554432ULL;
    std::cout << "[traversal]   Generating dataset..." << std::flush;
    auto dataset = generateUniform(N, seed);
    std::cout << " " << since(tStart) << "ms" << std::endl;

    auto t1b = Clock::now();
    std::cout << "[traversal]   Building T1 (original perm)..." << std::flush;
    SplayTree T1;
    for (auto key : dataset) T1.insert(key);
    std::cout << " " << since(t1b) << "ms" << std::endl;

    auto tShuffle = Clock::now();
    std::cout << "[traversal]   Shuffling dataset..." << std::flush;
    std::mt19937 rng(seed + 1);
    std::shuffle(dataset.begin(), dataset.end(), rng);
    std::cout << " " << since(tShuffle) << "ms" << std::endl;

    auto t2b = Clock::now();
    std::cout << "[traversal]   Building T2 (shuffled perm)..." << std::flush;
    SplayTree T2;
    for (auto key : dataset) T2.insert(key);
    std::cout << " " << since(t2b) << "ms" << std::endl;
    std::cout << "[traversal]   Both trees built. Total build: " << since(tStart) << "ms" << std::endl;

    auto tPreorder = Clock::now();
    std::cout << "[traversal]   Getting preorder of T1..." << std::flush;
    std::vector<uint32_t> seq;
    seq.reserve(N);
    getPreorder(T1.getRoot(), seq);
    std::cout << " " << since(tPreorder) << "ms" << std::endl;

    auto tSearch = Clock::now();
    std::cout << "[traversal]   Searching " << seq.size() << " keys in T2..." << std::flush;
    std::cout << " (progress: " << std::flush;

    double totalMs = 0.0;
    uint64_t step = 0;
    uint64_t pctStep = seq.size() / 10;
    if (pctStep == 0) pctStep = 1;

    for (auto key : seq) {
        auto t0 = Clock::now();
        T2.search(key);
        auto t1 = Clock::now();
        double ms = elapsedMs(t0, t1);
        totalMs += ms;
        f << step << "," << ms << "\n";
        ++step;
        if (step % pctStep == 0) std::cout << "." << std::flush;
    }
    std::cout << ")" << std::endl;

    std::cout << "[traversal]   Searches done: " << since(tSearch) << "ms" << std::endl;
    std::cout << "[traversal]   Total time: " << totalMs << " ms" << std::endl;
    std::cout << "[traversal] Done. Total: " << since(tStart) << "ms" << std::endl;
    T1.clear();
    T2.clear();
}
