#include <benchmark/benchmark.h>
#include "mybenchmark.h"

#define START1 100
#define START2 100

#define STEP1 200
#define STEP2 200

#define END1 5000
#define END2 5000
#define ITERATIONS 1

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
    int r_index = state.range(1);
    size_t i_size = in_quat_frames.size();
    size_t r_size = ref_quat_frames.size();
    std::vector<Quaternion *> v1(in_quat_frames.begin(), in_quat_frames.end() - i_size + i_index);
    std::vector<Quaternion *> v2(ref_quat_frames.begin(), ref_quat_frames.end() - r_size + r_index);

    for (auto _: state) {
        float *res = Dtw::dtw(v1, v2, quaternion_dist);
        benchmark::DoNotOptimize(res);
        free(res);
    }
}

BENCHMARK_REGISTER_F(MotionFileFixture, DTW_Execution)
        ->Name("Dynamic Time Warping")
        ->ArgsProduct({benchmark::CreateDenseRange(START1, END1, STEP1),
                       benchmark::CreateDenseRange(START2, END2, STEP2)})
        ->Unit(benchmark::kMillisecond)
        ->Complexity(benchmark::oAuto)
        ->Iterations(ITERATIONS);

// Benchmark Dynamic Time Warping SSC1
BENCHMARK_DEFINE_F(MotionFileFixture, DTW_SSC1_Execution)(benchmark::State &state) {
    int i_index = state.range(0);
    int r_index = state.range(1);
    size_t i_size = in_quat_frames.size();
    size_t r_size = ref_quat_frames.size();
    std::vector<Quaternion *> v1(in_quat_frames.begin(), in_quat_frames.end() - i_size + i_index);
    std::vector<Quaternion *> v2(ref_quat_frames.begin(), ref_quat_frames.end() - r_size + r_index);

    for (auto _: state) {
        float *res = Dtw::ss1_dtw(v1, v2, quaternion_dist);
        benchmark::DoNotOptimize(res);
        free(res);
    }
}

BENCHMARK_REGISTER_F(MotionFileFixture, DTW_SSC1_Execution)
        ->Name("DTW - Step-size-condition 1")
        ->ArgsProduct({benchmark::CreateDenseRange(START1, END1, STEP1),
                       benchmark::CreateDenseRange(START2, END2, STEP2)})
        ->Unit(benchmark::kMillisecond)
        ->Complexity(benchmark::oAuto)
        ->Iterations(ITERATIONS);

// Benchmark Dynamic Time Warping SSC2
BENCHMARK_DEFINE_F(MotionFileFixture, DTW_SSC2_Execution)(benchmark::State &state) {
    int i_index = state.range(0);
    int r_index = state.range(1);
    size_t i_size = in_quat_frames.size();
    size_t r_size = ref_quat_frames.size();
    std::vector<Quaternion *> v1(in_quat_frames.begin(), in_quat_frames.end() - i_size + i_index);
    std::vector<Quaternion *> v2(ref_quat_frames.begin(), ref_quat_frames.end() - r_size + r_index);

    for (auto _: state) {
        float *res = Dtw::ssc2_dtw(v1, v2, quaternion_dist);
        benchmark::DoNotOptimize(res);
        free(res);
    }
}

BENCHMARK_REGISTER_F(MotionFileFixture, DTW_SSC2_Execution)
        ->Name("DTW - Step-size-condition 2")
        ->ArgsProduct({benchmark::CreateDenseRange(START1, END1, STEP1),
                       benchmark::CreateDenseRange(START2, END2, STEP2)})
        ->Unit(benchmark::kMillisecond)
        ->Complexity(benchmark::oAuto)
        ->Iterations(ITERATIONS);

// Benchmark Dynamic Time Warping with local weights
BENCHMARK_DEFINE_F(MotionFileFixture, DTW_Local_Weights_Execution)(benchmark::State &state) {
    int i_index = state.range(0);
    int r_index = state.range(1);
    size_t i_size = in_quat_frames.size();
    size_t r_size = ref_quat_frames.size();
    std::vector<Quaternion *> v1(in_quat_frames.begin(), in_quat_frames.end() - i_size + i_index);
    std::vector<Quaternion *> v2(ref_quat_frames.begin(), ref_quat_frames.end() - r_size + r_index);

    for (auto _: state) {
        float *res = Dtw::local_weights_dtw(v1, v2, quaternion_dist, {2.0, 1.0, 1.0});
        benchmark::DoNotOptimize(res);
        free(res);
    }
}

BENCHMARK_REGISTER_F(MotionFileFixture, DTW_Local_Weights_Execution)
        ->Name("DTW_Local-Weights (2, 1, 1)")
        ->ArgsProduct({benchmark::CreateDenseRange(START1, END1, STEP1),
                       benchmark::CreateDenseRange(START2, END2, STEP2)})
        ->Unit(benchmark::kMillisecond)
        ->Complexity(benchmark::oAuto)
        ->Iterations(ITERATIONS);

// Benchmark LCSS
BENCHMARK_DEFINE_F(MotionFileFixture, LCSS_Execution)(benchmark::State &state) {
    int i_index = state.range(0);
    int r_index = state.range(1);
    size_t i_size = in_quat_frames.size();
    size_t r_size = ref_quat_frames.size();
    std::vector<Quaternion *> v1(in_quat_frames.begin(), in_quat_frames.end() - i_size + i_index);
    std::vector<Quaternion *> v2(ref_quat_frames.begin(), ref_quat_frames.end() - r_size + r_index);

    for (const auto _: state) {
        float res = LCSS::lcss(v1, v2, 0.3f, 5.0f, quaternion_dist);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_REGISTER_F(MotionFileFixture, LCSS_Execution)
        ->Name("LCSS")
        ->ArgsProduct({benchmark::CreateDenseRange(START1, END1, STEP1),
                       benchmark::CreateDenseRange(START2, END2, STEP2)})
        ->Unit(benchmark::kMillisecond)
        ->Complexity(benchmark::oAuto)
        ->Iterations(ITERATIONS);

// Benchmark Frechet
BENCHMARK_DEFINE_F(MotionFileFixture, FRECHET_Execution)(benchmark::State &state) {
    int i_index = state.range(0);
    int r_index = state.range(1);
    size_t i_size = in_quat_frames.size();
    size_t r_size = ref_quat_frames.size();
    std::vector<Quaternion *> v1(in_quat_frames.begin(), in_quat_frames.end() - i_size + i_index);
    std::vector<Quaternion *> v2(ref_quat_frames.begin(), ref_quat_frames.end() - r_size + r_index);

    for (const auto _: state) {
        float res = Frechet::frechet(v1, v2, quaternion_dist);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_REGISTER_F(MotionFileFixture, FRECHET_Execution)
        ->Name("Frechet")
        ->ArgsProduct({benchmark::CreateDenseRange(START1, END1, STEP1),
                       benchmark::CreateDenseRange(START2, END2, STEP2)})
        ->Unit(benchmark::kMillisecond)
        ->Complexity(benchmark::oAuto)
        ->Iterations(ITERATIONS);

// Benchmark EDR
BENCHMARK_DEFINE_F(MotionFileFixture, EDR_Execution)(benchmark::State &state) {
    int i_index = state.range(0);
    int r_index = state.range(1);
    size_t i_size = in_quat_frames.size();
    size_t r_size = ref_quat_frames.size();
    std::vector<Quaternion *> v1(in_quat_frames.begin(), in_quat_frames.end() - i_size + i_index);
    std::vector<Quaternion *> v2(ref_quat_frames.begin(), ref_quat_frames.end() - r_size + r_index);

    for (const auto _: state) {
        float res = EditDistance::edr(v1, v2, 0.3f, quaternion_dist);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_REGISTER_F(MotionFileFixture, EDR_Execution)
        ->Name("EDR")
        ->ArgsProduct({benchmark::CreateDenseRange(START1, END1, STEP1),
                       benchmark::CreateDenseRange(START2, END2, STEP2)})
        ->Unit(benchmark::kMillisecond)
        ->Complexity(benchmark::oAuto)
        ->Iterations(ITERATIONS);

// Benchmark TWED
BENCHMARK_DEFINE_F(MotionFileFixture, TWED_Execution)(benchmark::State &state) {
    int i_index = state.range(0);
    int r_index = state.range(1);
    size_t i_size = in_quat_frames.size();
    size_t r_size = ref_quat_frames.size();
    std::vector<Quaternion *> v1(in_quat_frames.begin(), in_quat_frames.end() - i_size + i_index);
    std::vector<Quaternion *> v2(ref_quat_frames.begin(), ref_quat_frames.end() - r_size + r_index);

    for (const auto _: state) {
        float res = EditDistance::twed(v1, v2, 0.3f, 1.0f, quaternion_dist);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_REGISTER_F(MotionFileFixture, TWED_Execution)
        ->Name("TWED")
        ->ArgsProduct({benchmark::CreateDenseRange(START1, END1, STEP1),
                       benchmark::CreateDenseRange(START2, END2, STEP2)})
        ->Unit(benchmark::kMillisecond)
        ->Complexity(benchmark::oAuto)
        ->Iterations(ITERATIONS);


BENCHMARK_MAIN();

/*
int main(int argc, char *argv[]) {
    auto entries = algoResult("results.txt");
    auto averagedResults = organizeAndAverageResults(entries);

    for (const auto &[algorithm, results]: averagedResults) {
        std::cout << "Algorithm: " << algorithm << std::endl;
        for (const auto &entry: results) {
            std::cout << entry << std::endl;
        }
    }

    return 0;
}
*/