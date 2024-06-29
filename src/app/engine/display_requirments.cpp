#include "display_requirements.h"

DR *DR::instance = nullptr;

DR::DR() {
    autoMode = true;
    mouse_pos = {0.0f, 0.0f};
}

DR::~DR() {
    delete context;
}

DR *DR::getI() {
    if (instance == nullptr) {
        instance = new DR();
    }
    return instance;
}

void DR::setAlignment(std::tuple<float, std::vector<int>, float *> &p_alignment) {
    instance->alignment = p_alignment;
}

void DR::setWDTWAlignment(std::tuple<float, std::vector<int>, float *> &p_alignment) {
    instance->wdtw_alignment = p_alignment;
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

void DR::setMousePos(const std::pair<int, int> &mouse_pos) {
    instance->mouse_pos = mouse_pos;
}

void DR::setMode(bool mode) {
    instance->autoMode = mode;
}

std::tuple<float, std::vector<int>, float *> DR::getAlignment() const {
    return instance->alignment;
}

std::tuple<float, std::vector<int>, float *> DR::getWDTWAlignment() const {
    return instance->wdtw_alignment;
}

std::vector<Frame> DR::getRef_frames() const {
    return instance->ref_frames;
}

std::vector<Frame> DR::getInp_frames() const {
    return instance->inp_frames;
}

UIContext *DR::getContext() const {
    return instance->context;
}

std::pair<int, int> DR::getMousePos() const {
    return instance->mouse_pos;
}

bool DR::is_auto() const {
    return instance->autoMode;
}