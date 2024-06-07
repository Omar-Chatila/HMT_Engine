#ifndef DISTANCEMEASURES_HMT_SINGLECONTEXT_H
#define DISTANCEMEASURES_HMT_SINGLECONTEXT_H

class SingleContext {
public:
    std::string input_file;
    std::string reference_file;

    int selected_amateur;
    int selected_prof;
    std::vector<std::string> amateur_squats;
    std::vector<std::string> prof_squats;
    std::vector<std::string> amateur_tai;
    std::vector<std::string> prof_tai;

    Distances dist_func;
    Trajectoy_analysis *analysis;
    std::tuple<float*, std::vector<int>, int, int> *matrix;
    float* costmatrix;
    float cost;
    int c_frame;

    SingleContext() {

    };
};

#endif