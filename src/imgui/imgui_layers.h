#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "Layer.h"

class ImGUI_Layers{
private:
	std::vector<std::shared_ptr<Layer>> layer_stack;

public:
	ImGUI_Layers();   // Konstruktor
    ~ImGUI_Layers();  // Destruktor

	template<typename T>
	void push_layer()
	{
		static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
		layer_stack.emplace_back(std::make_shared<T>())->onAttach();
	}
	template<typename T, typename... Args>
	void push_layer(Args&&... args) {
		static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
		layer_stack.emplace_back(std::make_shared<T>(std::forward<Args>(args)...))->onAttach();
	}
	void startup();
	void activate();
	void shutdown();
};