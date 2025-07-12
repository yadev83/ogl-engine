#include "entity.hpp"
#include "hierarchy.hpp"

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

    bool Entity::IsValid() const {
        return (mID > 0 && mRegistry && mRegistry->IsValidEntity(mID));
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

    void Entity::AddChild(EntityID childID) {
        mRegistry->AddChild(mID, childID);
    }

    void Entity::AddParent(EntityID parentID) {
        mRegistry->AddChild(parentID, mID);
    }

    void Entity::RemoveChild(EntityID childID) {
        mRegistry->RemoveChild(mID, childID);
    }

    void Entity::RemoveParent() {
        if(!HasComponent<Parent>()) throw std::runtime_error("this entity has no parent to be removed");
        EntityID parentID = GetComponent<Parent>().id;
        mRegistry->RemoveChild(parentID, mID);
    }
}