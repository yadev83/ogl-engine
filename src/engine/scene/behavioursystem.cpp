#include "behavioursystem.hpp"

#include <iostream>

namespace Engine::Scene {
    void BehaviourSystem::OnInit() {
        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Behaviour>()) {
            std::vector<Behaviour*> scripts = GetRegistry().GetComponents<Behaviour>(entityID);

            for(auto script : scripts) {
                if(script->enabled) script->OnInit();
            }
        }
    }

    void BehaviourSystem::OnFixedUpdate(float deltaTime) {
        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Behaviour>()) {
            std::vector<Behaviour*> scripts = GetRegistry().GetComponents<Behaviour>(entityID);

            for(auto script : scripts) {
                if(script->enabled) script->OnFixedUpdate(deltaTime);
            }
        }
    }

    void BehaviourSystem::OnUpdate(float deltaTime) {
        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Behaviour>()) {
            std::vector<Behaviour*> scripts = GetRegistry().GetComponents<Behaviour>(entityID);

            for(auto script : scripts) {
                if(script->enabled) script->OnUpdate(deltaTime);
            }
        }
    }
    
    void BehaviourSystem::OnRender(float alpha) {
        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Behaviour>()) {
            std::vector<Behaviour*> scripts = GetRegistry().GetComponents<Behaviour>(entityID);

            for(auto script : scripts) {
                if(script->enabled) script->OnRender(alpha);
            }
        }
    }

        
    void BehaviourSystem::OnUIRender() {
        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Behaviour>()) {
            std::vector<Behaviour*> scripts = GetRegistry().GetComponents<Behaviour>(entityID);

            for(auto script : scripts) {
                if(script->enabled) script->OnUIRender();
            }
        }
    }

    void BehaviourSystem::OnLateUpdate(float deltaTime) {
        for(EntityID entityID: GetRegistry().GetEntityIDsWith<Behaviour>()) {
            std::vector<Behaviour*> scripts = GetRegistry().GetComponents<Behaviour>(entityID);

            for(auto script : scripts) {
                if(script->enabled) script->OnLateUpdate(deltaTime);
            }
        }
    }
}