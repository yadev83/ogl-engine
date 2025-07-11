/**
 * @file physicsystem.hpp
 * @brief Définit le comportement d'un système physique, et des objets qui y sont soumis
 */
#pragma once

#include "../ecs/system.hpp"
#include "../scene/transform.hpp"
#include "../scene/behaviour.hpp"

#include "rigidbody.hpp"
#include "collider.hpp"
#include "manifold.hpp"
#include "spatialhash.hpp"
#include "aabb.hpp"
#include "obb.hpp"

namespace Engine::Physics {
    /**
     * @brief Le Système physique dans son ensemble définit ici.
     * 
     * La méthode onFixedUpdate est appelée dans le cycle de vie de l'app et gère les collisions, leur résolution, 
     * ainsi que la gestion de la dynamique (application de vitesses, mouvements).
     */
    class PhysicSystem : public ECS::System {
        private:
            int maxIterations = MAX_PHYSICS_ITERATIONS;
            double physicsTime = 0.0f;

            /** @brief Un vecteur de force gravitationnelle à appliquer aux rigidbodies qui y sont soumis
             * 
             * La valeur par défaut est de 9.81
             * Elle peut être modifiée pendant l'execution sans souci, pour mieux s'adapter à l'échelle du jeu si besoin.
             */
            glm::vec2 gravity = {0.0f, 9.81f};

            /**
             * @brief Applique les mouvements et les lois de la dynamique à tous les objets
             * 
             * @param dt Le temps écoulé depuis la dernière frame
             */
            void ApplyMotion(float dt);
            /**
             * @brief Détecte et résouds les collisions entre tous les colliders possibles
             * 
             * @param dt Le temps écoule depuis la dernière frame
             */
            void ResolveCollisions(float dt);

            /**
             * @brief Détecte la collision entre deux bounding boxes AABB
             * 
             * @param a Bounding Box d'un Collider "a"
             * @param b Bounding Box d'un autre Collider "b"
             * @return CollisionManifold Le manifeste de collision entre a et b pour la frame actuelle
             */
            CollisionManifold CheckAABBCollision(const AABB& a, const AABB& b);
            /**
             * @brief Détecte la collision entre deux bounding boxes OBB
             * 
             * @param a Bounding Box d'un Collider "a"
             * @param b Bounding Box d'un autre Collider "b"
             * @return CollisionManifold Le manifeste de collision entre a et b pour la frame actuelle
             */
            CollisionManifold CheckOBBCollision(const OBB& a, const OBB& b);
            /**
             * @brief Réfléchis la vélocité passée en paramètre dans la direction opposé (rebond)
             * 
             * @param velocity La vélocité à "inverser"
             * @param normal La normale de la collision selon laquelle on veut rebondir
             * @return glm::vec2 
             */
            glm::vec2 Reflect(const glm::vec2& velocity, const glm::vec2& normal);

            /**
             * @brief Génération d'un spatial hash à partir d'entités données
             * 
             * @param collidableIDs Les identifiants des entités que l'on considère comme collidables
             * @return SpatialHash 
             */
            SpatialHash BuildSpatialHash(const std::vector<EntityID>& collidableIDs);

            /**
             * @brief Génère des paires d'entités pour lesquelles on doit checker les collisions
             * Le tout en se basant sur le contenu d'une spatialHash
             * 
             * @return std::vector<std::pair<EntityID, EntityID>> 
             */
            std::vector<std::pair<EntityID, EntityID>> GenerateBroadPhasePairs(SpatialHash spatialHash);
        public:        
            /**
             * @brief Méthode de cycle de vie de l'app qui appelle les méthodes privées
             * 
             * @param deltaTime Temps passé depuis la dernière frame
             */
            void OnFixedUpdate(float deltaTime) override;
    };
}