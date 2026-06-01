#include "experiment.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

const char* getOpt(int argc, char** argv, const std::string& flag) {
    for (int i = 1; i < argc - 1; ++i) {
        if (flag == argv[i]) return argv[i + 1];
    }
    return nullptr;
}

bool hasFlag(int argc, char** argv, const std::string& flag) {
    for (int i = 1; i < argc; ++i) {
        if (flag == argv[i]) return true;
    }
    return false;
}

void usage() {
    std::cerr << "Uso: ./bin/trees run [--c 5] [--lambda 0.01] [--seed 42] "
              << "[--out-dir results/] [--bonus]\n";
}

int cmdRun(int argc, char** argv) {
    const char* cStr = getOpt(argc, argv, "--c");
    const char* lambdaStr = getOpt(argc, argv, "--lambda");
    const char* seedStr = getOpt(argc, argv, "--seed");
    const char* outDirStr = getOpt(argc, argv, "--out-dir");
    bool bonus = hasFlag(argc, argv, "--bonus");

    int C = cStr ? std::stoi(cStr) : 5;
    double lambda = lambdaStr ? std::stod(lambdaStr) : 0.01;
    uint64_t seed = seedStr ? std::stoull(seedStr) : 42ULL;
    std::string outDir = outDirStr ? outDirStr : "results";

    int dummy __attribute__((unused)) = std::system(("mkdir -p " + outDir).c_str());

    std::cout << "=== Base Scenarios (A/B/C/D) ===" << std::endl;
    runBaseScenarios(outDir, C, lambda, seed);

    std::cout << "=== Theorems ===" << std::endl;
    runSequentialAccess(outDir, seed);
    runWorkingSet(outDir, C, seed);

    if (bonus) {
        std::cout << "=== Bonus: Traversal Conjecture ===" << std::endl;
        runTraversal(outDir, seed);
    }

    std::cout << "Done." << std::endl;
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        usage();
        return 1;
    }
    const std::string cmd = argv[1];

    try {
        if (cmd == "run") return cmdRun(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }

    usage();
    return 1;
}
