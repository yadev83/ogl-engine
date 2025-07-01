#pragma once

#include <engine/ecs/entity.hpp>
#include <engine/scene/scene.hpp>
#include <engine/scene/behaviour.hpp>

class PrefabBehaviour : public Engine::Scene::Behaviour {
    public:
        void OnUpdate(float dt) override;
        void OnFixedUpdate(float dt) override;
        void OnLateUpdate(float dt) override;
};

Engine::ECS::Entity Prefab(Engine::Scene::Scene* scene);