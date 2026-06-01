#include "dataset.hpp"
#include <random>
#include <cmath>

std::vector<uint32_t> generateUniform(uint32_t n, uint64_t seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
    std::vector<uint32_t> data(n);
    for (uint32_t i = 0; i < n; ++i)
        data[i] = dist(rng);
    return data;
}

std::vector<double> exponentialPMF(uint32_t n, double lambda) {
    std::vector<double> P(n);
    double denom = 1.0 - std::exp(-lambda * n);
    for (uint32_t i = 0; i < n; ++i)
        P[i] = std::exp(-lambda * i) * (1.0 - std::exp(-lambda)) / denom;
    return P;
}
