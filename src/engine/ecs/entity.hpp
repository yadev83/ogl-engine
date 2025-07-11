/**
 * @file entity.hpp
 * @brief Décrit le fonctionnement de l'entité dans une architecture ECS
 * 
 * Le rôle de l'entité est de fournir un identifiant unique autour duquel on référence des composants.
 */
#pragma once

#include "../defs.hpp"
#include "registry.hpp"
#include "componentstorage.hpp"

#include <set>

namespace Engine::ECS {
    /**
     * @brief Une entité est l'élément "atomique" utilisé dans l'architecture ECS pour référencer les composants
     */
    class Entity {
        private:
            /** @brief L'identifiant de cette entité, utilisé dans les registres */
            EntityID mID;
            /** @brief Une référence vers le registre qui a créé cette entité */
            Registry* mRegistry;

        public:
            /**
             * @brief Construit une nouvelle entité
             * 
             * @param id 
             * @param registry 
             */
            Entity(EntityID id = -1, Registry* registry = nullptr);
            /**
             * @brief Retourne l'identifiant EntityID de cette
             * 
             * @return EntityID 
             */
            EntityID GetID() const;

            /**
             * @brief Renvoie true si l'entité est valide, false sinon
             * 
             * Les critères sont : 
             * - mID > 0
             * - mRegistry != nullptr
             * - mRegistry->IsValidEntity(mID) == true
             * 
             * @return true 
             * @return false 
             */
            bool IsValid() const;

            /**
             * @brief Renvoie une chaine de caractères qui représente cette entité
             * 
             * @return std::string 
             */
            std::string ToString() const;

            /**
             * @brief Renvoie le registre parent de cet entité
             * 
             * @return Registry& 
             */
            Registry& GetRegistry() const;

            /**
             * @brief Ajoute un tag à l'entité actuelle
             * 
             * Cette méthode est un wrapper sur Registry->AddTag(EntityID, std::string)
             * @param tag 
             */
            void AddTag(std::string tag);
            /**
             * @brief Check si l'entité possède un tag
             * 
             * Cette méthode est un wrapper autour de Registry->HasTag(EntityID, std::string)
             * @param tag 
             * @return true Si l'entité possède le tag
             * @return false Si l'entité ne possède pas le tag
             */
            bool HasTag(std::string tag);

            /**
             * @brief Ajoute un component de type T au registre, référencé sous l'entityID de cette entité
             * 
             * Cette méthode est un wrapper autour de registry->AddComponent<T>(EntityID)
             * @tparam T Le type du composant à ajouter
             * @tparam Args
             * @param args Les arguments de constructions du component
             * @return T& Une référence vers le composant créé
             */
            template<typename T, typename... Args>
            T& AddComponent(Args&&... args) {
                mRegistry->AddComponent<T>(mID, std::forward<Args>(args)...);
                T& component = mRegistry->GetComponent<T>(mID);
                if constexpr(requires(T t, Entity e) { t.SetEntity(e); }) {
                    component.SetEntity(*this);
                }

                return component;
            }

            /**
             * @brief Supprime le composant de type T référencé par l'EntityID de cette entité
             * 
             * Wrapper autour de registry->RemoveComponent(id)
             * @tparam T 
             */
            template<typename T>
            void RemoveComponent() {
                mRegistry->RemoveComponent<T>(mID);
            }

            /**
             * @brief Retourne le premier composant de type T associé à cette entité
             * 
             * Wrapper autour de registry->GetComponent(id)
             * @tparam T 
             * @return T& Une référence au composant demandé
             */
            template<typename T>
            T& GetComponent() {
                return mRegistry->GetComponent<T>(mID);
            }

            /**
             * @brief Retourne le tableau de composants de type T associé à cette entité
             * 
             * Wrapper autour de registry->GetComponents(id) 
             * @tparam T 
             * @return std::vector<T*> Un tableau de pointeurs vers les composants demandés
             */
            template<typename T>
            std::vector<T*> GetComponents() {
                return mRegistry->GetComponents<T>(mID);
            }

            /**
             * @brief Vérifie si l'entité possède le component demandé
             * 
             * @tparam T
             * @return bool
             */
            template<typename T>
            bool HasComponent() {
                return mRegistry->HasComponent<T>(mID);
            }
    };
}