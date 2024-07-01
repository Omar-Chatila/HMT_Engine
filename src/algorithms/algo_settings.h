//
// Created by omarc on 01/07/2024.
//

#ifndef DISTANCEMEASURESHMT_ALGO_SETTINGS_H
#define DISTANCEMEASURESHMT_ALGO_SETTINGS_H

#include "distance_measures.h"

namespace DTW_S {
    static Distances distance;
};

namespace WDTW_S {
    static float g = 0.005f;
    static float w_max = 1.0f;
    static Distances distance;
};

namespace WDDTW_S {
    static float g = 0.005f;
    static float w_max = 1.0f;
    static Distances distance;
};

namespace EDR_S {
    static float epsilon = 0.3f;
    static Distances distance;
};

namespace TWED_S {
    static float nu = 0.3f;
    static float lambda = 1.0f;
    static Distances distance;
};

namespace LCSS_S {
    static float epsilon = 0.3f;
    static float delta = 5.0f;
    static Distances distance;
};

namespace FRECHET_S {
    static Distances distance;
};

#endif //DISTANCEMEASURESHMT_ALGO_SETTINGS_H
