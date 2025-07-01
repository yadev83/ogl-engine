/**
 * @file component.hpp
 * @brief Définit un component de base dans le cadre d'un ECS
 */
#pragma once
#include <functional>

#include "entity.hpp"
#include "../scene/scene.hpp"

namespace Engine::ECS {
    /**
     * @brief Classe Component à dériver pour pouvoir être utilisée dans les systèmes ECS
     * 
     * Son intérêt réside dans le fait qu'elle peut stocker une entité et la renvoyer.
     * Dans le cadre de l'ECS, c'est le registre qui créé les components, et au passage, il appelle le setter pour lui attacher l'entité
     */
    class Component {
        friend class Registry;

        private:
            /** @brief L'entité attachée à ce component */
            Entity* mEntity;
        public:
            Component() = default;
            
            /**
             * @brief Attache l'entité au component
             * 
             * @param e 
             */
            void SetEntity(Entity e) { mEntity = new Entity(e); }
            /**
             * @brief Renvoie l'entité attachée
             * 
             * Utile dans les systèmes qui traitent avec des composants externes (physique par exemple).
             * On peut faire dans un callback (Component& other) => other.GetEntity() ...
             * 
             * @return Entity 
             */
            Entity& GetEntity() const { return *mEntity; }

            /**
             * @brief Renvoie le registre actuel
             * 
             * @return Registry& 
             */
            Registry& GetRegistry() const { return mEntity->GetRegistry(); }

            /**
             * @brief Renvoie la scène en cours
             * 
             * @return Scene::Scene& 
             */
            Scene::Scene& GetScene() const { return GetRegistry().GetScene(); }

            /**
             * @brief Renvoie l'instance d'application
             * 
             * @return App& 
             */
            App& GetApp() const { return GetRegistry().GetScene().GetApp(); }
    };
}