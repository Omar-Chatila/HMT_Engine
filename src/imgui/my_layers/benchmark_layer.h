#ifndef DISTANCEMEASURESHMT_BENCHMARK_LAYER_H
#define DISTANCEMEASURESHMT_BENCHMARK_LAYER_H

#include "../layer.h"
#include "../layer_data.h"
#include "util.h"
#include "parser.h"
#include <imgui/imgui.h>
#include <imgui/implot.h>
#include <imgui/implot_internal.h>

class BenchmarkLayer : public Layer {
public:
    BenchmarkLayer();

    void onRender() override;

    static void plotGraph(const string &algorithm, const vector<BenchmarkEntry> &entries);

    void plotAllGraphs();

private:
    std::map<std::string, std::vector<BenchmarkEntry>> averagedResults;

    void setResults();
};


#endif //DISTANCEMEASURESHMT_BENCHMARK_LAYER_H
