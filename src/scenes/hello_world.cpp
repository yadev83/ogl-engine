#include "hello_world.hpp"

#include <engine/app.hpp>
#include <engine/input.hpp>
#include <engine/scene.hpp>
#include <engine/ui.hpp>
#include <engine/graphics.hpp>
#include <engine/physics.hpp>
#include <engine/utils.hpp>

using namespace Engine;
using namespace Engine::Scene;
using namespace Engine::Physics;

void HelloWorld::OnEnter() {
    LOG_INFO("Hello World !");

    auto text = CreateEntity(PrimitiveType::Text);
    text.GetComponent<UI::Text>().anchor = Anchor::Top;
    text.GetComponent<UI::Text>().text = "Hello there !";
    text.GetComponent<Transform>().position.y = GetApp().GetHeight() * 0.45f;

    // Create the walls
    auto wallTop = CreateEntity(PrimitiveType::Quad);
    wallTop.GetComponent<Transform>().position.y = GetApp().GetHeight() * 0.5f;
    wallTop.GetComponent<Transform>().scale = glm::vec3(GetApp().GetWidth(), 50.0f, 0.0f);
    wallTop.GetComponent<Rigidbody>().isKinematic = true;
    auto wallBot = CreateEntity(PrimitiveType::Quad);
    wallBot.GetComponent<Transform>().position.y = GetApp().GetHeight() * -0.5f;
    wallBot.GetComponent<Transform>().scale = glm::vec3(GetApp().GetWidth(), 50.0f, 0.0f);
    wallBot.GetComponent<Rigidbody>().isKinematic = true;
    auto wallLeft = CreateEntity(PrimitiveType::Quad);
    wallLeft.GetComponent<Transform>().position.x = GetApp().GetWidth() * -0.5f;
    wallLeft.GetComponent<Transform>().scale = glm::vec3(50.0f, GetApp().GetHeight(), 0.0f);
    wallLeft.GetComponent<Rigidbody>().isKinematic = true;
    auto wallRight = CreateEntity(PrimitiveType::Quad);
    wallRight.GetComponent<Transform>().position.x = GetApp().GetWidth() * 0.5f;
    wallRight.GetComponent<Transform>().scale = glm::vec3(50.0f, GetApp().GetHeight(), 0.0f);
    wallRight.GetComponent<Rigidbody>().isKinematic = true;
}

void HelloWorld::OnFixedUpdate(float deltaTime) {
    if(Input::InputManager::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT).IsJustPressed()) {
        if(activeIds.size() > 100) {
            auto id = activeIds.front();
            DestroyEntity(id);

            activeIds.pop_front();
        }

        glm::vec2 worldPos = GetApp().GetProjectedMousePosition();
        auto box = CreateEntity(PrimitiveType::Quad);
        activeIds.push_back(box.GetID());
        box.GetComponent<Transform>().position = glm::vec3(0.0f);
        box.GetComponent<Transform>().scale = glm::vec3(15.0f, 15.0f, 0.0f);
        box.GetComponent<Rigidbody>().velocity = glm::normalize(glm::vec3(worldPos, 0.0f)) * 1000.0f * deltaTime;
        box.GetComponent<Rigidbody>().isBounceable = true;
    }
}