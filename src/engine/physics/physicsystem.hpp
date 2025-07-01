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
        public:
            /**
             * @brief Facteur d'echelle pour le système physique.
             * 
             * La simulation physique est basée sur les calculs qu'on pourrait faire dans le monde réel.
             * Mais un mètre dans la vraie vie n'est pas équivalent à un mètre dans l'application.
             * 
             * Ce facteur permet de dire combien d'unités (dans le jeu) sont équivalentes à un mètre (dans la vie)
             * Par défaut, la valeur est de 100u/m, si 1 unité = 1 pixel, ça veut dire que 100 pixels = 1 mètre.
             */
            float unitsPerMeter = 100.0f;

            /**
             * @brief Facteur de ralentissement dans le vide
             * 
             * Simule une perte de vélocité en l'absence de force, poru que les entités ne soient pas en mouvement perpétuel
             * Par défaut, le ralentissement est léger, mais sur quelques secondes, la vélocité va tendre vers 0.
             * Passer le dampingFactor à 1, désactive toute forme de ralentissement naturel
             */
            float dampingFactor = 0.99f;
        
            /**
             * @brief Méthode de cycle de vie de l'app qui appelle les méthodes privées
             * 
             * @param deltaTime Temps passé depuis la dernière frame
             */
            void OnFixedUpdate(float deltaTime) override;
    };
}