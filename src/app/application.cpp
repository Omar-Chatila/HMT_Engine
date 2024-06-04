#include "application.h"

Application::Application() {
    startup();
}

Application::~Application() {
    shutdown();
}

void Application::startup() {

}

void Application::activate() {
    for (auto& layer : layer_stack)
		layer->onRender();
}

void Application::shutdown() {
    for (auto& layer : layer_stack)
        layer->onDettach();

    layer_stack.clear();
}
