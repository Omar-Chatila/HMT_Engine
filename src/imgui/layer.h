#pragma once
#include "layer_data.h"

class Layer{
public:
	virtual ~Layer() = default;

	virtual void onAttach() {};
	virtual void onDettach() {};
	virtual void onRender() {};

protected:
    SharedData* sharedData;
};