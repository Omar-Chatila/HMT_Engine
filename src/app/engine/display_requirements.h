#pragma once

#include <vector>
#include "ui_context.h"

class DR {
private:
    
public:
    static DR* getI();
    static DR *instance;
    
    void setRefFrames(std::vector<Frame> &ref_frames);
    void setInpFrames(std::vector<Frame> &inp_frames);
    void setContext(UIContext *context);
    void setAlignment(std::tuple<float, std::vector<int>, float*> &alignment);

    std::vector<Frame> getRef_frames() const;
    std::vector<Frame> getInp_frames() const;
    UIContext *getContext() const;
    std::tuple<float, std::vector<int>, float*> getAlignment() const;
private:
    DR() = default;     
    ~DR();

    std::vector<Frame> ref_frames;
    std::vector<Frame> inp_frames;
    UIContext *context;
    std::tuple<float, std::vector<int>, float*> alignment;
};