#include "scene.hpp"

namespace Engine::Scene {
    Scene::Scene() {
        mRegistry.mScene = this;
    }

    ECS::Registry* Scene::GetRegistry() { return &mRegistry; }
    ICamera* Scene::GetCamera() { return mCamera; }
    App& Scene::GetApp() { return *mApp; }

    ECS::Entity Scene::CreateEntity() {
        EntityID id = mRegistry.CreateEntity();
        return ECS::Entity(id, &mRegistry);
    }

    void Scene::DestroyEntity(EntityID entityID) {
        mRegistry.DestroyEntity(entityID);
    }
}