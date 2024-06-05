#ifndef MOTION_DATA_H
#define MOTION_DATA_H

#include <vector>
#include <string>
#include "util.h"
#include <string.h>

struct motion_data {
    std::string motion_file;
    char sex;
    int age;
    int size;
    int weight;
    bool right_handed;
    bool right_footed;
    int expertise;
    int expertise_practical;
    int expertise_theoretical;
    int experience_trainer;
};

std::vector<motion_data> motion_info(const char* file) {
    auto lines = readAllLines(file, 0);
    std::vector<motion_data> result;
    for (auto& line : lines) {
        std::cout << line << std::endl;
        motion_data data{};
        auto elements = split(line, ',');
        
        data.motion_file = elements[0];
        data.sex = elements[1].c_str()[0];
        data.age = atoi(elements[2].c_str());
        data.size = atoi(elements[3].c_str());
        data.weight = atoi(elements[4].c_str());
        data.right_handed = atoi(elements[5].c_str());
        if (elements[6] != "nA")
            data.right_footed = atoi(elements[6].c_str());
        data.expertise = atoi(elements[7].c_str());
        data.expertise_practical = atoi(elements[8].c_str());
        data.expertise_theoretical = atoi(elements[9].c_str());
        data.experience_trainer = atoi(elements[10].c_str());
        result.push_back(data);
    }
    return result;
}

#endif