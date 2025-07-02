#pragma once

#include <engine/scene/scene.hpp>
#include <engine/ecs/entity.hpp>

class HelloWorld : public Engine::Scene::Scene {
    private:
        std::deque<Engine::EntityID> activeIds;

    public:
        void OnEnter() override;
        void OnFixedUpdate(float deltaTime) override;
};