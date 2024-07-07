#ifndef DISTANCEMEASURES_HMT_DTW_H
#define DISTANCEMEASURES_HMT_DTW_H

#include <vector>
#include <fstream>
#include <sstream>
#include <functional>
#include "distance_measures.h"
#include "algo_settings.h"
#include "imgui.h"

#define CURRENT_INDEX i * (m + 1) + j
#define ABOVE_INDEX ((i - 1) * (m + 1) + j)
#define LEFT_INDEX i * (m + 1) + (j - 1)
#define DIAG_LEFT_INDEX ((i - 1) * (m + 1) + (j-1))

using std::pair, std::vector;

class Dtw {

public:

    static std::vector<Quaternion *> deep_copy(const std::vector<Quaternion *> &original);

    static float *dtw(const Vec3D *v1, const Vec3D *v2, int size_v1, int size_v2,
                      std::function<float(const Vec3D &, const Vec3D &)> &func);

    static float *dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                      std::function<float(const Quaternion *, const Quaternion *,
                                          const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    static float *
    standardized_dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                     std::function<float(const Quaternion *, const Quaternion *,
                                         const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    static float *wdtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                       std::function<float(const Quaternion *, const Quaternion *,
                                           const std::array<bool, JOINT_COUNT> &selectedJ)> &func, float g,
                       float w_max);

    static float *wddtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                        std::function<float(const Quaternion *, const Quaternion *,
                                            const std::array<bool, JOINT_COUNT> &selectedJ)> &func, float g,
                        float w_max);

    static float *wdtw(const std::vector<Vec3D *> &inp_traj, const std::vector<Vec3D *> &ref_traj,
                       std::function<float(const Vec3D *, const Vec3D *)> &func, float g, float w_max);

    static float *get_cost_matrix(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                                  std::function<float(const Quaternion *, const Quaternion *,
                                                      const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    static std::pair<float, std::vector<int>> get_cost_and_alignment(float *cost_matrix, int n, int m);

    static std::vector<Quaternion *>
    normalize_trajectory(const std::vector<Quaternion *> &trajectory,
                         const std::vector<std::pair<Quaternion, Quaternion>> &max_rotations) {
        std::vector<Quaternion *> normalized_trajectory = trajectory;
        for (auto &quats: normalized_trajectory) {
            for (int i = 0; i < JOINT_COUNT; ++i) {
                quats[i].w =
                        (quats[i].w - max_rotations[i].first.w) /
                        (max_rotations[i].second.w - max_rotations[i].first.w);
                quats[i].x =
                        (quats[i].x - max_rotations[i].first.x) /
                        (max_rotations[i].second.x - max_rotations[i].first.x);
                quats[i].y =
                        (quats[i].y - max_rotations[i].first.y) /
                        (max_rotations[i].second.y - max_rotations[i].first.y);
                quats[i].z =
                        (quats[i].z - max_rotations[i].first.z) /
                        (max_rotations[i].second.z - max_rotations[i].first.z);
                quats[i] = quats[i] / norm(quats[i]);
            }
        }
        return normalized_trajectory;
    }

private:

    static void write_matrix_to_file(float *S, int rows, int cols, const std::string &filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    file << S[i * cols + j] << " ";
                }
                file << std::endl;
            }
            file.close();
        } else {
            std::cerr << "Unable to open file " << filename << std::endl;
        }
    }
};

#endif