#include "entity.hpp"

namespace Engine::ECS {
    Entity::Entity(EntityID id, Registry* registry) : mID(id), mRegistry(registry) {}
    
    std::string Entity::ToString() const {
        std::string output = "\n========== Entity";
        output += ("\n- ID : " + std::to_string(this->mID));

        auto tagList = mRegistry->GetTags(GetID());
        if(tagList.size()) {
            output += "\n- Tags : ";
            for(auto tag : tagList) {
                output += tag + "; ";
            }
        }

        output += "\n==========";
        return output;
    }

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