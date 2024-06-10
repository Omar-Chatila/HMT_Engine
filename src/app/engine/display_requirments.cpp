#include "display_requirements.h"

DisplayRequirements* DisplayRequirements::instance = nullptr;

DisplayRequirements::~DisplayRequirements() {
    delete context;
}

DisplayRequirements *DisplayRequirements::getI() {
    if (instance == nullptr) {
        instance = new DisplayRequirements();
    }
    return instance;
}

void DisplayRequirements::setAlignment(std::tuple<float, std::vector<int>, float*> &alignment) {
    instance->alignment = alignment;
}

void DisplayRequirements::setContext(UIContext *cont) {
    instance->context = cont;
}

void DisplayRequirements::setInpFrames(std::vector<Frame> &inp_frms) {
    instance->inp_frames = inp_frms;
}

void DisplayRequirements::setRefFrames(std::vector<Frame> &ref_frms) {
    instance->ref_frames = ref_frms;
}

std::tuple<float, std::vector<int>, float*> DisplayRequirements::getAlignment() const {
    return instance->alignment;
} 

std::vector<Frame> DisplayRequirements::getRef_frames() const {
    return instance->ref_frames;
}

std::vector<Frame> DisplayRequirements::getInp_frames () const {
    return instance->inp_frames;
}

UIContext* DisplayRequirements::getContext() const {
    return instance->context;
}