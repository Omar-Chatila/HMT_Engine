#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "Layer.h"

class Application{
private:
	std::vector<std::shared_ptr<Layer>> layer_stack;

public:
	Application();   // Konstruktor
    ~Application();  // Destruktor

	template<typename T>
	void push_layer()
	{
		static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
		layer_stack.emplace_back(std::make_shared<T>())->onAttach();
	}
	void push_layer(const std::shared_ptr<Layer>& layer) { layer_stack.emplace_back(layer); layer->onAttach(); }
	void startup();
	void activate();
	void shutdown();
};