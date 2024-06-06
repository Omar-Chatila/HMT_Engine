#pragma once

class Layer{
public:
	virtual ~Layer() = default;

	virtual void onAttach() {};
	virtual void onDettach() {};
	virtual void onRender() {};
};