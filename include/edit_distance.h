#ifndef DISTANCEMEASURES_HMT_EDIT_DISTANCE_H
#define DISTANCEMEASURES_HMT_EDIT_DISTANCE_H

#include <vector>
#include <fstream>
#include <sstream>
#include <functional>
#include "distance_measures.h"

using std::pair, std::vector;

class EditDistance {

private:
    static bool match(Vec3D &vec1, Vec3D &vec2, float epsilon, std::function<double(const Vec3D&, const Vec3D&)> func);
public:
    // Edit distance on real sequences
    static double edr(Vec3D *v1, Vec3D *v2, int size_v1, int size_v2, double epsilon, std::function<double(const Vec3D&, const Vec3D&)> func);
    // Time warp edit distance (todo)
};

#endif