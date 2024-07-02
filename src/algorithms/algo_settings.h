#ifndef DISTANCEMEASURESHMT_ALGO_SETTINGS_H
#define DISTANCEMEASURESHMT_ALGO_SETTINGS_H

#include "distance_measures.h"

class AlgoSettings {
public:
    static AlgoSettings &getInstance() {
        static AlgoSettings instance;
        return instance;
    }

    AlgoSettings(const AlgoSettings &) = delete;

    void operator=(const AlgoSettings &) = delete;

    Distances dtw_distance;

    float wdtw_g;
    float wdtw_w_max;
    Distances wdtw_distance;

    float edr_epsilon;
    Distances edr_distance;


    float wddtw_g;
    float wddtw_w_max;
    Distances wddtw_distance;

    float twed_nu;
    float twed_lambda;
    Distances twed_distance;

    float lcss_epsilon;
    float lcss_delta;
    Distances lcss_distance;

    Distances frechet_distance;

private:
    // Private constructor to prevent instantiation
    AlgoSettings()
            : dtw_distance(Distances::EUCLID), edr_epsilon(0.3f), edr_distance(Distances::EUCLID),
              wdtw_g(0.005f), wdtw_w_max(1.0f), wdtw_distance(Distances::EUCLID),
              wddtw_g(0.005f), wddtw_w_max(1.0f), wddtw_distance(Distances::EUCLID),
              twed_nu(0.3f), twed_lambda(1.0f), twed_distance(Distances::EUCLID),
              lcss_epsilon(0.3f), lcss_delta(5.0f), lcss_distance(Distances::EUCLID),
              frechet_distance(Distances::EUCLID) {}
};

#endif // DISTANCEMEASURESHMT_ALGO_SETTINGS_H
