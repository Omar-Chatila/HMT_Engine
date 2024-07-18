#ifndef DISTANCEMEASURESHMT_PARSER_H
#define DISTANCEMEASURESHMT_PARSER_H

#include "trajectory_analysis_manager.h"
#include "util.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using std::string, std::vector, std::cout, std::endl, std::stof, std::stoi;

struct BenchmarkEntry {
    string algorithm;
    int n;
    int m;
    float time_ms;
    int iterations;

    friend std::ostream &operator<<(std::ostream &os, const BenchmarkEntry &be) {
        os << be.algorithm << ": " << "(" << be.n << "," << be.m << "): " << be.time_ms << " ms - I: " << be.iterations;
        return os;
    }
};

inline vector<BenchmarkEntry> algoResult(const char *path) {
    vector<BenchmarkEntry> benchResults;
    vector<string> lines = readAllLines(path, 5);
    for (auto &line: lines) {
        // Split parameters (name and input sizes) and results (times and iterations)
        auto parts = split(line, ':');
        auto param = parts[0];
        auto res = parts[1];

        // split parameters into components (first: Name, second: n, third: m, fourth: iterations)
        auto params = split(param, '/');
        // split results into components (first: Time, second: CPU, third: Iterations)
        auto results = splitBySpaces(res);

        string name = params[0];
        int n = stoi(params[1]);
        int m = stoi(params[2]);
        float time = stof(results[1]);
        int iter = stoi(results[0]);

        benchResults.push_back({name, n, m, time, iter});
    }

    return benchResults;
}

using AlgorithmResults = std::map<std::pair<int, int>, std::vector<BenchmarkEntry>>;

inline std::map<std::string, std::vector<BenchmarkEntry>>
organizeAndAverageResults(const std::vector<BenchmarkEntry> &raw_entries) {
    std::map<std::string, AlgorithmResults> results;

    // Organize entries by algorithm and (n, m) pair
    for (const auto &entry: raw_entries) {
        results[entry.algorithm][{std::min(entry.n, entry.m), std::max(entry.n, entry.m)}].push_back(entry);
    }

    // Calculate averages and create new results
    std::map<std::string, std::vector<BenchmarkEntry>> averagedResults;

    for (const auto &[algorithm, algorithmResults]: results) {
        for (const auto &[key, entries]: algorithmResults) {
            float sum_time = 0;
            int sum_iterations = 0;
            for (const auto &entry: entries) {
                sum_time += entry.time_ms;
                sum_iterations += entry.iterations;
            }
            int count = entries.size();
            BenchmarkEntry avgEntry{
                    algorithm, key.first, key.second, sum_time / count, sum_iterations / count
            };
            averagedResults[algorithm].push_back(avgEntry);
        }
    }

    return averagedResults;
}

#endif //DISTANCEMEASURESHMT_PARSER_H
