/**
 * @file componentstorage.hpp
 * @brief 
 */
#pragma once

#include <unordered_map>

#include "../defs.hpp"

namespace Engine::ECS {
    /**
     * @brief Interface pour le système de stockage de composants
     * 
     */
    struct IComponentStorage {
        /**
         * @brief Interface pour supprimer un composant attaché à l'entité avec l'id donné
         * 
         * @param id EntityID de l'entité dont on supprime le composant
         */
        virtual void Remove(EntityID id) = 0;

        /**
         * @brief Nettoye un stockage de composant tout entier, sans distinction d'entityID
         * 
         */
        virtual void Clear() = 0;
        
        virtual ~IComponentStorage() = default;
    };

    /**
     * @brief Stockage de composant typé selon template, basé sur l'interface générique
     * 
     * @tparam T Type de composant que le stockage utilise
     */
    template<typename T>
    class ComponentStorage : public IComponentStorage {
        private:
            /** @brief Stockage de composants de type T, indexés selon l'identifiant de l'entité qui les possède */
            std::unordered_map<EntityID, T*> mComponents;
        
        public:
            /**
             * @brief Ajoute un nouveau composant au stockage, associé à l'entityID donné
             * 
             * @param entityID 
             * @param component 
             */
            void Add(EntityID entityID, T* component) {
                mComponents[entityID] = component;
            }

            /**
             * @brief Supprime le composant associé à l'entityID donné
             * 
             * @param entityID 
             */
            void Remove(EntityID entityID) override {
                mComponents.erase(entityID);
            }

            /**
             * @brief Nettoye le stockage de composants
             * 
             */
            void Clear() override {
                for(auto [_, t] : mComponents) { delete t; }
                mComponents.clear();
            }

            /**
             * @brief Récupère un pointeur vers le composant associé à l'entityID
             * 
             * @param entityID 
             * @return T* Un pointeur vers le composant demandé (nullptr si introuvable)
             */
            T* Get(EntityID entityID) {
                auto it = mComponents.find(entityID);
                return (it != mComponents.end()) ? it->second : nullptr;
            }

            /**
             * @brief Renvoie une référence vers le composant associé à l'entityID
             * 
             * @param entityID 
             * @return T& 
             */
            T& GetRef(EntityID entityID) {
                return *(mComponents.at(entityID));
            }

            /**
             * @brief Renvoie une réfèrence vers la table entière de stockage de composants
             * 
             * @return const std::unordered_map<EntityID, T>& 
             */
            const std::unordered_map<EntityID, T*>& GetRaw() const {
                return mComponents;
            }
    };
}