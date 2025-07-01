#include "hello_world.hpp"

#include <engine/app.hpp>
#include <engine/input.hpp>
#include <engine/scene.hpp>
#include <engine/ui.hpp>
#include <engine/graphics.hpp>
#include <engine/physics.hpp>

using namespace Engine;
using namespace Engine::Input;
using namespace Engine::Scene;

void HelloWorld::OnEnter() {
    LOG_INFO("Hello World !");

    auto text = CreateEntity(PrimitiveType::Text);
    text.GetComponent<UI::Text>().anchor = Anchor::Top;
    text.GetComponent<UI::Text>().text = "Hello there !";
    text.GetComponent<Transform>().position.y = GetApp().GetHeight() * 0.5f;
}

void HelloWorld::OnUpdate(float deltaTime) {
    if(InputManager::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT).IsPressed()) {
        glm::vec2 worldPos = GetApp().GetProjectedMousePosition();
        auto box = CreateEntity(PrimitiveType::Quad);
        box.GetComponent<Transform>().position = glm::vec3(worldPos, 0.0f);
        box.GetComponent<Transform>().scale = glm::vec3(15.0f, 15.0f, 0.0f);
    }
}