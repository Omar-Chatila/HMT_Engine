#include "imgui_layers.h"

ImGUI_Layers::ImGUI_Layers() {
    startup();
}

ImGUI_Layers::~ImGUI_Layers() {
    shutdown();
}

void ImGUI_Layers::startup() {

}

void ImGUI_Layers::activate() {
    for (auto& layer : layer_stack)
		layer->onRender();
}

void ImGUI_Layers::shutdown() {
    for (auto& layer : layer_stack)
        layer->onDettach();

    layer_stack.clear();
}
