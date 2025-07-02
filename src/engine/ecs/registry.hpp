/**
 * @file registry.hpp
 * @brief Définit le rôle du Registre dans l'architecture ECS
 * 
 * Le registre est l'élément central du système, il fait le lien entre les stockages de composants, les entités, et les systèmes.
 * Chaque Scene a son propre registre ECS pour gérer ses entités et composants indépendament du reste.
 */
#pragma once

#include <unordered_map>
#include <queue>
#include <stdexcept>
#include <set>
#include <iostream>

#include "../defs.hpp"
#include "../constants.hpp"
#include "componentstorage.hpp"

#include <engine/core/logger.hpp>

// Forward declaration
namespace Engine::Scene { class Scene; }

namespace Engine::ECS {
    /** @brief Le prochain identifiant de composant pour les storages */
    inline size_t nextComponentTypeId = 0;
    
    /**
     * @brief Classe Registre qui stocke des composants et gère l'attribution des IDs d'entités 
     */
    class Registry {
        friend class Scene::Scene;

        // Templates qui permettent de checker si un composant possède un ::BaseType
        template<typename, typename = std::void_t<>>
        struct HasBaseType : std::false_type {};

        template<typename T>
        struct HasBaseType<T, std::void_t<typename T::BaseType>> : std::true_type {};
        
        template<typename T, bool = HasBaseType<T>::value>
        struct BaseOrSelf {
            using type = T;
        };

        template<typename T>
        struct BaseOrSelf<T, true> {
            using type = typename T::BaseType;
        };

        private:
            /** @brief Pointeur vers la scène qui possède le registre */
            Scene::Scene* mScene;
            
            /** @brief Une table qui associe un type de component à un stockage de composant associé */
            std::unordered_map<size_t, IComponentStorage*> mStorages;
            /** @brief Une table qui associe un set de tags à un EntityID pour tagger les entités */
            std::unordered_map<EntityID, std::set<std::string>> mTags;
            /** 
             * @brief Une liste des IDs d'entités disponibles de taille MAX_ENTITIES
             * 
             * Les entités détruites libèrent leur ID le rendant accessible aux nouvelles, créant un roulement.
             * Chaque registre à sa propre file d'IDs
             */
            std::queue<EntityID> mAvailableEntityIDs;

            /**
             * @brief Récupère un stockage de component, le créé s'il n'en existe pas pour ce type de composant
             * 
             * @tparam T Le type de component cherché
             * @return ComponentStorage<T>* Un pointeur sur le storage typé du composant
             */
            template<typename T>
            ComponentStorage<T>* GetOrCreateStorage() {
                size_t tid = GetComponentTypeID<T>();

                if(mStorages.find(tid) == mStorages.end()) {
                    mStorages[tid] = new ComponentStorage<T>();
                }

                return static_cast<ComponentStorage<T>*>(mStorages[tid]);
            }

            /**
             * @brief Récupère le stockage de component, sans le créer s'il n'existe pas
             * 
             * @tparam T Le type du component cherché
             * @return ComponentStorage<T>* Pointeur vers le storage typé du composant (nullptr si pas existant)
             */
            template<typename T>
            ComponentStorage<T>* GetStorage() {
                size_t tid = GetComponentTypeID<T>();
                
                if(mStorages.find(tid) != mStorages.end()) {
                    return static_cast<ComponentStorage<T>*>(mStorages[tid]);
                }

                return nullptr;
            }

        public:
            /**
             * @brief Créé un objet Registry.
             * 
             * Le constructeur remplis la file d'identifiants d'entités de 1 à MAX_ENTITIES
             */
            Registry();

            /**
             * @brief Affiche un print du registre dans la console
             * 
             */
            void Print();

            /**
             * @brief Renvoie la scène courante
             * 
             * @return Scene& 
             */
            Scene::Scene& GetScene();

            /* ECS Registry Public Section => Lifecycle of entities and systems */
            
            /**
             * @brief Créé une nouvelle entité et renvoie l'identifiant qui lui a été attribué
             * 
             * @return EntityID L'identifiant de l'entité nouvellement créée
             */
            EntityID CreateEntity();

            /**
             * @brief Créé une nouvelle entité avec les composants voulus
             * 
             * @tparam Components Les types de composants que l'on veut ajouter à l'entité
             * @return EntityID L'identifiant de l'entité nouvellement créée
             */
            template<typename... Components>
            EntityID CreateEntityWith() {
                EntityID id = CreateEntity();
                (AddComponent<Components>(id), ...); // C++17 fold expression
                return id;
            }

            /**
             * @brief Supprime l'entité référencée par l'identifiant donné
             * 
             * A pour effet de supprimer les composants rattachés en supprimant les entrées des storages
             * @param entityID L'id de l'entité à supprimer
             */
            void DestroyEntity(EntityID entityID);

            /**
             * @brief Nettoie le registre dans son entiereté.
             * 
             * A pour effet de nettoyer tous les componentStorages, y compris la liste de storages du registre.
             * Nettoie ensuite les tags d'entités, et rend tous les EntityIDs possibles à nouveau disponibles.
             */
            void Clear();

            /**
             * @brief Renvoie un tableau d'EntityIDs sous condition
             * 
             * Les entités renvoyées doivent toutes référencer les composants demandés pour être valides
             * 
             * @tparam TComponents Liste de composants pour filtrer les entités
             * @return std::vector<EntityID> Tableau d'entités respectant la liste de composants
             */
            template <typename... TComponents>
            std::vector<EntityID> GetEntityIDsWith() {
                std::vector<EntityID> result;

                if(sizeof...(TComponents) == 0) return result;

                // On récupère le premier stockage pour itérer sur une base minimale
                auto* baseStorage = GetStorage<std::tuple_element_t<0, std::tuple<TComponents...>>>();
                if(!baseStorage) return result;

                for (const auto& [entityID, _] : baseStorage->GetRaw()) {
                    if ((HasComponent<TComponents>(entityID) && ...)) {
                        result.push_back(entityID);
                    }
                }

                return result;
            }

            /**
             * @brief Renvoie le premier EntityID trouvé pour le tag donné
             * 
             * @param targetTag Tag à chercher
             * @return EntityID Le premier EntityID qui correspond à la demande dans la liste
             */
            EntityID GetEntityIDWithTag(std::string targetTag);
            
            /**
             * @brief Renvoie une liste d'entityID qui possèdent le tag donné
             * 
             * @param targetTag Le tag à chercher
             * @return std::vector<EntityID> Liste d'id d'entités qui correspondent
             */
            std::vector<EntityID> GetEntityIDsWithTag(std::string targetTag);

            /* COMPONENT MANAGER PUBLIC SECTION */
            /**
             * @brief Ajoute un nouveau component
             * 
             * @tparam T Le type de component à ajouter
             * @tparam Args Liste de types d'arguments pour construire le component
             * @param entityID L'EntityID qui référence le nouveau composant
             * @param args Les arguments pour créer le composant
             */
            template <typename T, typename... Args>
            void AddComponent(EntityID entityID, Args&&... args) {
                using Base = typename BaseOrSelf<T>::type;

                GetOrCreateStorage<Base>()->Add(entityID, new T(std::forward<Args>(args)...));
            }

            /**
             * @brief Supprime le composant de type T associé à l'entité à l'EntityID donné
             * 
             * @tparam T Le type de component à supprimer
             * @param entityID L'entityID qui référence le component à supprimer
             */
            template <typename T>
            void RemoveComponent(EntityID entityID) {
                using Base = typename BaseOrSelf<T>::type;

                auto storage = GetStorage<Base>();
                if (storage) {
                    storage->Remove(entityID);
                }
            }

            /**
             * @brief Renvoie une identifiant unique pour chaque type de composant
             * 
             * La première fois qu'un type T appelle cette méthode, elle créé une constante avec un id unique incrémental
             * Les appels subséquents pour le même type T ne pouvant pas modifier une constante, se contentent de renvoyer l'id stocké (static)
             * 
             * @tparam T 
             * @return size_t 
             */
            template <typename T>
            inline size_t GetComponentTypeID() {
                static const size_t tid = nextComponentTypeId++;
                return tid;
            }

            /**
             * @brief Renvoie le component référencé par l'EntityID donné
             * 
             * @tparam T Le type de component voulu
             * @param entityID L'id de l'entité dont on cherche le composant
             * @return T& Une référence au component demandé
             */
            template <typename T>
            T& GetComponent(EntityID entityID) {
                using Base = typename BaseOrSelf<T>::type;

                auto* storage = GetStorage<Base>();
                if(!storage) throw std::runtime_error("Registry::GetComponent<T>: no storage for component type");
                return static_cast<T&>(storage->GetRef(entityID));
            }

            /**
             * @brief Vérifie si une entité possède un component
             * 
             * @tparam T Le type de component à chercher
             * @param entityID L'id d'entité dont on veut savoir si elle a un composant 
             * @return true Si l'entité possède un component de type T 
             * @return false Si l'entité ne possède pas de component de type T
             */
            template <typename T>
            bool HasComponent(EntityID entityID) {
                using Base = typename BaseOrSelf<T>::type;

                auto *storage = GetStorage<Base>();
                return storage && !!storage->Get(entityID);
            }

            // Tags management
            /**
             * @brief Ajoute un tag à une entité
             * 
             * @param entityID 
             * @param tag 
             */
            void AddTag(EntityID entityID, std::string tag);

            /**
             * @brief Vérifie si une entité possède un tag
             * 
             * @param entityID 
             * @param tag 
             * @return true 
             * @return false 
             */
            bool HasTag(EntityID entityID, std::string tag);
    };
}