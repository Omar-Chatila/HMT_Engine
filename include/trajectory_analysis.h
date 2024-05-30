#ifndef TRAJECTOY_ANALYSIS_H
#define TRAJECTOY_ANALYSIS_H

#include <functional>
#include "trajectories.h"
#include "distance_measures.h"
#include "dtw.h"
#include "edit_distance.h"

// For Minkonwski
const double p = 3;

enum Distances {
    EUCLID, SQUARED_EUCLID, MANHATTAN, MINKOWSKI, MAX_NORM, MIN_DISTANCE,
    CANBERRA_DISTANCE, SCALAR_PRODUCT, COSINE_SIMILARITY, COSINE_DISTANCE1,
    COSINE_DISTANCE2, HAMMING_DISTANCE
};

extern std::vector<std::function<double(const Vec3D&, const Vec3D&)>> dist_measures;

class Trajectoy_analysis {
private:
    Trajectories input_trajectories;
    Trajectories reference_trajectories;
public:
    Trajectoy_analysis(const Trajectories &p_input, const Trajectories &p_reference) 
        : input_trajectories{p_input}, reference_trajectories{p_reference} {}
    ~Trajectoy_analysis() {};

    std::pair<double, std::vector<int>> perform_DTW(Joint::Type joint, Distances type);
    double perform_EDR(Joint::Type joint, Distances type, double epsilon);

};

#endif