#ifndef DISTANCEMEASURES_HMT_DTW_H
#define DISTANCEMEASURES_HMT_DTW_H

#include <vector>
#include <fstream>
#include <sstream>
#include <functional>
#include "distance_measures.h"
#include "algo_settings.h"

#define INDEX(x, y) ((x)* (m + 1) + (y))
#define INDEX2(x, y, M) ((x) * (M + 3) + (y))
#define WEIGHT_F(n, m) func(inp_traj[n], ref_traj[m], AlgoSettings::getInstance().joint_weights)
#define SELECT_F(n, m) func(inp_traj[n], ref_traj[m], AlgoSettings::getInstance().selected_joints)

#define CURRENT_INDEX i * (m + 1) + j
#define ABOVE_INDEX ((i - 1) * (m + 1) + j)
#define LEFT_INDEX i * (m + 1) + (j - 1)
#define DIAG_LEFT_INDEX ((i - 1) * (m + 1) + (j-1))

using std::pair, std::vector;

/**
 * @class Dtw
 * @brief A class providing various Dynamic Time Warping (DTW) algorithms for trajectory analysis.
 */
class Dtw {

public:

    /**
     * @brief Creates a deep copy of a vector of Quaternion pointers.
     * @param original The original vector to copy.
     * @return A new vector that is a deep copy of the original.
     */
    static std::vector<Quaternion *> deep_copy(const std::vector<Quaternion *> &original);

    /**
     * @brief Computes the DTW distance between two vectors of Vec3D.
     * @param v1 First vector.
     * @param v2 Second vector.
     * @param size_v1 Size of the first vector.
     * @param size_v2 Size of the second vector.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static float *dtw(const Vec3D *v1, const Vec3D *v2, int size_v1, int size_v2,
                      std::function<float(const Vec3D &, const Vec3D &)> &func);

    /**
     * @brief Computes the DTW distance between two trajectories of Quaternion pointers using a joint selection function.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static float *dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                      std::function<float(const Quaternion *, const Quaternion *,
                                          const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    /**
     * @brief Computes the DTW distance between two trajectories of Quaternion pointers using a weighting function.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static float *dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                      std::function<float(const Quaternion *, const Quaternion *,
                                          const std::array<float, JOINT_COUNT> &jWeights)> &func);

    /**
     * @brief Step size function that can omit frames but has lower vertical/horizontal alignment occurrences.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static float *ss1_dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                          std::function<float(const Quaternion *, const Quaternion *,
                                              const std::array<float, JOINT_COUNT> &jWeights)> &func);

    /**
     * @brief Variant with step size function that can omit frames but has lower vertical/horizontal alignment occurrences.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static float *ss1_dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                          std::function<float(const Quaternion *, const Quaternion *,
                                              const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    /**
     * @brief Step size function that cannot omit frames but has lower vertical/horizontal alignment occurrences.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static float *ssc2_dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                           std::function<float(const Quaternion *, const Quaternion *,
                                               const std::array<float, JOINT_COUNT> &jWeights)> &func);

    /**
     * @brief Step size function that cannot omit frames but has lower vertical/horizontal alignment occurrences.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static float *ssc2_dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                           std::function<float(const Quaternion *, const Quaternion *,
                                               const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    /**
     * @brief Computes the DTW matrix between two trajectories of normalized Quaternion pointers using a joint-selection function.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static float *
    standardized_dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                     std::function<float(const Quaternion *, const Quaternion *,
                                         const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    /**
     * @brief Computes the standardized DTW distance between two trajectories of Quaternion pointers using a weighting function.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static float *
    standardized_dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                     std::function<float(const Quaternion *, const Quaternion *,
                                         const std::array<float, JOINT_COUNT> &selectedJ)> &func);

    /**
     * @brief Computes the weighted DTW distance between two trajectories of Quaternion pointers using a selection function.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @param g Weight parameter.
     * @param w_max Maximum weight.
     * @return A pointer to the cost matrix.
     */
    static float *wdtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                       std::function<float(const Quaternion *, const Quaternion *,
                                           const std::array<bool, JOINT_COUNT> &selectedJ)> &func, float g,
                       float w_max);

    /**
     * @brief Computes the weighted DTW distance between two trajectories of Quaternion pointers using a weighting function.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @param g Weight parameter.
     * @param w_max Maximum weight.
     * @return A pointer to the cost matrix.
     */
    static float *wdtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                       std::function<float(const Quaternion *, const Quaternion *,
                                           const std::array<float, JOINT_COUNT> &selectedJ)> &func, float g,
                       float w_max);

    /**
     * @brief Computes the weighted derivative DTW distance between two trajectories of Quaternion pointers using a selection function.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @param g Weight parameter.
     * @param w_max Maximum weight.
     * @return A pointer to the cost matrix.
     */
    static float *wddtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                        std::function<float(const Quaternion *, const Quaternion *,
                                            const std::array<bool, JOINT_COUNT> &selectedJ)> &func, float g,
                        float w_max);

    /**
     * @brief Computes the weighted derivative DTW distance between two trajectories of Quaternion pointers using a weighting function.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @param g Weight parameter.
     * @param w_max Maximum weight.
     * @return A pointer to the cost matrix.
     */
    static float *wddtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                        std::function<float(const Quaternion *, const Quaternion *,
                                            const std::array<float, JOINT_COUNT> &selectedJ)> &func, float g,
                        float w_max);

    /**
     * @brief Computes the weighted DTW distance between two trajectories of Vec3D pointers.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @param g Weight parameter.
     * @param w_max Maximum weight.
     * @return A pointer to the cost matrix.
     */
    static float *wdtw(const std::vector<Vec3D *> &inp_traj, const std::vector<Vec3D *> &ref_traj,
                       std::function<float(const Vec3D *, const Vec3D *)> &func, float g, float w_max);


    /**
     * @brief Computes the cost matrix for DTW between two trajectories of Quaternion pointers using a selection function.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static float *get_cost_matrix(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                                  std::function<float(const Quaternion *, const Quaternion *,
                                                      const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    /**
     * @brief Computes the cost matrix for DTW between two trajectories of Quaternion pointers using a selection function.
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static float *get_cost_matrix(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                                  std::function<float(const Quaternion *, const Quaternion *,
                                                      const std::array<float, JOINT_COUNT> &selectedJ)> &func);

    /**
     * @brief Computes the cost matrix for DTW between two trajectories of Quaternion pointers using a selection function with step-size condition 2
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static std::pair<float, std::vector<int>> get_cost_and_alignment_ss2(float *cost_matrix, int n, int m);

    /**
     * @brief Computes the cost matrix for DTW between two trajectories of Quaternion pointers using a selection function with step-size condition 2
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @return A pointer to the cost matrix.
     */
    static std::pair<float, std::vector<int>> get_cost_and_alignment_ss1(float *cost_matrix, int n, int m);

    /**
     * @brief Retrieves the cost and alignment from the cost matrix.
     * @param cost_matrix The cost matrix.
     * @param n Number of rows.
     * @param m Number of columns.
     * @return A pair containing the final cost and the alignment path.
     */
    static std::pair<float, std::vector<int>> get_cost_and_alignment(float *cost_matrix, int n, int m);

    /**
     * @brief Computes the DTW distance between two trajectories of Quaternion pointers using a joint weighing function while applying 3 local weight parameters w_d, w_h, w_v to the respective indices
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @param weights Vector containing the local weights in order of diagonal, horizontal and vertical
     * @return A pointer to the cost matrix.
     */
    static float *
    local_weights_dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                      std::function<float(const Quaternion *, const Quaternion *,
                                          const std::array<float, JOINT_COUNT> &jWeights)> &func, const Vec3D &weights);

    /**
     * @brief Computes the DTW distance between two trajectories of Quaternion pointers using a joint selection function while applying 3 local weight parameters w_d, w_h, w_v to the respective indices
     * @param inp_traj Input trajectory.
     * @param ref_traj Reference trajectory.
     * @param func Distance function to use.
     * @param weights Vector containing the local weights in order of diagonal, horizontal and vertical
     * @return A pointer to the cost matrix.
     */
    static float *
    local_weights_dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                      std::function<float(const Quaternion *, const Quaternion *,
                                          const std::array<bool, JOINT_COUNT> &selectedJ)> &func, const Vec3D &weights);

    /**
     * @brief Normalizes a trajectory based on given max rotations.
     * @param trajectory The trajectory to normalize.
     * @param max_rotations Maximum rotations for normalization.
     * @return A normalized trajectory.
     */
    static std::vector<Quaternion *>
    normalize_trajectory(const std::vector<Quaternion *> &trajectory,
                         const std::vector<std::pair<Quaternion, Quaternion>> &max_rotations);


private:

    /**
     * @brief Writes a matrix to a file.
     * @param S The matrix to write.
     * @param rows Number of rows in the matrix.
     * @param cols Number of columns in the matrix.
     * @param filename The name of the file.
     */
    static void write_matrix_to_file(float *S, int rows, int cols, const std::string &filename);
};

#endif
