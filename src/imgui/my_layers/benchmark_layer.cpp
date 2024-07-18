#include "benchmark_layer.h"
#include "imgui.h"
#include "implot.h"

BenchmarkLayer::BenchmarkLayer() {
    setResults();
}

void BenchmarkLayer::onRender() {
    ImGui::Begin("Benchmark - Results");

    if (ImGui::BeginTabBar("ResultsTabBar")) {
        if (ImGui::BeginTabItem("Normal Results")) {
            for (const auto &[algorithm, results]: averagedResults) {
                plotGraph(algorithm, results);
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Comparison")) {
            plotAllGraphs();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void BenchmarkLayer::setResults() {
    std::string currentFilePath = __FILE__;
    std::string directoryPath = currentFilePath.substr(0, currentFilePath.find_last_of("\\/"));
    std::string resultsFilePath = directoryPath + "/results.txt";
    auto entries = algoResult(resultsFilePath.c_str());
    this->averagedResults = organizeAndAverageResults(entries);
}

void BenchmarkLayer::plotGraph(const std::string &algorithm, const std::vector<BenchmarkEntry> &entries) {
    std::vector<float> x_values;
    std::vector<float> y_values;

    for (const auto &entry: entries) {
        x_values.push_back(static_cast<float>(std::sqrt(entry.n * entry.m)));
        y_values.push_back(entry.time_ms);
    }

    if (ImPlot::BeginPlot(algorithm.c_str())) {
        ImPlot::PlotScatter("Time [ms]", x_values.data(), y_values.data(), static_cast<int>(x_values.size()));
        ImPlot::EndPlot();
    }
}

void BenchmarkLayer::plotAllGraphs() {
    if (ImPlot::BeginPlot("All Algorithms", ImVec2(-1, 600))) {
        for (const auto &[algorithm, entries]: averagedResults) {
            std::vector<float> x_values;
            std::vector<float> y_values;

            for (const auto &entry: entries) {
                x_values.push_back(static_cast<float>(std::sqrt(entry.n * entry.m)));
                y_values.push_back(entry.time_ms);
            }

            ImPlot::PlotScatter(algorithm.c_str(), x_values.data(), y_values.data(), static_cast<int>(x_values.size()));
        }
        ImPlot::EndPlot();
    }
}