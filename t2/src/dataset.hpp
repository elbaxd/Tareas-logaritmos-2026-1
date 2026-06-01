#pragma once

#include <cstdint>
#include <vector>

std::vector<uint32_t> generateUniform(uint32_t n, uint64_t seed);
std::vector<double> exponentialPMF(uint32_t n, double lambda);
