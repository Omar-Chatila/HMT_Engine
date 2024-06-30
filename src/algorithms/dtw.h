#ifndef DISTANCEMEASURES_HMT_DTW_H
#define DISTANCEMEASURES_HMT_DTW_H

#include <vector>
#include <fstream>
#include <sstream>
#include <functional>
#include "distance_measures.h"

#define CURRENT_INDEX i * (m + 1) + j
#define ABOVE_INDEX ((i - 1) * (m + 1) + j)
#define LEFT_INDEX i * (m + 1) + (j - 1)
#define DIAG_LEFT_INDEX ((i - 1) * (m + 1) + (j-1))

using std::pair, std::vector;

class Dtw {

public:
    static float *dtw(const Vec3D *v1, const Vec3D *v2, int size_v1, int size_v2,
                      std::function<float(const Vec3D &, const Vec3D &)> &func);

    static float *dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                      std::function<float(const Quaternion *, const Quaternion *)> &func);

    static float *wdtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                       std::function<float(const Quaternion *, const Quaternion *)> &func, float g, float w_max);

    static float *wddtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                        std::function<float(const Quaternion *, const Quaternion *)> &func, float g, float w_max);

    static float *wdtw(const std::vector<Vec3D *> &inp_traj, const std::vector<Vec3D *> &ref_traj,
                       std::function<float(const Vec3D *, const Vec3D *)> &func, float g, float w_max);

    static float *get_cost_matrix(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                                  std::function<float(const Quaternion *, const Quaternion *)> &func);

    static std::pair<float, std::vector<int>> get_cost_and_alignment(float *cost_matrix, int n, int m);
};

#endif