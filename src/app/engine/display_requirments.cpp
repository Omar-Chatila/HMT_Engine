#include "display_requirements.h"

DR* DR::instance = nullptr;

DR::~DR() {
    delete context;
}

DR *DR::getI() {
    if (instance == nullptr) {
        instance = new DR();
    }
    return instance;
}

void DR::setAlignment(std::tuple<float, std::vector<int>, float*> &alignment) {
    instance->alignment = alignment;
}

void DR::setContext(UIContext *cont) {
    instance->context = cont;
}

void DR::setInpFrames(std::vector<Frame> &inp_frms) {
    instance->inp_frames = inp_frms;
}

void DR::setRefFrames(std::vector<Frame> &ref_frms) {
    instance->ref_frames = ref_frms;
}

std::tuple<float, std::vector<int>, float*> DR::getAlignment() const {
    return instance->alignment;
} 

std::vector<Frame> DR::getRef_frames() const {
    return instance->ref_frames;
}

std::vector<Frame> DR::getInp_frames () const {
    return instance->inp_frames;
}

UIContext* DR::getContext() const {
    return instance->context;
}