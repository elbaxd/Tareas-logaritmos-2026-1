#pragma once

#include <cstdint>
#include <string>

void runBaseScenarios(const std::string& outDir, int C, double lambda, uint64_t seed);
void runSequentialAccess(const std::string& outDir, uint64_t seed);
void runWorkingSet(const std::string& outDir, int C, uint64_t seed);
void runTraversal(const std::string& outDir, uint64_t seed);
