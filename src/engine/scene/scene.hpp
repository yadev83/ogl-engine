/**
 * @file scene.hpp
 * @brief Implémente une notion de "Scène" dans le moteur de jeu
 */
#pragma once

#include <queue>

#include "../defs.hpp"
#include "../ecs/registry.hpp"
#include "../ecs/entity.hpp"

#include "camera.hpp"
#include "../core/event.hpp"

// Forward declaration
namespace Engine { class App; }

namespace Engine::Scene {
    /**
     * @brief Les PrimitiveType sont des types d'entités préfabriquées
     * 
     * Le but est de passer un type d'entité primitive à la fonction Scene::CreateEntity(PrimitiveType type) pour pouvoir générer des prefabs simples.
     * On a souvent besoin de créer des éléments redondants, alors certains ont été préparés au sein même du moteur.
     */
    enum class PrimitiveType {
        Empty,
        Quad,
        Text
    };

    /**
     * @brief Classe qui encapsule un registre de composants ECS, et des fonctions de lifcecycle
     */
    class Scene {
        friend class App;

        private:
            /** @brief Pointeur vers l'application */
            App* mApp;
            /** @brief Pointeur vers la caméra principale de cette scène */
            ICamera* mCamera;

            /** @brief Registre ECS de la scène */
            ECS::Registry mRegistry;
            /** @brief Un event dispatcher spécifique à cette scène */
            Core::EventDispatcher mEventDispatcher;
            
        public:
            /**
             * @brief Construit une nouvelle scène
             * 
             */
            Scene();
            ~Scene() = default;

            /**
             * @brief Renvoie le registre ECS de la scène
             * 
             * @return ECS::Registry* 
             */
            ECS::Registry* GetRegistry();
            /**
             * @brief Renvoie la caméra principale de la scène
             * 
             * @return ICamera* 
             */
            ICamera *GetCamera();
            /**
             * @brief Renvoie une référence à l'Application courante
             * 
             * @return App& 
             */
            App& GetApp();

            // Event management
            /**
             * @brief Permet d'ajouter un listener aux évènements de la scène
             * 
             * @tparam EventType 
             * @param callback 
             */
            template<typename EventType>
            void AddEventListener(Core::EventDispatcher::EventCallback<EventType> callback) {
                mEventDispatcher.Subscribe<EventType>(callback);
            }

            /**
             * @brief Emet un évènement dans le contexte de la scène
             * 
             * @tparam EventType 
             * @param event 
             */
            template<typename EventType>
            void EmitEvent(EventType* event) {
                mEventDispatcher.Emit<EventType>(event);
            }
            
            // Entity management
            /**
             * @brief Créé une nouvelle entité dans le registre de la scène courante 
             * 
             * @return ECS::Entity 
             */
            ECS::Entity CreateEntity(PrimitiveType type = PrimitiveType::Empty);

            /**
             * @brief Créé une entité en lui ajoutant au passage une liste de components
             * 
             * @tparam Components 
             * @return ECS::Entity 
             */
            template<typename... Components>
            ECS::Entity CreateEntityWith() {
                ECS::Entity e = CreateEntity();
                (e.AddComponent<Components>(), ...); // C++17 fold expression
                return e;
            }
            /**
             * @brief Détruit une entité donnée
             * 
             * @param entityID 
             */
            void DestroyEntity(EntityID entityID);

            /**
             * @brief Callback appelé au lorsque cette scène est chargée comme scène courante
             */
            virtual void OnEnter() {}

            /**
             * @brief Callback appelé lorsqu'une autre scène prend la place de scène courante
             */
            virtual void OnExit() {}

            // Méthodes de cycle de vie, appelées par l'application pour bypass l'ECS si besoin
            virtual void OnInit() {}
            virtual void OnFixedUpdate(float deltaTime) {}
            virtual void OnUpdate(float deltaTime) {}
            virtual void OnRender(float alpha = 0.0f) {}
            virtual void OnUIRender() {}
            virtual void OnLateUpdate(float deltaTime) {}
    };
}