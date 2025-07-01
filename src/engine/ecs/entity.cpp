#include "entity.hpp"

namespace Engine::ECS {
    Entity::Entity(EntityID id, Registry* registry) : mID(id), mRegistry(registry) {}
    
    Registry& Entity::GetRegistry() const { 
        return *mRegistry;
    }

    EntityID Entity::GetID() const {
        return mID;
    }
    
    void Entity::AddTag(std::string tag) {
        mRegistry->AddTag(GetID(), tag);
    }

    bool Entity::HasTag(std::string tag) {
        return mRegistry->HasTag(GetID(), tag);
    }
}