#include <iostream>
#include <array>
#include <cassert>

using namespace std;

void test1();

void test2();

void write_matrix_to_file(int *S, size_t rows, size_t cols, const std::string &filename) {
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

#define INFTY 1000

int SELECT(const vector<int> &inp_traj, const vector<int> &ref_traj, int n, int m) {
    assert(n >= 0 && m >= 0);
    assert(n < inp_traj.size());
    assert(m < ref_traj.size());
    auto res = std::abs(inp_traj[n] - ref_traj[m]);
    return res;
}

#define INDEX4(n, m) ((n) * (M + 2) + (m))

int *ss1_dtw(const std::vector<int> &inp_traj, const std::vector<int> &ref_traj) {
    const int N = inp_traj.size();
    const int M = ref_traj.size();

    auto *S = (int *) malloc((N + 2) * (M + 2) * sizeof(int));
    const int INF = 1000;

    for (int n = 0; n < N + 2; ++n) {
        S[INDEX4(n, 0)] = INF;
        S[INDEX4(n, 1)] = INF;
    }
    for (int m = 0; m < M + 2; ++m) {
        S[INDEX4(0, m)] = INF;
        S[INDEX4(1, m)] = INF;
    }

    S[INDEX4(0, 0)] = 0;
    S[INDEX4(1, 1)] = SELECT(inp_traj, ref_traj, 0, 0);

    for (int n = 2; n < N + 2; ++n) {
        for (int m = 2; m < M + 2; ++m) {
            S[INDEX4(n, m)] = std::min({
                                               S[INDEX4(n - 1, m - 1)],
                                               S[INDEX4(n - 2, m - 1)],
                                               S[INDEX4(n - 1, m - 2)]
                                       }) + SELECT(inp_traj, ref_traj, n - 2, m - 2);
        }
    }

    write_matrix_to_file(S, N + 2, M + 2, "SSC1.txt");
    return S;
}

std::pair<int, std::vector<int>> get_cost_and_alignment_ss1(int *D, int N, int M) {
    int index = (N + 2) * (M + 2) - 1;
    std::cout << "I: " << index << std::endl;
    std::vector<int> alignment;
    const int cost = D[index];
    const int ROW = M + 2;
    while (index / (M + 2) >= 2 && index % (M + 2) >= 2) {
        alignment.push_back(index);
        int i1 = index - 1 * (ROW) - 1;
        int i2 = index - 2 * (ROW) - 1;
        int i3 = index - 1 * (ROW) - 2;

        if (D[i1] < D[i2] && D[i1] < D[i3]) {
            index = i1;
        } else if (D[i2] < D[i3]) {
            index = i2;
        } else {
            index = i3;
        }
    }
    std::reverse(alignment.begin(), alignment.end());
    free(D);
    return {cost, alignment};
}


int *ssc2_dtw(const vector<int> &inp_traj, const vector<int> &ref_traj) {
    const size_t N = inp_traj.size();
    const size_t M = ref_traj.size();
    int *D = new int[(N + 3) * (M + 3)];

    // Initial values
    for (int n = 0; n < N + 3; ++n) {
        D[INDEX2(n, 0, M)] = INFTY;
        D[INDEX2(n, 1, M)] = INFTY;
        D[INDEX2(n, 2, M)] = INFTY;
    }
    for (int m = 0; m < M + 3; ++m) {
        D[INDEX2(0, m, M)] = INFTY;
        D[INDEX2(1, m, M)] = INFTY;
        D[INDEX2(2, m, M)] = INFTY;
    }

    D[INDEX2(3, 3, M)] = SELECT(inp_traj, ref_traj, 0, 0);

    for (int n = 3; n < N + 3; ++n) {
        for (int m = 3; m < M + 3; ++m) {
            if (n == 3 && m == 3) continue;
            int cost_n_m = SELECT(inp_traj, ref_traj, n - 3, m - 3);
            int cost_n_1_m = n > 3 ? SELECT(inp_traj, ref_traj, n - 4, m - 3) : INFTY;
            int cost_n_m_1 = m > 3 ? SELECT(inp_traj, ref_traj, n - 3, m - 4) : INFTY;
            int cost_n_2_m = n > 4 ? SELECT(inp_traj, ref_traj, n - 5, m - 3) : INFTY;
            int cost_n_m_2 = m > 4 ? SELECT(inp_traj, ref_traj, n - 3, m - 5) : INFTY;
            D[INDEX2(n, m, M)] = std::min({
                                                  D[INDEX2(n - 1, m - 1, M)] + cost_n_m,
                                                  D[INDEX2(n - 2, m - 1, M)] + cost_n_1_m + cost_n_m,
                                                  D[INDEX2(n - 1, m - 2, M)] + cost_n_m_1 + cost_n_m,
                                                  D[INDEX2(n - 3, m - 1, M)] + cost_n_2_m + cost_n_1_m + cost_n_m,
                                                  D[INDEX2(n - 1, m - 3, M)] + cost_n_m_2 + cost_n_m_1 + cost_n_m
                                          });
            std::cout << "(" << n - 3 << ": " << m - 3 << ") " << D[INDEX2(n, m, M)] << "|";
        }
        std::cout << "\n";
    }
    write_matrix_to_file(D, N + 3, M + 3, "SSC2");
    return D;
}

std::pair<int, std::vector<int>> get_cost_and_alignment_ss2(int *D, int N, int M) {
    int index = (N + 3) * (M + 3) - 1;
    std::vector<int> alignment;
    const int cost = D[index];
    const int ROW = M + 3;
    while (index / (M + 3) >= 3 && index % (M + 3) >= 3) {
        alignment.push_back(index);
        int i1 = index - 3 * (ROW) - 1;
        int i2 = index - 2 * (ROW) - 1;
        int i3 = index - 1 * (ROW) - 1;
        int i4 = index - 1 * (ROW) - 2;
        int i5 = index - 1 * (ROW) - 3;
        if (D[i1] < D[i2] && D[i1] < D[i3] && D[i1] < D[i4] && D[i1] < D[i5]) {
            index = i1;
        } else if (D[i2] < D[i3] && D[i2] < D[i4] && D[i2] < D[i5]) {
            index = i2;
        } else if (D[i3] < D[i4] && D[i3] < D[i5]) {
            index = i3;
        } else if (D[i4] < D[i5]) {
            index = i4;
        } else {
            index = i5;
        }
    }
    std::reverse(alignment.begin(), alignment.end());
    free(D);
    return {cost, alignment};
}

void test_DTW_SSC2() {
    vector<int> in = {2, 4, 1, 5, 4, 2, 7};
    vector<int> re = {4, 2, 1, 8, 5, 2, 8};
    auto matrix = ssc2_dtw(in, re);
    int N = static_cast<int>(in.size());
    int M = static_cast<int>(re.size());
    auto res = get_cost_and_alignment_ss2(matrix, N, M);
    std::cout << res.first << std::endl;
    for (auto i: res.second) {
        std::cout << i / (N + 3) << ":" << i % (M + 3) << std::endl;
    }
}

void test_DTW_SSC1() {
    vector<int> in = {2, 4, 1, 5, 4, 2, 5, 1};
    vector<int> re = {4, 2, 1, 8, 3, 5, 2, 2};
    auto matrix = ss1_dtw(in, re);
    int N = static_cast<int>(in.size());
    int M = static_cast<int>(re.size());
    auto res = get_cost_and_alignment_ss1(matrix, N, M);
    std::cout << res.first << std::endl;
    for (auto i: res.second) {
        std::cout << i / (N + 2) << ":" << i % (M + 2) << std::endl;
    }
}


void test() {

    // Parse input trajectory
    /*
    SharedData *sharedData = new SharedData();
    MotionFileProcessor *motionFileProcessor = new MotionFileProcessor(SQUATS);
    const char *default_file = "fb_41_pre_splitted_1.txt";
    //motionFileProcessor->processInputFile(std::string(default_file));
    DR *disp_req = DR::getI();
    auto results = motionFileProcessor->getKClosestMatches(3, DTW);
    TrajectoryAnalysisManager *manager = results.front();
    manager->updateDisplayRequirements();

    /*
    std::string input_file = R"(resources\test\fb_41_pre_splitted_1.txt)";
    Input_parser* input = new Input_parser(input_file.c_str());
    std::vector<Frame> input_frames = input->get_frames();
    //Trajectories* input_trajectories = new Trajectories(input_frames);
    /*
    std::cout << "input traj created" << std::endl;

    // Parse reference trajectory
    std::string ref_file = R"(resources\test\expertise_01_single100_2_splitted_1.txt)";
    Input_parser* reference = new Input_parser(ref_file.c_str());
    std::vector<Frame> ref_frms = reference->get_frames();
    Trajectories* ref_trajcts = new Trajectories(ref_frms);

    std::cout << "ref traj created" << std::endl;

    Trajectoy_analysis* analysis = new Trajectoy_analysis(*input_trajectories, *ref_trajcts);
    auto quats_in = input_trajectories->get_anglesTrajectories();
    auto quats_ref = ref_trajcts->get_anglesTrajectories();

    std::cout << "dist " << quaternion_dist(quats_in[0], quats_ref[200]);


    std::tuple<float, std::vector<int>, float*> alignment = analysis->perform_DTW(input_trajectories->get_anglesTrajectories(), ref_trajcts->get_anglesTrajectories());
    std::cout << "Cost: " << std::get<0>(alignment) << std::endl;
    int last = std::get<1>(alignment)[std::get<1>(alignment).size() - 1];

    std::cout << "dtw" << std::endl;

    int n = input_trajectories->get_anglesTrajectories().size();
    int m = ref_trajcts->get_anglesTrajectories().size();
    std::cout << last / (m + 1) << ", " << last % (m + 1) << std::endl;

    std::string squats_info = R"(resources\squats_subject_info.csv)";

    delete analysis;
    delete ref_trajcts;
    delete reference;
    delete input_trajectories;
    delete input;
*/
}

/*
void test1() {
    Quaternion* joint_rotations1 = new Quaternion[JOINT_COUNT];
    Quaternion* joint_rotations2 = new Quaternion[JOINT_COUNT];
    Quaternion* joint_rotations3 = new Quaternion[JOINT_COUNT];
    Quaternion* joint_rotations4 = new Quaternion[JOINT_COUNT];
    Quaternion* joint_rotations5 = new Quaternion[JOINT_COUNT];
    Vec3D* joint_translations = (Vec3D*)calloc(JOINT_COUNT, sizeof(Vec3D));

    joint_rotations1[0] = {-0.999332f, -0.023652f, 0.027844f, 0.000959f};                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       joint_rotations1[0] = {-0.999332, -0.023652, 0.027844, 0.000959};
    joint_rotations1[1] = {0.999726f, -0.020961f, -0.007759f, -0.006978f};
    joint_rotations1[2] = {0.999864f, 0.014422f, -0.007761f, 0.001601f};
    joint_rotations1[3] = {0.98349f, 0.180619f, 0.001061f, 0.011067f};
    joint_rotations1[4] = {0.997289f, -0.073544f, -0.000845f, -0.001929f};
    joint_rotations1[5] = {0.997027f, 0.03738f, -0.04256f, 0.052247f};
    joint_rotations1[6] = {0.700886f, 0.338549f, -0.363797f, -0.511658f};
    joint_rotations1[7] = {0.983605f, -0.019397f, -0.17888f, 0.012108f};
    joint_rotations1[8] = {0.998854f, -0.019697f, -0.017401f, -0.040014f};
    joint_rotations1[9] = {0.986987f, 0.10084f, 0.12525f, -0.000366f};
    joint_rotations1[10] = {0.746249f, 0.306097f, 0.335233f, 0.486865f};
    joint_rotations1[11] = {0.969757f, -0.117973f, 0.197718f, -0.081f};
    joint_rotations1[12] = {0.989064f, -0.120319f, -0.067038f, 0.052744f};
    joint_rotations1[13] = {0.997188f, -0.022603f, 0.017014f, 0.069397f};
    joint_rotations1[14] = {0.992725f, 0.108246f, 0.005206f, -0.05247f};
    joint_rotations1[15] = {0.98717f, -0.104636f, 0.116078f, -0.032736f};
    joint_rotations1[16] = {0.994777f, -0.044975f, -0.011373f, -0.09092f};
    joint_rotations1[17] = {0.991928f, 0.113739f, -0.009885f, 0.055184f};
    joint_rotations1[18] = {0.991949f, -0.089173f, -0.078372f, 0.044065f};

    for (int i = 0; i < JOINT_COUNT; i++) {
        joint_rotations2[i] = {joint_rotations1[i].w + 0.01f, joint_rotations1[i].w + 0.01f, joint_rotations1[i].w + 0.01f,joint_rotations1[i].w + 0.01f};
    }

    for (int i = 0; i < JOINT_COUNT; i++) {
        joint_rotations3[i] = {joint_rotations2[i].w + 0.01f, joint_rotations2[i].w + 0.01f, joint_rotations2[i].w + 0.01f,joint_rotations2[i].w + 0.01f};
    }

    for (int i = 0; i < JOINT_COUNT; i++) {
        joint_rotations4[i] = {joint_rotations1[i].w - 0.01f, joint_rotations1[i].w + 0.01f, joint_rotations1[i].w - 0.04f,joint_rotations1[i].w + 0.01f};
    }

    for (int i = 0; i < JOINT_COUNT; i++) {
        joint_rotations5[i] = {joint_rotations2[i].w - 0.01f, joint_rotations2[i].w - 0.03f, joint_rotations2[i].w + 0.01f,joint_rotations2[i].w + 0.01f};
    }

    // Initialize test frames
    std::vector<Frame> frames = {
        {1, {1.0f, 2.0f, 3.0f}, joint_rotations1, joint_translations, {"label1", "label2", "label3"}},
        {1, {1.0f, 2.0f, 3.0f}, joint_rotations2, joint_translations, {"label1", "label2", "label3"}},
        {1, {1.0f, 2.0f, 3.0f}, joint_rotations3, joint_translations, {"label1", "label2", "label3"}}
    };

    // Initialize test frames
    std::vector<Frame> frames2 = {
        {1, {1.0f, 2.0f, 3.0f}, joint_rotations4, joint_translations, {"label1", "label2", "label3"}},
        {1, {1.0f, 2.0f, 3.0f}, joint_rotations5, joint_translations, {"label1", "label2", "label3"}}
    };

    // Test dot product
    double dotP = joint_rotations1[0] * joint_rotations1[1];
    std::cout << dotP << std::endl;

    // Test cost function
    double cost = quaternion_dist(joint_rotations1, joint_rotations4);
    std::cout << "COST1: " << cost << std::endl;
    // Test cost function
    double cost1 = quaternion_dist(joint_rotations1, joint_rotations5);
    std::cout << "COST1: " << cost1 << std::endl;
    // Test cost function
    double cost2 = quaternion_dist(joint_rotations2, joint_rotations4);
    std::cout << "COST1: " << cost2 << std::endl;
    // Test cost function
    double cost3 = quaternion_dist(joint_rotations2, joint_rotations5);
    std::cout << "COST1: " << cost3 << std::endl;
    // Test cost function
    double cost4 = quaternion_dist(joint_rotations3, joint_rotations4);
    std::cout << "COST1: " << cost4 << std::endl;
    // Test cost function
    double cost5 = quaternion_dist(joint_rotations3, joint_rotations5);
    std::cout << "COST1: " << cost5 << std::endl;

    Trajectories* t = new Trajectories(frames);
    const auto& angles = t->get_anglesTrajectories();
    Trajectories* t2 = new Trajectories(frames2);
    const auto& angles2 = t2->get_anglesTrajectories();
    std::cout << "df" << std:: endl;
    float* matrix = Dtw::dtw(angles, angles2, quaternion_dist);
    std::cout << "df" << std:: endl;
    auto alignment = Dtw::get_cost_and_alignment(matrix, angles.size(), angles2.size());
    std::cout << alignment.first << std::endl;

}
    */

/*
void test2() {
    Vec3D *joint_rotations1 = new Vec3D[JOINT_COUNT];
    Vec3D *joint_rotations2 = new Vec3D[JOINT_COUNT];
    Vec3D *joint_rotations3 = new Vec3D[JOINT_COUNT];
    Vec3D *joint_rotations4 = new Vec3D[JOINT_COUNT];
    Vec3D *joint_rotations5 = new Vec3D[JOINT_COUNT];
    Vec3D *joint_rotations6 = new Vec3D[JOINT_COUNT];

    joint_rotations1[0] = Vec3D(-0.1f, -0.2f, 0.5f);
    joint_rotations1[1] = Vec3D(0.999726f, -0.020961f, -0.007759f);
    joint_rotations1[2] = Vec3D(0.999864f, 0.014422f, -0.007761f);
    joint_rotations1[3] = Vec3D(0.98349f, 0.180619f, 0.001061f);
    joint_rotations1[4] = Vec3D(0.997289f, -0.073544f, -0.000845f);
    joint_rotations1[5] = Vec3D(0.997027f, 0.03738f, -0.04256f);
    joint_rotations1[6] = Vec3D(0.700886f, 0.338549f, -0.363797f);
    joint_rotations1[7] = Vec3D(0.983605f, -0.019397f, -0.17888f);
    joint_rotations1[8] = Vec3D(0.998854f, -0.019697f, -0.017401f);
    joint_rotations1[9] = Vec3D(0.986987f, 0.10084f, 0.12525f);
    joint_rotations1[10] = Vec3D(0.746249f, 0.306097f, 0.335233f);
    joint_rotations1[11] = Vec3D(0.969757f, -0.117973f, 0.197718f);
    joint_rotations1[12] = Vec3D(0.989064f, -0.120319f, -0.067038f);
    joint_rotations1[13] = Vec3D(0.997188f, -0.022603f, 0.017014f);
    joint_rotations1[14] = Vec3D(0.992725f, 0.108246f, 0.005206f);
    joint_rotations1[15] = Vec3D(0.98717f, -0.104636f, 0.116078f);
    joint_rotations1[16] = Vec3D(0.994777f, -0.044975f, -0.011373f);
    joint_rotations1[17] = Vec3D(0.991928f, 0.113739f, -0.009885f);
    joint_rotations1[18] = Vec3D(0.991949f, -0.089173f, -0.078372f);

    srand(0);
    for (int i = 0; i < JOINT_COUNT; i++) {
        joint_rotations2[i].x = joint_rotations1[i].x + (rand() % 10) / 10.0;
        joint_rotations2[i].y = joint_rotations1[i].y + (rand() % 10) / 10.0;
        joint_rotations2[i].z = joint_rotations1[i].z + (rand() % 10) / 10.0;

        joint_rotations3[i].x = joint_rotations1[i].x + (rand() % 10) / 10.0;
        joint_rotations3[i].y = joint_rotations1[i].y + (rand() % 10) / 10.0;
        joint_rotations3[i].z = joint_rotations1[i].z + (rand() % 10) / 10.0;

        joint_rotations4[i].x = joint_rotations1[i].x + (rand() % 10) / 10.0;
        joint_rotations4[i].y = joint_rotations1[i].y + (rand() % 10) / 10.0;
        joint_rotations4[i].z = joint_rotations1[i].z + (rand() % 10) / 10.0;

        joint_rotations5[i].x = joint_rotations1[i].x + (rand() % 10) / 10.0;
        joint_rotations5[i].y = joint_rotations1[i].y + (rand() % 10) / 10.0;
        joint_rotations5[i].z = joint_rotations1[i].z + (rand() % 10) / 10.0;

        joint_rotations6[i].x = joint_rotations1[i].x + (rand() % 10) / 10.0;
        joint_rotations6[i].y = joint_rotations1[i].y + (rand() % 10) / 10.0;
        joint_rotations6[i].z = joint_rotations1[i].z + (rand() % 10) / 10.0;

    }

    // Initialize test frames
    std::vector<Frame> frames = {
            {1, {1.0f, 2.0f, 3.0f}, nullptr, joint_rotations1},
            {1, {1.0f, 2.0f, 3.0f}, nullptr, joint_rotations2},
            {1, {1.0f, 2.0f, 3.0f}, nullptr, joint_rotations3}
    };

    // Initialize test frames
    std::vector<Frame> frames2 = {
            {1, {1.0f, 2.0f, 3.0f}, nullptr, joint_rotations4},
            {1, {1.0f, 2.0f, 3.0f}, nullptr, joint_rotations5},
            {1, {1.0f, 2.0f, 3.0f}, nullptr, joint_rotations6}
    };

    Trajectories *t1 = new Trajectories(frames);
    Vec3D *hips1 = t1->get_positionsTrajectory(Joint::HumanoidRoot);
    for (int i = 0; i < 3; i++) {
        std::cout << hips1[i];
    }
    std::cout << std::endl;
    Trajectories *t2 = new Trajectories(frames2);
    Vec3D *hips2 = t2->get_positionsTrajectory(Joint::HumanoidRoot);
    for (int i = 0; i < 3; i++) {
        std::cout << hips2[i];
    }

    EditDistance::edr(hips1, hips2, 3, 3, 0.8, vec_dist_measures[EUCLID]);

    delete[] joint_rotations1;
    delete[] joint_rotations2;
    delete[] joint_rotations3;
    delete[] joint_rotations4;
    delete[] joint_rotations5;
    delete[] joint_rotations6;
}
*/