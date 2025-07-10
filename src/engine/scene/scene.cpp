#include "scene.hpp"

#include "transform.hpp"
#include "../physics/rigidbody.hpp"
#include "../physics/collider.hpp"
#include "../graphics/sprite.hpp"
#include "../ui/text.hpp"

namespace Engine::Scene {
    Scene::Scene() {
        mRegistry.mScene = this;
    }

    ECS::Registry* Scene::GetRegistry() { return &mRegistry; }
    ICamera* Scene::GetCamera() { return mCamera; }
    App& Scene::GetApp() { return *mApp; }

    ECS::Entity Scene::CreateEntity(PrimitiveType type) {
        EntityID id = mRegistry.CreateEntity();
        ECS::Entity entity(id, &mRegistry);

        switch(type) {
            case PrimitiveType::Quad:
                entity.AddComponent<Transform>();
                entity.AddComponent<Physics::Rigidbody>();
                entity.AddComponent<Physics::BoxCollider>();
                entity.AddComponent<Graphics::Sprite>();
                break;

            case PrimitiveType::Sprite:
                entity.AddComponent<Transform>();
                entity.AddComponent<Graphics::Sprite>();
                break;

            case PrimitiveType::Text:
                entity.AddComponent<Transform>();
                entity.AddComponent<UI::Text>(UI::Text{.text="Text"});
                break;

            case PrimitiveType::Empty:
            default:
                break;
        }

        return entity;
    }

    void Scene::DestroyEntity(EntityID entityID) {
        mRegistry.DestroyEntity(entityID);
    }
}