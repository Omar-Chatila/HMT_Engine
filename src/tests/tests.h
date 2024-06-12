#include <stdio.h>
#include <stdlib.h>
#include "distance_measures.h"
#include "trajectory_analysis.h"
#include "dtw.h"

void test1();

void test() {
    // Parse input trajectory
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