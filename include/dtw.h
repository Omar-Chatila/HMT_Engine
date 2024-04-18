//
// Created by omarc on 09/04/2024.
//

#ifndef DISTANCEMEASURES_HMT_DTW_H
#define DISTANCEMEASURES_HMT_DTW_H

#include <vector>
#include <fstream>
#include <sstream>
#include "distance_measures.h"

using std::pair, std::vector;

class Dtw {

public:
    static double* dtw(const double *v1, const double *v2, int size_v1, int size_v2, double (*func) (double, double));
    static double* dtw(const Point3D *v1, const Point3D *v2, int size_v1, int size_v2, double (*func) (Point3D, Point3D));

    static std::pair<double, std::vector<int>> get_cost_and_alignment(const double* cost_matrix, int m, int n);
    static vector<vector<double>> get_curves_from_csv();
    static pair<double, vector<int>> run_example(double *vals1, double *vals2, int n, int m, double (*func)(double, double));
    static void set_frames(double *vals1, double *vals2, int m, int n, vector<double *> &frames1, vector<double *> &frames2);
    
};

#endif