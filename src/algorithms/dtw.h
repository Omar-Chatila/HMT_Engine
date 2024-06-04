#ifndef DISTANCEMEASURES_HMT_DTW_H
#define DISTANCEMEASURES_HMT_DTW_H

#include <vector>
#include <fstream>
#include <sstream>
#include <functional>
#include "distance_measures.h"

using std::pair, std::vector;

class Dtw {

public:
    static float* dtw(const Vec3D *v1, const Vec3D *v2, int size_v1, int size_v2, std::function<float(const Vec3D&, const Vec3D&)> func);
    static float* dtw(const std::vector<Quaternion*> &inp_traj, const std::vector<Quaternion*> &ref_traj, std::function<float(const Quaternion*, const Quaternion*)> func);
    static std::pair<float, std::vector<int>> get_cost_and_alignment(float* cost_matrix, int n, int m);
};

#endif