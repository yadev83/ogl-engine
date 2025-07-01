#pragma once

#include <engine/scene/scene.hpp>

class HelloWorld : public Engine::Scene::Scene {
    public:
        void OnEnter() override;
        void OnUpdate(float deltaTime) override;
};