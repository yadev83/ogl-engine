#include "behavioursystem.hpp"

#include <iostream>

namespace Engine::Scene {
    void BehaviourSystem::OnInit() {
        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Behaviour>()) {
            auto& script = GetRegistry().GetComponent<Behaviour>(entityID);
            script.OnInit();
        }
    }

    void BehaviourSystem::OnFixedUpdate(float deltaTime) {
        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Behaviour>()) {
            auto& script = GetRegistry().GetComponent<Behaviour>(entityID);
            script.OnFixedUpdate(deltaTime);
        }
    }

    void BehaviourSystem::OnUpdate(float deltaTime) {
        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Behaviour>()) {
            auto& script = GetRegistry().GetComponent<Behaviour>(entityID);
            script.OnUpdate(deltaTime);
        }
    }
    
    void BehaviourSystem::OnRender(float alpha) {
        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Behaviour>()) {
            auto& script = GetRegistry().GetComponent<Behaviour>(entityID);
            script.OnRender(alpha);
        }
    }

        
    void BehaviourSystem::OnUIRender() {
        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Behaviour>()) {
            auto& script = GetRegistry().GetComponent<Behaviour>(entityID);
            script.OnUIRender();
        }
    }

    void BehaviourSystem::OnLateUpdate(float deltaTime) {
        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Behaviour>()) {
            auto& script = GetRegistry().GetComponent<Behaviour>(entityID);
            script.OnLateUpdate(deltaTime);
        }
    }
}