#include "registry.hpp"
#include "hierarchy.hpp"

namespace Engine::ECS {
    Registry::Registry() {
        // Initialise la queue d'ids entités disponibles
        for(EntityID i = 0; i < MAX_ENTITIES; ++i) {
            mAvailableEntityIDs.push(i);
        }
    }

    void Registry::Print() {
        LOG_INFO("==== Registry ====");
        LOG_INFO("next entityID : " + std::to_string(mAvailableEntityIDs.front()));
        LOG_INFO("storages : " + std::to_string(mStorages.size()));
    }

    Scene::Scene& Registry::GetScene() {
        return *mScene;
    }

    EntityID Registry::CreateEntity() {
        if(mAvailableEntityIDs.empty()) throw std::runtime_error("Registry::CreateEntity: no available entity ID was found");
        EntityID entityID = mAvailableEntityIDs.front();
        mAvailableEntityIDs.pop();
        mLivingEntities.insert(entityID);

        return entityID;
    }

    void Registry::DestroyEntity(EntityID entityID) {
        for(auto& [_, storage] : mStorages) {
            storage->Remove(entityID);
        }

        mTags[entityID].clear();
        mLivingEntities.erase(entityID);
        mAvailableEntityIDs.push(entityID);
    }

    bool Registry::IsValidEntity(EntityID entityID) {
        return mLivingEntities.find(entityID) != mLivingEntities.end();
    }

    void Registry::Clear() {
        for(auto& [typeID, storage] : mStorages) {
            storage->Clear();
            delete storage;
        }

        mStorages.clear();
        mTags.clear();

        for(EntityID i = 0; i < MAX_ENTITIES; ++i) {
            mAvailableEntityIDs.push(i);
        }
    }

    void Registry::AddTag(EntityID entityID, std::string tag) {
        mTags[entityID].emplace(tag);
    }

    bool Registry::HasTag(EntityID entityID, std::string tag) {
        return mTags[entityID].find(tag) != mTags[entityID].end();
    }

    std::set<std::string> Registry::GetTags(EntityID entityID) {
        return mTags[entityID];
    }

    EntityID Registry::GetEntityIDWithTag(std::string targetTag) {
        for(auto [id, tags] : mTags) {
            if(tags.find(targetTag) != tags.end())
                return id;
        }

        return -1;
    }

    std::vector<EntityID> Registry::GetEntityIDsWithTag(std::string targetTag) {
        std::vector<EntityID> result;

        for(auto [id, tags] : mTags) {
            if(tags.find(targetTag) != tags.end())
                result.push_back(id);
        }

        return result;
    }

    void Registry::AddChild(EntityID parentID, EntityID childID) {
        // Check if child already has the Parent component or add it, same for the Children component on the parent id
        if(!HasComponent<Parent>(childID)) AddComponent<Parent>(childID);
        if(!HasComponent<Children>(parentID)) AddComponent<Children>(parentID);

        // now set the parentID in the child
        GetComponent<Parent>(childID).id = parentID;

        // add the child to the list of the parent
        GetComponent<Children>(parentID).ids.emplace(childID);
    }

    void Registry::RemoveChild(EntityID parentID, EntityID childID, bool removeComponents) {
        if(!HasComponent<Parent>(childID)) throw std::runtime_error("given childID has no Parent component");
        if(!HasComponent<Children>(parentID)) throw std::runtime_error("given parentID has no Children component");

        if(GetComponent<Parent>(childID).id != parentID) throw std::runtime_error("given childID is not linked to the given parentID");
        if(!GetComponent<Children>(parentID).ids.contains(childID)) throw std::runtime_error("given parentID is not linked to the given childID");

        GetComponent<Parent>(childID).id = -1;
        if(removeComponents) RemoveComponent<Parent>(childID);

        GetComponent<Children>(parentID).ids.erase(childID);
        if(removeComponents && GetComponent<Children>(parentID).ids.size() == 0) RemoveComponent<Children>(parentID);
    }
}