#pragma once

#include <vector>
#include "ui_context.h"

class DR {
private:

public:
    static DR *getI();

    static DR *instance;

    void setRefFrames(std::vector <Frame> &ref_frames);

    void setInpFrames(std::vector <Frame> &inp_frames);

    void setContext(UIContext *context);

    void setAlignment(std::tuple<float, std::vector<int>, float *> &alignment);

    void setMousePos(const std::pair<int, int> &mouse_pos);

    void setMode(bool mode);

    std::vector <Frame> getRef_frames() const;

    std::vector <Frame> getInp_frames() const;

    UIContext *getContext() const;

    std::tuple<float, std::vector<int>, float *> getAlignment() const;

    std::pair<int, int> getMousePos() const;

    bool is_auto() const;

private:
    DR();

    ~DR();

    std::vector <Frame> ref_frames;
    std::vector <Frame> inp_frames;
    UIContext *context;
    std::tuple<float, std::vector<int>, float *> alignment;
    std::pair<int, int> mouse_pos;
    bool autoMode;
};