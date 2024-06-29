#include "layer.h"
#include <imgui/imgui.h>

class ExampleLayer : public Layer {
public:
	virtual void onRender() override {
		ImGui::ShowDemoWindow();
	}
};
