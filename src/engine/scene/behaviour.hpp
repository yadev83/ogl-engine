/**
 * @file behaviour.hpp
 * @brief Component de "scripting" qui permet de profiter des fonctions de cycle de vie sur n'importe quelle entité
 * 
 */
#pragma once

#include "../ecs/component.hpp"
#include "../ecs/entity.hpp"
#include "../physics/manifold.hpp"

namespace Engine::Scene {
    /**
     * @brief Component de "scripting" qui permet de profiter des fonctions de cycle de vie sur n'importe quelle entité
     * 
     * Contient des callbacks pour toute sorte d'évènements :  
     * - Cycle de vie de l'application  
     * - Callbacks de collision / trigger  
     * - Callbacks pour les interfaces
     */
    class Behaviour : public ECS::Component {        
        public:
            using BaseType = Behaviour;
            Behaviour() = default;
            virtual ~Behaviour() = default;

            // Callback de cycle de vie
            virtual void OnInit() {}
            virtual void OnFixedUpdate(float deltaTime) {}
            virtual void OnUpdate(float deltaTime) {}
            virtual void OnRender(float alpha = 0.0f) {}
            virtual void OnUIRender() {}
            virtual void OnLateUpdate(float deltaTime) {}

            // Callbacks pour les systèmes physiques
            virtual void OnCollisionEnter(ECS::Entity other, Physics::CollisionManifold collision) {}
            virtual void OnTriggerEnter(ECS::Entity other, Physics::CollisionManifold collision) {}
            virtual void OnCollisionStay(ECS::Entity other, Physics::CollisionManifold collision) {}
            virtual void OnTriggerStay(ECS::Entity other, Physics::CollisionManifold collision) {}
            virtual void OnCollisionExit(ECS::Entity other, Physics::CollisionManifold collision) {}
            virtual void OnTriggerExit(ECS::Entity other, Physics::CollisionManifold collision) {}

            // Callbacks pour Actions d'interface
            virtual void OnHoverEnter() {}
            virtual void OnHoverExit() {}
            virtual void OnFocusEnter() {}
            virtual void OnFocusExit() {}
            virtual void OnSubmit() {}
    };
}