/**
 * @file behavioursystem.hpp
 * @brief Appelle les callbacks de cycle de vie sur les components Behaviour actifs
 */
#pragma once

#include "../ecs/system.hpp"

#include "behaviour.hpp"

namespace Engine::Scene {
    /**
     * @brief Appelle les callbacks de cycle de vie sur les components Behaviour actifs
     */
    class BehaviourSystem : public ECS::System {
        public:
            void OnInit() override;
            void OnFixedUpdate(float deltaTime) override;
            void OnUpdate(float deltaTime) override;
            void OnRender(float alpha = 0.0f) override;
            void OnUIRender() override;
            void OnLateUpdate(float deltaTime) override;
    };
}