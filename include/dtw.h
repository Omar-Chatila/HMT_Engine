#ifndef DISTANCEMEASURES_HMT_DTW_H
#define DISTANCEMEASURES_HMT_DTW_H

#include <vector>
#include <fstream>
#include <sstream>
#include "distance_measures.h"

using std::pair, std::vector;

class Dtw {

public:
    static double* dtw(const Vec3D *v1, const Vec3D *v2, int size_v1, int size_v2, double (*func) (Vec3D, Vec3D));
    static std::pair<double, std::vector<int>> get_cost_and_alignment(const double* cost_matrix, int m, int n);
};

#endif