#include <benchmark/benchmark.h>
#include "mybenchmark.h"

#define START 1
#define END 96
#define ITERATIONS 10

class MotionFileFixture : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State &state) override {
        if (squat_files.empty())
            initFileLocations(SQUATS);
        if (in_quat_frames.empty() || ref_quat_frames.empty())
            initTrajectories();
    }
};

// Benchmark parsing process
BENCHMARK_DEFINE_F(MotionFileFixture, MotionFile_Parsing)(benchmark::State &state) {
    int num_files = state.range(0);
    for (auto _: state) {
        for (int i = 0; i < num_files; ++i) {
            Input_parser *parser = new Input_parser(squat_files[i].c_str());
            benchmark::DoNotOptimize(parser);
            parser->get_frames();
            delete parser;
        }
    }
}

/*
BENCHMARK_REGISTER_F(MotionFileFixture, MotionFile_Parsing)
    ->Name("Motion File Parsing")
    ->DenseRange(START, END, 1)
    ->Complexity(benchmark::oN)
    ->Unit(benchmark::kMillisecond);
    */

// Benchmark Dynamic Time Warping
BENCHMARK_DEFINE_F(MotionFileFixture, DTW_Execution)(benchmark::State &state) {
    int i_index = state.range(0);
    for (auto _: state) {
        float *res = Dtw::dtw(in_quat_frames[i_index], ref_quat_frames[0], quat_dist);
        benchmark::DoNotOptimize(res);
        free(res);
    }
}

BENCHMARK_REGISTER_F(MotionFileFixture, DTW_Execution)
        ->Name("Dynamic Time Warping")
        ->DenseRange(0, 79, 1)
        ->Unit(benchmark::kMicrosecond)
        ->Iterations(ITERATIONS);

// Benchmark LCSS
BENCHMARK_DEFINE_F(MotionFileFixture, LCSS_Execution)(benchmark::State &state) {
    int i_index = state.range(0);
    for (const auto _: state) {
        float res = LCSS::lcss(in_quat_frames[i_index], ref_quat_frames[0], 0.3f, 5.0f, quat_dist);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_REGISTER_F(MotionFileFixture, LCSS_Execution)
        ->Name("LCSS")
        ->DenseRange(0, 79, 1)
        ->Unit(benchmark::kMicrosecond)
        ->Iterations(ITERATIONS);

// Benchmark Frechet
BENCHMARK_DEFINE_F(MotionFileFixture, FRECHET_Execution)(benchmark::State &state) {
    int i_index = state.range(0);
    for (const auto _: state) {
        float res = Frechet::frechet(in_quat_frames[i_index], ref_quat_frames[0], quat_dist);
        benchmark::DoNotOptimize(res);
    }
}

// Benchmark EDR
BENCHMARK_DEFINE_F(MotionFileFixture, EDR_Execution)(benchmark::State &state) {
    int i_index = state.range(0);
    for (const auto _: state) {
        float res = EditDistance::edr(in_quat_frames[i_index], ref_quat_frames[0], 0.3f, quat_dist);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_REGISTER_F(MotionFileFixture, EDR_Execution)
        ->Name("EDR")
        ->DenseRange(0, 79, 1)
        ->Unit(benchmark::kMillisecond)
        ->Iterations(ITERATIONS);

// Benchmark TWED
BENCHMARK_DEFINE_F(MotionFileFixture, TWED_Execution)(benchmark::State &state) {
    int i_index = state.range(0);
    for (const auto _: state) {
        float res = EditDistance::twed(in_quat_frames[i_index], ref_quat_frames[0], 0.3f, 1.0f, quat_dist);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_REGISTER_F(MotionFileFixture, TWED_Execution)
        ->Name("TWED")
        ->DenseRange(0, 79, 1)
        ->Unit(benchmark::kMillisecond)
        ->Iterations(ITERATIONS);

BENCHMARK_MAIN();
